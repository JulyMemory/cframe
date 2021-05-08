# CMakeList.txt: cframe 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
#
MESSAGE(STATUS "Running cmake version ${CMAKE_VERSION}")

if (WIN32)
# CMake 3.8.0 is needed for Visual Studio 2017 and x64 toolset.
  cmake_minimum_required(VERSION 3.8)
elseif (APPLE)
elseif (UNIX)
endif()

cmake_minimum_required(VERSION 3.8)
project ("cframe")

# 添加头文件目录
include_directories(F:/Codebase/CPP/cframe/3rd/boost/boost_1_75_0)
# 添加需要链接的库文件目录
link_directories(F:/Codebase/CPP/cframe/3rd/boost/boost_1_75_0/stage/lib)

# 添加子目录到生成
# add_subdirectory(cbase)
# 从该文件运行或加载cmake代码
# include(cbase)

# 指定生成目标 
# add_executable(${PROJECT_NAME}.out asio/timer01.cpp)

# 指定可执行文件和lib文件目录
GET_FILENAME_COMPONENT(REALPATH_CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR} REALPATH)
GET_FILENAME_COMPONENT(REALPATH_CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR} REALPATH)

MESSAGE(STATUS "Source directory ${REALPATH_CMAKE_SOURCE_DIR}")
MESSAGE(STATUS "Binary directory ${REALPATH_CMAKE_BINARY_DIR}")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

# If necessary, use the RELATIVE flag, otherwise each source file may be listed 
# with full pathname. RELATIVE may makes it easier to extract an executable name
# automatically.
# file( GLOB APP_SOURCES RELATIVE app/*.cxx )
# get_filename_component(<var> <FileName> <mode> [CACHE])

file( GLOB APP_SOURCES RELATIVE_PATH *.cpp asio/*.cpp)
foreach( testsourcefile ${APP_SOURCES} )
    # I used a simple string replace, to cut off .cpp.
    # string( REPLACE ".cpp" "" testname ${testsourcefile} )
    get_filename_component(testname ${testsourcefile} NAME_WE)
    add_executable( ${testname}.out ${testsourcefile} )
    # Make sure YourLib is linked to each app
    # 添加链接库
    target_link_libraries(${testname}.out pthread)
    target_link_libraries(${testname}.out boost_chrono)
endforeach( testsourcefile ${APP_SOURCES} )

# TODO: 如有需要，请添加测试并安装目标。

