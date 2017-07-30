#pragma once

#include "ScreenPartition.h"

/**
 * @brief Class representing the game screen
 *
 * Partitions the screen into sections for optimization purposes, and also
 * provides screen dimensions to the rest of the system
 */
class Screen
{
public:
	Screen(int32_t screenWidth, int32_t screenHeight,
		   uint8_t rowCount, uint8_t columnCount);
	~Screen();

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
	void Inform(UIComponent*, EInformType);

	/**
	 * @brief Gets the UIComponent at the current screen position, if any
	 */
	UIComponent *FindElementAt(double x, double y);

private:
	int32_t m_Width;
	int32_t m_Height;

	uint8_t m_RowCount;
	uint8_t m_ColumnCount;
	int m_PartitionCount;

	float m_WidthScaleFactor;
	float m_HeightScaleFactor;

	std::vector<ScreenPartition> m_Partitions;
};
