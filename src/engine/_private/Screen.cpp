#include "Screen.h"

Screen::Screen(int32_t screenWidth, int32_t screenHeight,
			   uint16_t rowCount, uint16_t columnCount) :
	m_Width(screenWidth), m_Height(screenHeight),
	m_RowCount(rowCount), m_ColumnCount(columnCount),
	m_PartitionCount(rowCount * columnCount),
	m_WidthScaleFactor(float(columnCount)/screenWidth),
	m_HeightScaleFactor(float(rowCount)/screenHeight)
{
	for(int32_t i = 0; i < m_PartitionCount; ++i)
	{
		m_Partitions.push_back(ScreenPartition());
	}
}

Screen::~Screen()
{
	m_Partitions.clear();
}

void Screen::SetSize(int32_t width, int32_t height)
{
	m_Width = width;
	m_Height = height;

	m_WidthScaleFactor = float(m_ColumnCount)/width;
	m_HeightScaleFactor = float(m_RowCount)/height;
}

void Screen::Inform(UIElement *pElem, EInformType informType)
{
	// TODO perhaps store partition ptrs/#s in UIElement for creation & move types
	// so that we only need to inform changed partitions
	switch(informType)
	{
	case EIT_CREATED:
		for(int32_t i = 0; i < m_PartitionCount; ++i)
		{
			m_Partitions[i].InformCreated(pElem);
		}
		break;
	case EIT_MOVED:
		// Inform all partitions, since the element may have moved out of
		// a partition!
		for(int32_t i = 0; i < m_PartitionCount; ++i)
		{
			m_Partitions[i].InformMoved(pElem);
		}
		break;
	case EIT_DELETED:
		for(int32_t i = 0; i < m_PartitionCount; ++i)
		{
			m_Partitions[i].InformDeleted(pElem);
		}
		break;
	}
}

UIElement *Screen::FindElementAt(double x, double y)
{
	// Find proper partition
	int32_t col = int32_t(m_WidthScaleFactor * x);
	int32_t row = int32_t(m_HeightScaleFactor * y); // TODO this might be inverted

	int32_t i = col + (row * m_ColumnCount);

	return m_Partitions[i].FindElementAt(x, y);
}
