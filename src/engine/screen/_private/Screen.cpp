#include "engine/screen/Screen.h"

#include "core/BaseTypes.h"
#include "engine/game/CallbackContext.h"
#include "engine/ui/UIComponent.h"

namespace tetrad {

#define GET_COL_FROM_X(x)          (uint8_t(m_WidthScaleFactor * (x)))
#define GET_ROW_FROM_Y(y)          (uint8_t(m_HeightScaleFactor * (y)))
#define GET_INDEX_UNSAFE(row, col) (int(col) + int(row) * m_PartitionCols)
#define GET_INDEX(row, col) \
  std::min(std::max(0, GET_INDEX_UNSAFE(row, col)), m_PartitionCount - 1)

#define SAFE_ROW(row) std::min(row, uint8_t(m_PartitionRows - 1))
#define SAFE_COL(col) std::min(col, uint8_t(m_PartitionCols - 1))

#define SET_PARTITIONS_ARRAY(partitions, rectBound) \
  {                                                 \
    uint8_t xMin = SAFE_COL(rectBound.points[0].X); \
    uint8_t xMax = SAFE_COL(rectBound.points[1].X); \
    uint8_t yMax = SAFE_ROW(rectBound.points[1].Y); \
    uint8_t yMin = SAFE_ROW(rectBound.points[0].Y); \
    int prevI = -1;                                 \
    for (uint8_t y = yMin; y <= yMax; ++y)          \
    {                                               \
      for (uint8_t x = xMin; x <= xMax; ++x)        \
      {                                             \
        int i = GET_INDEX(y, x);                    \
        if (i > prevI) partitions.push_back(i);     \
        prevI = i;                                  \
      }                                             \
    }                                               \
  }

enum EBitPositions
{
  EBP_FULLSCREEN,
  EBP_RESIZABLE,
  EBP_VSYNC
};

ScreenAttributes::ScreenAttributes(uint32_t width, uint32_t height, bool fullscreen,
                                   bool isResizable, bool useVsync, uint8_t samples,
                                   uint8_t partitionRows, uint8_t partitionCols,
                                   std::string title)
    : m_Width(width),
      m_Height(height),
      m_Flags(0),
      m_SampleCount(samples),
      m_PartitionRows(partitionRows),
      m_PartitionCols(partitionCols),
      m_Title(title)
{
  m_Flags |= (fullscreen << EBP_FULLSCREEN);
  m_Flags |= (isResizable << EBP_RESIZABLE);
  m_Flags |= (useVsync << EBP_VSYNC);
}

Screen::Screen() : m_IsInitialized(false) {}

Screen::~Screen() { Shutdown(); }

bool Screen::Initialize(const ScreenAttributes &attributes)
{
  if (m_IsInitialized)
  {
    return false;
  }

  // Set screen data
  *(ScreenAttributes *)this = attributes;
  m_PartitionCount = m_PartitionRows * m_PartitionCols;
  m_WidthScaleFactor = float(m_PartitionCols) / m_Width;
  m_HeightScaleFactor = float(m_PartitionRows) / m_Height;

  // Create window
  GLFWmonitor *pMonitor = nullptr;
  if (m_Flags & ((uint8_t)1 << EBP_FULLSCREEN))
  {
    pMonitor = glfwGetPrimaryMonitor();
  }

  glfwWindowHint(GLFW_SAMPLES, m_SampleCount);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, !!(m_Flags & (1 << EBP_RESIZABLE)));

  m_pWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), pMonitor, NULL);

  if (!m_pWindow)
  {
    LOG_ERROR("Failed to create the GLFW window!");
    return false;
  }

  glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);

  // TODO - Only do this if we're making the context current!
  glfwMakeContextCurrent(m_pWindow);
  glfwSwapInterval(!!(m_Flags & (1 << EBP_VSYNC)));
  glViewport(0, 0, m_Width, m_Height);

  m_RenderList.Initialize();

  // Create partitions
  m_Partitions.reserve(m_PartitionCount);
  for (int i = 0; i < m_PartitionCount; ++i)
  {
    m_Partitions.emplace_back();
    m_Partitions.back().m_SearchList.Initialize();
  }

  m_IsInitialized = true;
  return true;
}

void Screen::Shutdown()
{
  if (m_IsInitialized)
  {
    m_Partitions.clear();

    glfwSetWindowShouldClose(m_pWindow, GLFW_TRUE);
    m_IsInitialized = false;
  }
}

void Screen::SetSize(int32_t width, int32_t height)
{
  m_Width = width;
  m_Height = height;

  m_WidthScaleFactor = float(m_PartitionCols) / width;
  m_HeightScaleFactor = float(m_PartitionRows) / height;
}

void Screen::Inform(UIComponent *pElem, EInformType informType)
{
  // Get current partitions
  if (informType == EIT_DELETED)
  {
    // Create array of partitions this element is in
    std::vector<int> partitions;
    UIRectangleBound_t rBound = pElem->m_PartitionRectangle;
    SET_PARTITIONS_ARRAY(partitions, rBound);

    // Update render list
    m_RenderList.Remove(pElem->m_RenderNode, pElem->m_Priority);

    // Inform partitions of deletion
    for (int partition : partitions)
    {
      DEBUG_ASSERT(partition < m_PartitionCount);
      m_Partitions[partition].InformDeleted(pElem);
    }

    // Ensure callback context is not caching current element
    if (CallbackContext::GetCachedUI() == pElem)
    {
      CallbackContext::ClearCachedUI();
    }

    return;
  }

  // Create "partition rectangle"
  screenBound_t screenBounds = pElem->GetScreenBounds();
  screenBounds.points[0].X *= m_Width;
  screenBounds.points[1].X *= m_Width;
  screenBounds.points[0].Y *= m_Height;
  screenBounds.points[1].Y *= m_Height;

  uint8_t sX = GET_COL_FROM_X(screenBounds.points[0].X);
  uint8_t sY = GET_ROW_FROM_Y(screenBounds.points[0].Y);
  uint8_t eX = GET_COL_FROM_X(screenBounds.points[1].X);
  uint8_t eY = GET_ROW_FROM_Y(screenBounds.points[1].Y);
  UIRectangleBound_t rect(sX, sY, eX, eY);

  std::vector<int> newPartitions;
  SET_PARTITIONS_ARRAY(newPartitions, rect);

  if (informType == EIT_CREATED)
  {
    // Store partition rectangle in element
    pElem->m_PartitionRectangle.value = rect.value;

    // Update render list
    m_RenderList.PushBack(pElem->m_RenderNode, pElem->m_Priority);

    // Inform created for relevant partitions
    for (int partition : newPartitions)
    {
      DEBUG_ASSERT(partition < m_PartitionCount);
      m_Partitions[partition].InformCreated(pElem);
    }
  }
  else  // informType == EIT_UPDATED
  {
    // Update render list
    m_RenderList.Remove(pElem->m_RenderNode, pElem->m_Priority);
    m_RenderList.PushBack(pElem->m_RenderNode, pElem->m_Priority);

    // Retrieve element's partition rectangle
    UIRectangleBound_t rBound = pElem->m_PartitionRectangle;

    // If difference, inform created & deleted as appropriate, then
    // update element's partition rectangle
    if (rBound.value != rect.value)
    {
      std::vector<int> oldPartitions;
      SET_PARTITIONS_ARRAY(oldPartitions, rBound);

      size_t i = 0, j = 0;
      uint8_t oldIndex, newIndex;
      while (i < oldPartitions.size() && j < newPartitions.size())
      {
        oldIndex = oldPartitions[i];
        newIndex = newPartitions[j];
        if (oldIndex < newIndex)
        {
          // oldIndex not contained in new partition list
          DEBUG_ASSERT(oldIndex < m_PartitionCount);
          m_Partitions[oldIndex].InformDeleted(pElem);
          ++i;
        }
        else if (oldIndex > newIndex)
        {
          // newIndex not contained in old partition list
          DEBUG_ASSERT(newIndex < m_PartitionCount);
          m_Partitions[newIndex].InformCreated(pElem);
          ++j;
        }
        else
        {
          // partition already contains relevant index
          DEBUG_ASSERT(oldIndex < m_PartitionCount);
          m_Partitions[oldIndex].InformUpdated(pElem);
          ++i;
          ++j;
        }
      }

      // Handle leftover partitions
      while (i < oldPartitions.size())
      {
        DEBUG_ASSERT(oldPartitions[i] < m_PartitionCount);
        m_Partitions[oldPartitions[i++]].InformDeleted(pElem);
      }
      while (j < newPartitions.size())
      {
        DEBUG_ASSERT(newPartitions[j] < m_PartitionCount);
        m_Partitions[newPartitions[j++]].InformCreated(pElem);
      }

      // Update element's partition rectangle
      pElem->m_PartitionRectangle.value = rect.value;
    }
    else
    {
      // Update all partitions
      for (int partition : newPartitions)
      {
        DEBUG_ASSERT(partition < m_PartitionCount);
        m_Partitions[partition].InformUpdated(pElem);
      }
    }

    // Update all children UI as well
    for (UIComponent *pUI : pElem->GetChildren())
    {
      Inform(pUI, EIT_UPDATED);
    }
  }
}

UIComponent *Screen::FindElementAt(double x, double y)
{
  // Find proper partition
  uint8_t row = GET_ROW_FROM_Y(y);
  uint8_t col = GET_COL_FROM_X(x);

  int i = GET_INDEX(row, col);

  // Normalize coords
  x /= m_Width;
  y /= m_Height;

  return m_Partitions[i].FindElementAt(x, y);
}

}  // namespace tetrad
