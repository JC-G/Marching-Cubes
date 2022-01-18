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

    src/Engine/Action.cpp
    src/Engine/Action.h
    src/Engine/Camera.cpp
    src/Engine/Camera.h
    src/Engine/Controller.cpp
    src/Engine/Controller.h
    src/Engine/Shader.cpp
    src/Engine/Shader.h
    src/Engine/Window.cpp
    src/Engine/Window.h
    
    src/Engine/Drawing/Drawing.cpp
    src/Engine/Drawing/Drawing.h
    src/Engine/Drawing/Preview.cpp
    src/Engine/Drawing/Preview.h
    src/Engine/Drawing/Text.cpp
    src/Engine/Drawing/Text.h

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

    src/Editing/Brushes/BezierBrush.cpp
    src/Editing/Brushes/BezierBrush.h
    src/Editing/Brushes/Brush.cpp
    src/Editing/Brushes/Brush.h
    src/Editing/Brushes/BrushBoundingBox.cpp
    src/Editing/Brushes/BrushBoundingBox.h
    src/Editing/Brushes/CubicBezierBrush.cpp
    src/Editing/Brushes/CubicBezierBrush.h
    src/Editing/Brushes/CylinderBrush.cpp
    src/Editing/Brushes/CylinderBrush.h
    src/Editing/Brushes/EllipsoidBrush.cpp
    src/Editing/Brushes/EllipsoidBrush.h
    src/Editing/Brushes/RoadBrush.cpp
    src/Editing/Brushes/RoadBrush.h

    src/Editing/Editing.cpp
    src/Editing/Editing.h
    src/Editing/EditingHeaders.h
    src/Editing/TestBrushes.cpp
    src/Editing/TestBrushes.h

    src/Editing/Actions/BezierAction.cpp
    src/Editing/Actions/BezierAction.h
    src/Editing/Actions/CubicBezierAction.cpp
    src/Editing/Actions/CubicBezierAction.h
    src/Editing/Actions/CylinderAction.cpp
    src/Editing/Actions/CylinderAction.h
    src/Editing/Actions/SphereAction.cpp
    src/Editing/Actions/SphereAction.h
    src/Editing/Actions/DigSphereAction.cpp
    src/Editing/Actions/DigSphereAction.h
    src/Editing/Actions/InterpolationSplineAction.cpp
    src/Editing/Actions/InterpolationSplineAction.h
    src/Editing/Actions/RoadAction.cpp
    src/Editing/Actions/RoadAction.h
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
include_directories("src/Engine/Drawing")
include_directories("src/Geometry")
include_directories("src/SDF")
include_directories("src/Editing")
include_directories("src/Editing/Actions")
include_directories("src/Editing/Brushes")

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

# freetype
target_link_libraries(MarchingCubes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/freetype/lib/libfreetype.a)
include_directories("freetype/include/freetype2")

#tinyobjloader
add_subdirectory(lib/tinyobjloader EXCLUDE_FROM_ALL)
target_link_libraries(MarchingCubes PRIVATE tinyobjloader)


target_link_options(MarchingCubes PRIVATE -static-libgcc -static-libstdc++)

# include shader files and config in output directory

add_custom_command(TARGET MarchingCubes POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:MarchingCubes>/Shaders)
add_custom_command(TARGET MarchingCubes POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.json $<TARGET_FILE_DIR:MarchingCubes>/config.json)

