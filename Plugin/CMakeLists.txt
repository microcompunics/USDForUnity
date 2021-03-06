CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(usdi)

OPTION(USDI_DEBUG "Enable Debug" OFF)

SET(USD_ROOT /opt/pixar/usd)
SET(USD_VFX_ROOT /opt/pixar/usd-vfx2015)


IF(MSVC)
ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wno-deprecated")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,${USD_ROOT}/lib:${USD_VFX_ROOT}/lib")
ENDIF()

ADD_DEFINITIONS(-DBUILD_OPTLEVEL_OPT -DBUILD_COMPONENT_SRC_PREFIX="usdi")
IF(USDI_DEBUG)
    ADD_DEFINITIONS(-DusdiDebug)
    SET(CMAKE_BUILD_TYPE Debug)
ELSE()
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()


INCLUDE_DIRECTORIES(
    ${INCLUDES}
    .
    ${USD_ROOT}/include
    ${USD_VFX_ROOT}/include
    ${USD_VFX_ROOT}/include/python2.7
    ${USD_VFX_ROOT}/packages/openexr-2.2.0/include/OpenEXR
)

LINK_DIRECTORIES (
    ${USD_ROOT}/lib
    ${USD_VFX_ROOT}/lib
)


FILE(GLOB USDI_CXX_FILES usdi/*.cpp)
FILE(GLOB USDI_H_FILES usdi/*.h)
ADD_LIBRARY(usdi SHARED ${USDI_CXX_FILES} ${USDI_H_FILES})
TARGET_LINK_LIBRARIES(usdi usd usdGeom usdUtils boost_python-mt)


ADD_EXECUTABLE(test_import Test/usdiTestImport.cpp)
ADD_DEPENDENCIES(test_import usdi)
TARGET_LINK_LIBRARIES(test_import usdi)


ADD_EXECUTABLE(test_export Test/usdiTestExport.cpp)
ADD_DEPENDENCIES(test_export usdi)
TARGET_LINK_LIBRARIES(test_export usdi)
