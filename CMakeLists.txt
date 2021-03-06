cmake_minimum_required(VERSION 3.17)
project(RideMiner)

include_directories(./src)
include_directories(./vendor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Gui Widgets Sql Charts)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql Qt5::Charts)

file(GLOB VENDOR_LIBS CONFIGURE_DEPENDS "vendor/*.h" "vendor/*.cpp")
file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/tasks/*.h" "src/tasks/*.cpp" "src/components/*.h" "src/components/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES} ${VENDOR_LIBS})


if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
