# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Mandel)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()


# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP


if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.3.7/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Mandelbrot 1
add_executable(Mandelbrot
	common/keyinput.cpp
	common/keyinput.hpp
	common/shader.cpp
	common/shader.hpp
	
	Mandelbrot/Mandelbrot.cpp
	Mandelbrot/Bulb.shader
	Mandelbrot/vertex.shader
	Mandelbrot/fragment.shader
	Mandelbrot/vertex1.shader
	Mandelbrot/fragment1.shader
)
target_link_libraries(Mandelbrot
	${ALL_LIBS}
	ANTTWEAKBAR_116_OGLCORE_GLFW
)
# Xcode and Visual working directories
set_target_properties(Mandelbrot PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Mandelbrot/")
create_target_launcher(Mandelbrot WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Mandelbrot/")




# SolidMandel
add_executable(SolidMandel
	SolidMandel/SolidMandel.cpp
	common/keyinput.cpp
	common/keyinput.hpp
	common/shader.cpp
	common/shader.hpp

	SolidMandel/Bulb.vertexshader
	SolidMandel/View.vertexshader
	SolidMandel/View.fragmentshader
)
target_link_libraries(SolidMandel
	${ALL_LIBS}
	ANTTWEAKBAR_116_OGLCORE_GLFW
)

# Xcode and Visual working directories
set_target_properties(SolidMandel PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SolidMandel/")
create_target_launcher(SolidMandel WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/SolidMandel/")


# Mandelbulb
add_executable(Mandelbulb
	Mandelbulb/Mandelbulb.cpp
	common/keyinput.cpp
	common/keyinput.hpp
	common/shader.cpp
	common/shader.hpp
	
	Mandelbulb/vertex.shader
	Mandelbulb/fragment.shader
	Mandelbulb/vertex1.shader
	Mandelbulb/fragment1.shader
	Mandelbulb/Bspudd.shader
	Mandelbulb/Bulb.shader
	Mandelbulb/Bulbs.shader
	Mandelbulb/Bulb2.shader
	Mandelbulb/BulbDE.shader
	Mandelbulb/Bulb2DE.shader
	Mandelbulb/Box.shader
	Mandelbulb/BoxDE.shader
	Mandelbulb/Hybrid0.shader
	Mandelbulb/Hybrid1.shader
	Mandelbulb/Draw.shader
 	Mandelbulb/Filter.shader

)
target_link_libraries(Mandelbulb
	${ALL_LIBS}
	ANTTWEAKBAR_116_OGLCORE_GLFW
)
# Xcode and Visual working directories
set_target_properties(Mandelbulb PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Mandelbulb/")
create_target_launcher(Mandelbulb WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Mandelbulb/")


# Tracer
add_executable(Tracer
	Tracer/Tracer.cpp
	commontracer/keyinput.cpp
	commontracer/keyinput.hpp
	commontracer/shader.cpp
	commontracer/shader.hpp
	commontracer/texture.cpp
	commontracer/texture.hpp
	
	Tracer/Tracer.computeshader
	Tracer/Alttracer.computeshader
	Tracer/Filter0.computeshader
	Tracer/Filter1.computeshader
	Tracer/View.vertexshader
	Tracer/View.fragmentshader
	Tracer/View1.vertexshader
	Tracer/View1.fragmentshader
	Tracer/Spare0.computeshader
)
target_link_libraries(Tracer
	${ALL_LIBS}
	ANTTWEAKBAR_116_OGLCORE_GLFW
)

# Xcode and Visual working directories
set_target_properties(Tracer PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tracer/")
create_target_launcher(Tracer WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tracer/")




SOURCE_GROUP(commontracer REGULAR_EXPRESSION ".*/commontracer/.*" )
SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
   TARGET Mandelbulb POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Mandelbulb${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/Mandelbulb/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
