#include "engine/screen/ScreenPartition.h"

#include "engine/screen/Screen.h"
#include "engine/ui/UIComponent.h"

namespace tetrad
{

using UINode = ExternalLinkedNode<UIComponent>;

ScreenPartition::ScreenPartition() {}

ScreenPartition::~ScreenPartition()
{
  LinkedNode<UIComponent> *pUINode = m_SearchList.Last();
  while (pUINode)
  {
    UINode *pNode = linked_node_owner(pUINode, UINode, node);
    pUINode = m_SearchList.Prev(*pUINode);

    delete pNode;
  }
}

void ScreenPartition::InformCreated(UIComponent *pElem)
{
  UINode *pUINode = new UINode(nullptr, nullptr, pElem);
  m_SearchList.PushBack(pUINode->node, pElem->GetPriority());
}

void ScreenPartition::InformUpdated(UIComponent *pElem)
{
  LinkedNode<UIComponent> *pNode = FindNode(pElem);
  DEBUG_ASSERT(pNode);
  m_SearchList.Remove(*pNode, pElem->GetPriority());
  m_SearchList.PushBack(*pNode, pElem->GetPriority());
}

void ScreenPartition::InformDeleted(UIComponent *pElem)
{
  LinkedNode<UIComponent> *pNode = FindNode(pElem);
  DEBUG_ASSERT(pNode);
  m_SearchList.Remove(*pNode, pElem->GetPriority());

  UINode *pUINode = linked_node_owner(pNode, UINode, node);
  DEBUG_ASSERT(pUINode);
  delete pUINode;
}

UIComponent *ScreenPartition::FindElementAt(double x, double y)
{
  LinkedNode<UIComponent> *pUINode = m_SearchList.Last();
  while (pUINode)
  {
    UIComponent *pElem = ((UINode *)pUINode)->pObj;
    DEBUG_ASSERT(pElem);

    screenBound_t &&screenBound = pElem->GetScreenBounds();
    if (screenBound.points[0].X <= x && x <= screenBound.points[1].X)
    {
      if (screenBound.points[0].Y <= y && y <= screenBound.points[1].Y)
      {
        return pElem;
      }
    }

    pUINode = m_SearchList.Prev(*pUINode);
  }

  return nullptr;
}

LinkedNode<UIComponent> *ScreenPartition::FindNode(UIComponent *pElem)
{
  LinkedNode<UIComponent> *pUINode = m_SearchList.Last(pElem->GetPriority());
  while (pUINode)
  {
    UINode *pNode = linked_node_owner(pUINode, UINode, node);
    if (pNode->pObj == pElem)
    {
      return pUINode;
    }

    pUINode = m_SearchList.Prev(*pUINode);
  }

  return nullptr;
}

}  // namespace tetrad
