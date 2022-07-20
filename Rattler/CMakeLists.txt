cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(Rattler)

set(CMAKE_BUILD_TYPE  "Release")

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
	message(STATUS "found openmp") 
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}, ${OPENMP_C_FLAGS})
   	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}, ${OPENMP_CXX_FLAGS})
else()
   	message(FATAL_ERROR "openmp not found!")
endif()


# include files
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdLib
)

# Third Party Library files
set(3rdLib
	3rdLib/stb_image.h
	3rdLib/stb_image_write.h
	3rdLib/stb_image_resize.h
	3rdLib/OpenGL/glfw3.h
	3rdLib/OpenGL/glfw3native.h
	3rdLib/OpenGL/khrplatform.h
	3rdLib/OpenGL/glad.h
	3rdLib/OpenGL/glad.c
)
# Make the 3rdLib group
SOURCE_GROUP("3rdLib" FILES ${3rdLib})

# Core files
set(Core
	Core/Common.h
	Core/Geometry.h
	Core/Geometry.cpp
	Core/Transform.h
	Core/Transform.cpp
	Core/Spectrum.h
	Core/Spectrum.cpp
)
# Make the Core group
SOURCE_GROUP("Core" FILES ${Core})

# Main files
set(Main
	Main/Main.cpp
	Main/ModelLoad.h
	Main/ModelLoad.cpp
	Main/File.h
	Main/File.cpp
	Main/ImageProcee.h
	Main/ImageProcee.cpp
)
# Make the Main group
SOURCE_GROUP("Main" FILES ${Main})

# Create executable
add_executable(Rattler
	${3rdLib}
	${Main}
	${Core}
)

FILE(GLOB Icons ${CMAKE_CURRENT_SOURCE_DIR}/Icons/*.png)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${Icons} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Icons/)
# 用于包含第三方库
set(AssimpLib ${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/Assimp-lib/)
set(OpenGLLib ${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/OpenGL-lib/)

FILE(GLOB ASSIMPDLL ${AssimpLib}/assimp-vc140-mt.dll)
file(COPY ${ASSIMPDLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
FILE(GLOB OpenGLDLL ${OpenGLLib}/glfw3.dll)
file(COPY ${OpenGLDLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

target_link_libraries(Rattler 
	${AssimpLib}assimp.lib
	${OpenGLLib}glfw3.lib
	${OpenGLLib}glfw3_mt.lib
	${OpenGLLib}glfw3dll.lib
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Rattler)







