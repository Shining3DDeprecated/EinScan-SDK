cmake_minimum_required(VERSION 3.4)
project(sdk_demo_pro)

file(GLOB header *.h)
file(GLOB source *.cpp)
file(GLOB Resources *.qrc)

include_directories(${PROJECT_SOURCE_DIR}/../include)
link_directories(${PROJECT_SOURCE_DIR}/../x64_lib)

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}_bin")

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5OpenGL)
add_definitions(-DUNICODE)

qt5_wrap_ui(UI_FILES *.ui)
qt5_add_resources(ResourceSources ${Resources})
set(QT_USE_QTMAIN TRUE)

add_executable(${PROJECT_NAME} ${header} ${source} ${ResourceSources} ${UI_FILES})

if(MSVC)
set_target_properties( ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/MP" )
endif(MSVC)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  sn3d_pro_sdk
)