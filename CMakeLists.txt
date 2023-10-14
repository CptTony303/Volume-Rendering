cmake_minimum_required(VERSION 3.0)
project(VolumeRenderer)

# Füge das `include`-Verzeichnis hinzu, damit die Header-Dateien gefunden werden
include_directories(include)

# Füge die Quellcode-Dateien hinzu
file(GLOB_RECURSE SRCS "src/*.cpp")
add_executable(VolumeRenderer ${SRCS})

# Verknüpfe die Anwendung mit der GLFW-Bibliothek
add_subdirectory(lib/glfw)
target_link_libraries(VolumeRenderer PRIVATE glfw)

# Füge OpenGL als Abhängigkeit hinzu
find_package(OpenGL REQUIRED)
target_link_libraries(VolumeRenderer PRIVATE OpenGL::GL)
