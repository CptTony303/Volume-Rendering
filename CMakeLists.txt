cmake_minimum_required(VERSION 3.0)
project(VolumeRenderer)

include_directories(include)

file(GLOB_RECURSE INCLUDE "include/*.h")

foreach(HEADER_FILE ${INCLUDE})
    # Erstelle einen relativen Pfad zum Header-Dateinamen
    file(RELATIVE_PATH HEADER_REL_PATH ${CMAKE_SOURCE_DIR} ${HEADER_FILE})

    # Erstelle ein Source Group basierend auf dem relativen Pfad
    get_filename_component(HEADER_GROUP "${HEADER_REL_PATH}" DIRECTORY)
    string(REPLACE "/" "\\" HEADER_GROUP "${HEADER_GROUP}")

    # Füge die Header-Datei zur entsprechenden Source Group hinzu
    source_group("${HEADER_GROUP}" FILES ${HEADER_FILE})
endforeach()


# Füge die Quellcode-Dateien hinzu
file(GLOB_RECURSE SRCS "src/*.cpp")

foreach(FILE ${SRCS})
    # Erstelle einen relativen Pfad zum Dateinamen
    file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${FILE})

    # Erstelle ein Source Group basierend auf dem relativen Pfad
    get_filename_component(GROUP "${REL_PATH}" DIRECTORY)
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Füge die Datei zur entsprechenden Source Group hinzu
    source_group("${GROUP}" FILES ${FILE})
endforeach()

set(IMGUI_SOURCES
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(VolumeRenderer ${SRCS} ${INCLUDE} ${IMGUI_SOURCES} lib/glad/src/glad.c lib/stb_image/stb_image.cpp)

# Include the IMGUI headers
target_include_directories(VolumeRenderer PRIVATE lib/imgui)
target_include_directories(VolumeRenderer PRIVATE lib/imgui/backends)
# Add any required dependencies here
target_link_libraries(VolumeRenderer PRIVATE ${CONAN_LIBS})  # Example for using Conan package manager

# Verknüpfe die Anwendung mit der GLFW-Bibliothek
add_subdirectory(lib/glfw)
target_link_libraries(VolumeRenderer PRIVATE glfw)


# Installiere GLM
add_subdirectory(lib/glm)
target_link_libraries(VolumeRenderer PRIVATE glm)

# Füge OpenGL als Abhängigkeit hinzu
find_package(OpenGL REQUIRED)
target_link_libraries(VolumeRenderer PRIVATE OpenGL::GL)

#stb_image
include_directories(lib/stb_image/include)

#load shader
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})



