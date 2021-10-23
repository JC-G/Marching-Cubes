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
)


include_directories("src")
include_directories("src/Engine")
include_directories("src/Geometry")
include_directories("src/SDF")

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

# include shader files and config in output directory

add_custom_command(TARGET MarchingCubes POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Shaders $<TARGET_FILE_DIR:MarchingCubes>/Shaders)
add_custom_command(TARGET MarchingCubes POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.json $<TARGET_FILE_DIR:MarchingCubes>/config.json)

