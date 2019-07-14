# protobuf
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(protobuf/cmake)

# GLFW
if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions("/MP")
endif()

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
#set(BUILD_SHARED_LIBS ON)
add_subdirectory(glfw)

include_directories(
	glfw/include/GLFW/
	glew/include/
	)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(OPENGL_LIBRARIES
		${OPENGL_LIBRARY}
		-lGL -lGLU -lXrandr -lXext -lX11 -lrt
		${CMAKE_DL_LIBS}
		#glfw
		#${OPENGL_glu_LIBRARY}
		${GLFW_LIBRARIES}
		)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(OPENGL_LIBRARIES
		${OPENGL_LIBRARY}
		${CMAKE_DL_LIBS}
		${GLFW_LIBRARIES}
		)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# GLEW
set(GLEW_SOURCE
	glew/src/glew.c)

set(GLEW_HEADERS
	)

add_library(GLEW STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
	)

target_link_libraries(GLEW
	${OPENGL_LIBRARIES}
	${EXTRA_LIBS}
	)

# FreeType
add_subdirectory(freetype2)

# Assimp
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
## Currently we only use the obj importer. This will likely change in the
## future.
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)

include_directories(assimp/include)
include_directories(${CMAKE_BINARY_DIR}/external/assimp/code)
add_subdirectory(assimp)
target_include_directories(assimp PRIVATE
	assimp/include
	assimp/code/BoostWorkaround
	${CMAKE_SOURCE_DIR}/external/assimp/contrib/zlib
	${CMAKE_BINARY_DIR}/external/assimp/contrib/zlib
	)
