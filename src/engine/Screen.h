#pragma once

#include "ScreenPartition.h"

/**
 * @brief Class representing the game screen
 */
class Screen
{
public:
	Screen(int32_t screenWidth, int32_t screenHeight,
		   uint16_t rowCount, uint16_t columnCount);
	~Screen();

	void SetSize(int32_t width, int32_t height);

	enum EInformType
	{
		EIT_CREATED,
		EIT_MOVED,
		EIT_DELETED
	};

	/**
	 * @brief Informs all partitions of particular UI events
	 */
	void Inform(UIElement*, EInformType);

	/**
	 * @brief Gets the UIElement at the current screen position, if any
	 */
	UIElement *FindElementAt(double x, double y);
private:
	int32_t m_Width;
	int32_t m_Height;

	uint16_t m_RowCount;
	uint16_t m_ColumnCount;
	int32_t m_PartitionCount;

	float m_WidthScaleFactor;
	float m_HeightScaleFactor;

	std::vector<ScreenPartition> m_Partitions;
};
