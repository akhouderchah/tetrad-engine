#include "core/ObjectHandle.h"

namespace tetrad
{
const ObjectHandle ObjectHandle::null;
const ObjectHandle::ID_t ObjectHandle::MAX_ID = UINT32_MAX;

ObjectHandle::ObjectHandle() : m_Handle(0) {}

ObjectHandle::ObjectHandle(handle_t handleVal) : m_Handle(handleVal) {}

ObjectHandle::handle_t ObjectHandle::constructRawHandle(ID_t ID, type_t type,
                                                        version_t version)
{
  handle_t handle = 0;
  handle |= (handle_t)version;
  handle |= ((handle_t)type) << 16;
  handle |= ((handle_t)ID) << 32;

  return handle;
}

ObjectHandle ObjectHandle::constructHandle(ID_t ID, type_t type, version_t version)
{
  return ObjectHandle(constructRawHandle(ID, type, version));
}

ObjectHandle::ID_t ObjectHandle::GetID() const
{
  handle_t ID = m_Handle & 0xFFFFFFFF00000000;
  return (ID_t)(ID >> 32);
}

ObjectHandle::type_t ObjectHandle::GetType() const
{
  handle_t type = m_Handle & 0x00000000FFFF0000;
  return (type_t)(type >> 16);
}

ObjectHandle::version_t ObjectHandle::GetVersion() const
{
  handle_t version = m_Handle & 0x000000000000FFFF;
  return (version_t)version;
}

}  // namespace tetrad
