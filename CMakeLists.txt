cmake_minimum_required(VERSION 2.6)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(PROJECT GeoTiffLoader)

include(cmake/cmake_utils.cmake)

include(src/SourcesList.cmake)

project(${PROJECT})

find_package(Qt4 REQUIRED)
include (${QT_USE_FILE})

qt4_wrap_cpp (MOC_SOURCES ${MOC_HEADERS})
qt4_add_resources (QRC_SOURCES ${RESOURCES})

add_executable(${PROJECT}
    ${Sources_List_Full_Path}
    ${MOC_Full_Path}
    ${FORMS}
    ${RESOURCES}
)

target_link_libraries(${PROJECT} ${QT_LIBRARIES})