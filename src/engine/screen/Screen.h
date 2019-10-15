#pragma once

#include <string>
#include <vector>

#include "core/BaseTypes.h"
#include "core/GlTypes.h"
#include "core/PriorityLinkedList.h"
#include "engine/screen/ScreenPartition.h"

namespace tetrad {

/**
 * @brief Struct to initialize screen
 */
struct ScreenAttributes
{
  ScreenAttributes(uint32_t width, uint32_t height, bool fullscreen = false,
                   bool isResizable = false, bool useVsync = false, uint8_t samples = 4,
                   uint8_t partitionRows = 4, uint8_t partitionCols = 4,
                   std::string title = "Test Window");

  ScreenAttributes() {}

  uint32_t m_Width;
  uint32_t m_Height;

  uint8_t m_Flags;
  uint8_t m_SampleCount;
  uint8_t m_PartitionRows;
  uint8_t m_PartitionCols;

  std::string m_Title;
};

/**
 * @brief Class representing the game screen
 *
 * Partitions the screen into sections for optimization purposes, and also
 * provides screen dimensions to the rest of the system
 */
class Screen : public ScreenAttributes
{
 public:
  Screen();
  ~Screen();

  bool Initialize(const ScreenAttributes &attributes);
  void Shutdown();

  void SetSize(int32_t width, int32_t height);

  enum EInformType
  {
    EIT_CREATED,
    EIT_DELETED,
    EIT_UPDATED
  };

  /**
   * @brief Informs all partitions of particular UI events
   */
  void Inform(UIComponent *, EInformType);

  /**
   * @brief Gets the UIComponent at the current screen position, if any
   */
  UIComponent *FindElementAt(double x, double y);

  inline const uint32_t &GetWidth() const { return m_Width; }
  inline const uint32_t &GetHeight() const { return m_Height; }

  inline GLFWwindow *GetWindow() { return m_pWindow; }

  inline const LinkedList<UIComponent> &GetRenderList() const { return m_RenderList; }

 private:
  bool m_IsInitialized;

  int m_PartitionCount;

  float m_WidthScaleFactor;
  float m_HeightScaleFactor;

  GLFWwindow *m_pWindow;

  std::vector<ScreenPartition> m_Partitions;

  PriorityLinkedList<UIComponent, UI_PRIORITY_COUNT> m_RenderList;
};

}  // namespace tetrad
