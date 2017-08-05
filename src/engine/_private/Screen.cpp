#include "Screen.h"
#include "UIComponent.h"
#include "Base.h"

#define GET_COL_FROM_X(x) (uint8_t(m_WidthScaleFactor * (x)))
#define GET_ROW_FROM_Y(y) (uint8_t(m_HeightScaleFactor * (y)))
#define GET_INDEX_UNSAFE(row, col) (int(col) + int(row)*m_ColumnCount)
#define GET_INDEX(row, col) std::min(std::max(0, GET_INDEX_UNSAFE(row,col)), \
									 m_PartitionCount-1)

#define SAFE_ROW(row) std::min(row, uint8_t(m_RowCount-1))
#define SAFE_COL(col) std::min(col, uint8_t(m_ColumnCount-1))

#define SET_PARTITIONS_ARRAY(partitions, rectBound)						\
	{																	\
		uint8_t xMax = SAFE_COL(rectBound.points[1].X);					\
		uint8_t yMax = SAFE_ROW(rectBound.points[1].Y);					\
		uint8_t yMin = SAFE_ROW(rectBound.points[0].Y);					\
		int prevI = -1;													\
		for(uint8_t x = SAFE_COL(rectBound.points[0].X);				\
			x <= xMax; ++x){											\
			for(uint8_t y = yMin; y <= yMax; ++y){						\
				int i = GET_INDEX(y, x);								\
				if(i > prevI)											\
					partitions.push_back(i);							\
				prevI = i;												\
			}															\
		}																\
	}

Screen::Screen(int32_t screenWidth, int32_t screenHeight,
			   uint8_t rowCount, uint8_t columnCount) :
	m_Width(screenWidth), m_Height(screenHeight),
	m_RowCount(rowCount), m_ColumnCount(columnCount),
	m_PartitionCount(rowCount * columnCount),
	m_WidthScaleFactor(float(columnCount)/screenWidth),
	m_HeightScaleFactor(float(rowCount)/screenHeight)
{
	for(int i = 0; i < m_PartitionCount; ++i)
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

void Screen::Inform(UIComponent *pElem, EInformType informType)
{
	// Get current partitions
	if(informType == EIT_DELETED)
	{
		// Create array of partitions this element is in
		std::vector<int> partitions;
		UIRectangleBound_t rBound = pElem->m_PartitionRectangle;
		SET_PARTITIONS_ARRAY(partitions, rBound);

		// Get inform deleted
		for(int partition : partitions)
		{
			m_Partitions[partition].InformDeleted(pElem);
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

	if(informType == EIT_CREATED)
	{
		// Store partition rectangle in element
		pElem->m_PartitionRectangle.value = rect.value;

		// Inform created for relevant partitions
		for(int partition : newPartitions)
		{
			m_Partitions[partition].InformCreated(pElem);
		}
	}
	else // informType == EIT_UPDATED
	{
		// Retrieve element's partition rectangle
		UIRectangleBound_t rBound = pElem->m_PartitionRectangle;

		// If difference, inform created & deleted as appropriate, then
		// update element's partition rectangle
		if(rBound.value != rect.value)
		{
			std::vector<int> oldPartitions;
			SET_PARTITIONS_ARRAY(oldPartitions, rBound);

			size_t i = 0, j = 0;
			uint8_t oldIndex, newIndex;
			while(i < oldPartitions.size() && j < newPartitions.size())
			{
				oldIndex = oldPartitions[i]; newIndex = newPartitions[j];
				if(oldIndex < newIndex)
				{
					// oldIndex not contained in new partition list
					m_Partitions[oldIndex].InformDeleted(pElem);
					++i;
				}
				else if(oldIndex > newIndex)
				{
					// newIndex not contained in old partition list
					m_Partitions[newIndex].InformCreated(pElem);
					++j;
				}
				else
				{
					// partition already contains relevant index
					m_Partitions[oldIndex].InformUpdated(pElem);
					++i; ++j;
				}
			}

			// Handle leftover partitions
			while(i < oldPartitions.size())
			{
				m_Partitions[oldPartitions[i++]].InformDeleted(pElem);
			}
			while(j < newPartitions.size())
			{
				m_Partitions[newPartitions[j++]].InformCreated(pElem);
			}

			// Update element's partition rectangle
			pElem->m_PartitionRectangle.value = rect.value;
		}
		else
		{
			// Update all partitions
			for(int partition : newPartitions)
			{
				m_Partitions[partition].InformUpdated(pElem);
			}
		}

		// Update all children UI as well
		for(UIComponent *pUI : pElem->GetChildren())
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
