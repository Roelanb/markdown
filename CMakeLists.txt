cmake_minimum_required(VERSION 3.16)
project(MarkdownEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets)

# Enable automoc, autouic, autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/markdowneditor.cpp
    src/markdownhighlighter.cpp
    src/filebrowser.cpp
    src/previewwidget.cpp
    src/codehighlighter.cpp
    src/emojisupport.cpp
)

set(HEADERS
    src/mainwindow.h
    src/markdowneditor.h
    src/markdownhighlighter.h
    src/filebrowser.h
    src/previewwidget.h
    src/codehighlighter.h
    src/emojisupport.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
