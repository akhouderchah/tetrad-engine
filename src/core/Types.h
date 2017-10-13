#pragma once

#include <cstdint>

/** Base types **/

/** Engine types **/
typedef float deltaTime_t;

/**
 * @brief Defines a region bounded by a rectangle
 *
 * @note The actual interpretation of these values depend on the user. In the
 * UI system, the origin is in the top-left corner of the screen, and
 * (startX, startY) defines the top-left point of the bound.
 */
template <typename T, typename S>
union RectangularBound
{
	struct
	{
		T X;
		T Y;
	} points[2];
	S value;

	RectangularBound(T sX, T sY, T eX, T eY)
    { points[0] = {sX, sY}; points[1] = {eX, eY}; }
};

typedef RectangularBound<float, float> screenBound_t;

/**
 * @brief Defines the handle used for the component system.
 *
 * Current format is laid out as such. Entities have a type 0,
 * and components have a non-zero type. The ID 0 is reserved for
 * error codes (think of it as nullptr).
 * +---------------------+-----------+------------+
 * |        ID:32        |  type:16  | version:16 |
 * +---------------------+-----------+------------+
 */
class ObjHandle
{
public:
	typedef uint64_t handle_t;
	typedef uint32_t ID_t;
	typedef uint16_t type_t;
	typedef uint16_t version_t;

	ObjHandle();

	static handle_t  constructRawHandle(ID_t ID, type_t type, version_t version);
	static ObjHandle constructHandle(ID_t ID, type_t type, version_t version);

	ID_t      GetID()      const;
	type_t    GetType()    const;
	version_t GetVersion() const;

public:
	static const ObjHandle null;
	static const ID_t MAX_ID;

private:
	ObjHandle(handle_t handleVal);
	handle_t m_Handle;
};
