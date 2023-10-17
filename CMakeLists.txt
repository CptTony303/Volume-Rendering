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

add_executable(VolumeRenderer ${SRCS} ${INCLUDE})

# Verknüpfe die Anwendung mit der GLFW-Bibliothek
add_subdirectory(lib/glfw)
target_link_libraries(VolumeRenderer PRIVATE glfw)

# Füge OpenGL als Abhängigkeit hinzu
find_package(OpenGL REQUIRED)
target_link_libraries(VolumeRenderer PRIVATE OpenGL::GL)