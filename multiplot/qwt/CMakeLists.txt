project(Qwt)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Svg Xml PrintSupport Concurrent)

set(CMAKE_SOURCE_DIR .)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#---

set(LibName qwt)
include(${CMAKE_CURRENT_SOURCE_DIR}/SourcesList.cmake)

add_definitions(-DQWT_MOC_INCLUDE)
add_library(${LibName} ${SOURCES} ${HEADERS})

target_link_libraries(${LibName} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Svg Qt5::Xml)
target_link_libraries(${LibName} PUBLIC Qt5::PrintSupport Qt5::Concurrent)
set(QWT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${LibName} PUBLIC ${QWT_INCLUDE_DIR})

configure_file(
    ${CMAKE_SOURCE_DIR}/CMake/QwtConfig.cmake.in
    "${CMAKE_BINARY_DIR}/QwtConfig.cmake" @ONLY)

export(TARGETS ${LibName}
    FILE "${CMAKE_BINARY_DIR}/QwtTargets.cmake")
