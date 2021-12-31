cmake_minimum_required(VERSION 3.1)
project(MarchingCubes VERSION 0.1.0)

# include(CTest)
# enable_testing()

add_executable(MarchingCubes
    main.cpp

    src/Config.h
    src/Config.cpp
    src/GL_headers.h
    src/Octree.cpp
    src/Octree.h

    src/Engine/Camera.cpp
    src/Engine/Camera.h
    src/Engine/Controller.cpp
    src/Engine/Controller.h
    src/Engine/Drawing.cpp
    src/Engine/Drawing.h
    src/Engine/Shader.cpp
    src/Engine/Shader.h
    src/Engine/Window.cpp
    src/Engine/Window.h

    src/Geometry/CPUMarchingCubesGenerator.cpp
    src/Geometry/CPUMarchingCubesGenerator.h
    src/Geometry/CubeGeometryGenerator.cpp
    src/Geometry/CubeGeometryGenerator.h
    src/Geometry/Geometry.h
    src/Geometry/GeometryGenerator.cpp
    src/Geometry/GeometryGenerator.h
    src/Geometry/GPUMarchingCubesGenerator.cpp
    src/Geometry/GPUMarchingCubesGenerator.h
    src/Geometry/MarchingChunk.cpp
    src/Geometry/MarchingChunk.h
    src/Geometry/TransvoxelGenerator.cpp
    src/Geometry/TransvoxelGenerator.h

    src/SDF/NoiseTerrain.cpp
    src/SDF/NoiseTerrain.h
    src/SDF/planesdf.cpp
    src/SDF/planesdf.h
    src/SDF/SDF.cpp
    src/SDF/SDF.h
    src/SDF/SinTerrain.cpp
    src/SDF/SinTerrain.h
    src/SDF/Sphere.cpp
    src/SDF/Sphere.h
    src/SDF/TestSDF.cpp
    src/SDF/TestSDF.h

    src/Editing/Brush.cpp
    src/Editing/Brush.h
    src/Editing/BrushBoundingBox.cpp
    src/Editing/BrushBoundingBox.h
    src/Editing/CylinderBrush.cpp
    src/Editing/CylinderBrush.h
    src/Editing/Editing.cpp
    src/Editing/Editing.h
    src/Editing/EllipsoidBrush.cpp
    src/Editing/EllipsoidBrush.h
    src/Editing/TestBrushes.cpp
    src/Editing/TestBrushes.h

    # These are not compiled - but adding them here means the project is rebuild (and hence the modifications are reflected in the output directory)
    config.json

    Shaders/frag.glsl
    Shaders/vert.glsl
    Shaders/line_frag.glsl
    Shaders/line_vert.glsl
    Shaders/screen_frag.glsl
    Shaders/screen_vert.glsl

    Shaders/Compute/common.glsl
    Shaders/Compute/countpolygons.glsl
    Shaders/Compute/generatedensity.glsl
    Shaders/Compute/polygonize.glsl

    Shaders/Compute/shadertop.glsl

    Shaders/Compute/transvoxel_common.glsl
    Shaders/Compute/transvoxel_count.glsl
    Shaders/Compute/transvoxel_generate.glsl
    Shaders/Compute/transvoxel_polygonize.glsl

    Shaders/Compute/terrain_modification.glsl
    Shaders/Compute/brush_functions.glsl

)


include_directories("src")
include_directories("src/Engine")
include_directories("src/Geometry")
include_directories("src/SDF")
include_directories("src/Editing")

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# https://github.com/ArthurSonzogni/OpenGL_CMake_Skeleton/blob/master/CMakeLists.txt


set_property(TARGET MarchingCubes PROPERTY CXX_STANDARD 11)
target_compile_options(MarchingCubes PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(MarchingCubes PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(MarchingCubes PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(MarchingCubes PRIVATE glm)

# gli
add_subdirectory(lib/gli EXCLUDE_FROM_ALL)
target_link_libraries(MarchingCubes PRIVATE gli)

#message(STATUS "csd: ${CMAKE_CURRENT_SOURCE_DIR}/lib")
# freetype
#target_include_directories(MarchingCubes PRIVATE ${FREETYPE_INCLUDE_DIRS})

# target_link_libraries(MarchingCubes PRIVATE freetype)
target_link_libraries(MarchingCubes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/freetype/lib/libfreetype.a)
include_directories("freetype/include/freetype2")


target_link_options(MarchingCubes PRIVATE -static-libgcc -static-libstdc++)

# include shader files and config in output directory

add_custom_command(TARGET MarchingCubes POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:MarchingCubes>/Shaders)
add_custom_command(TARGET MarchingCubes POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.json $<TARGET_FILE_DIR:MarchingCubes>/config.json)

