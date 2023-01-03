
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(rasty)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
# cmake_policy(SET CMP0074 NEW)




SET(embree_DIR ""
    CACHE FILEPATH "Directory containing an embree-config.cmake file")
SET(ospray_DIR ""
    CACHE FILEPATH "Directory containing an osprayConfig.cmake file")
message(STATUS "embree_DIR: ${embree_DIR}")
message(STATUS "ospray_DIR: ${ospray_DIR}")
message(STATUS "OSPRAY_INSTALL_DIR: ${OSPRAY_INSTALL_DIR}")

# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

SET(OSPRAY_LIBRARIES ${OSPRAY_INSTALL_DIR}/lib/libospray.so)
SET(RKCOMMON_LIBRARY ${OSPRAY_INSTALL_DIR}/lib/libospray.so)

# FIND_PACKAGE(rkcommon "1.10.0" REQUIRED)
FIND_PACKAGE(ospray "2.10.0" REQUIRED)
FIND_PACKAGE(embree "3.13.3" REQUIRED)

OPTION(BUILD_EXAMPLES "Build example applications" ON)

SET(RASTY_LIBS  ${OSPRAY_LIBRARIES} ${EMBREE_LIBRARY}) # ${RKCOMMON_LIBRARY}
SET(RASTY_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include"
    ${OSPRAY_INCLUDE_DIR} ${EMBREE_INCLUDE_DIRS} ${RKCOMMON_INCLUDE_DIR})


# SET(BASE_OSP /home/hp/github/research/geofabric/geofabric/)
# SET(embree_DIR ${BASE_OSP}embree-3.13.3.x86_64.linux/lib/cmake/embree-3.13.3)
# SET(ospray_ROOT ${BASE_OSP}ospray-2.10.0.x86_64.linux/lib/cmake/ospray-2.10.0 )
# SET(ospray_DIR /opt/intel/oneapi/ospray/2.10.0/lib/cmake/ospray)
# SET(ospray_ROOT /opt/intel/oneapi/ospray/2.10.0/)
# SET(ospray_DIR ${BASE_OSP}ospray-2.10.0.x86_64.linux/lib/cmake/ospray-2.10.0)
# SET(OSPRAY_CMAKE_DIR ${BASE_OSP}ospray-2.10.0.x86_64.linux/lib/cmake/ospray-2.10.0 )


# include(cmake/FindOSPRay.cmake)


# FIND_PACKAGE(ospray "2.10.0" REQUIRED)
# FIND_PACKAGE(ospray "2.10.0" REQUIRED HINT ${ospray_DIR})
# FIND_PACKAGE(embree "3.13.3" REQUIRED)
# FIND_PACKAGE(rkcommon "1.10.0" REQUIRED)
# FIND_PACKAGE(embree "3.13.3" REQUIRED HINT ${embree_DIR})
# FIND_PACKAGE(CGAL)


# FIND_PACKAGE(embree 3.2 REQUIRED)
# FIND_PACKAGE(ospray REQUIRED)

# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# OPTION(USE_NETCDF "Enable NetCDF file reading" ON)
# OPTION(BUILD_EXAMPLES "Build example applications" ON)
# OPTION(BUILD_DOCUMENTATION "Build documentation with Sphinx" OFF)

# ${CGAL_LIBRARIES}
# SET(RASTY_LIBS  ${OSPRAY_LIBRARIES} ${EMBREE_LIBRARIES}) # ${RKCOMMON_LIBRARY}
# SET(RASTY_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include"
    # ${OSPRAY_INCLUDE_DIR} ${EMBREE_INCLUDE_DIRS} ${RKCOMMON_INCLUDE_DIR})

MESSAGE(STATUS "RASTY LIBS: ${RASTY_LIBS}")
MESSAGE(STATUS "OSPRAY LIBS: ${OSPRAY_LIBRARIES}")
MESSAGE(STATUS "EMBREE LIBS: ${EMBREE_LIBRARIES}")
# MESSAGE(STATUS "RKCOMMON_INCLUDE_DIR: ${RKCOMMON_INCLUDE_DIR}")


#USE_NETCDF adds netcdf includes/libs for data reading
# IF(USE_NETCDF)
    # give cmake some hints to finding NetCDF - god knows it needs it
SET(NETCDF_INCLUDE_DIR "/usr/include"
    CACHE FILEPATH "Path to NetCDF includes")
SET(NETCDF_LIBRARY "/usr/lib/x86_64-linux-gnu/libnetcdf_c++4.so"
    CACHE FILEPATH "Path to NetCDF library")
# set this else the findnetcdf won't look in system paths
SET(NETCDF_USE_DEFAULT_PATHS ON)

FIND_PACKAGE(NetCDF COMPONENTS CXX)

IF(NETCDF_FOUND)
    ADD_DEFINITIONS(-DRASTY_NETCDF)
    SET(RASTY_INCLUDE_DIRS ${RASTY_INCLUDE_DIRS}
        ${NETCDF_CXX_INCLUDE_DIRS})
    SET(RASTY_LIBS ${RASTY_LIBS} ${NETCDF_CXX_LIBRARIES})
ELSE()
    MESSAGE(FATAL_ERROR
        "Requested NetCDF but the library could not be found")
ENDIF()
# ENDIF(USE_NETCDF)

FILE(GLOB RASTY_SOURCES "src/*.cpp" "src/lodepng/*.cpp")

ADD_LIBRARY(rasty SHARED ${RASTY_SOURCES})
TARGET_INCLUDE_DIRECTORIES(rasty PUBLIC ${RASTY_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(rasty ${RASTY_LIBS} pthread jpeg gdal)

IF(BUILD_EXAMPLES)
    ADD_EXECUTABLE(simple_render ${RASTY_SOURCES}
        "examples/simple_render.cpp")
    TARGET_LINK_LIBRARIES(simple_render ${RASTY_LIBS} pthread jpeg gdal ospray::ospray)
    TARGET_INCLUDE_DIRECTORIES(simple_render PUBLIC ${RASTY_INCLUDE_DIRS})

    ADD_EXECUTABLE(rastyTest ${RASTY_SOURCES}
    "examples/rastyTest.cpp")
    TARGET_LINK_LIBRARIES(rastyTest ${RASTY_LIBS} pthread jpeg gdal)
    TARGET_INCLUDE_DIRECTORIES(rastyTest PUBLIC ${RASTY_INCLUDE_DIRS})
ENDIF(BUILD_EXAMPLES)

# IF(BUILD_DOCUMENTATION)
#     ADD_SUBDIRECTORY(docs)
# ENDIF(BUILD_DOCUMENTATION)

# install rules
INSTALL(TARGETS rasty LIBRARY DESTINATION lib)
INSTALL(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/" DESTINATION include
    FILES_MATCHING PATTERN "*.h")
#####################################################################################################################

# # Here and below: Cmake version and out-of-source build setup
# # minmimum version check
# cmake_minimum_required(VERSION 3.22)
# if(${CMAKE_VERSION} VERSION_LESS 3.12)
#     cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
# endif()

# # no in-source builds check
# if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
#     message( FATAL_ERROR "In-source builds not allowed! Create a build directory and run CMake from there. " )
# endif()

# # create project
# project(
# 	rasterize
# 	VERSION 1.0
# 	LANGUAGES CXX
# )

# OPTION(BUILD_EXAMPLES "Build example applications" ON)


# get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

# SET(CMAKE_CXX_STANDARD 11)
# SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# SET(BASE_OSP /home/hp/github/research/ospray/build/install/)

# SET(embree_DIR ${BASE_OSP}embree/lib/cmake/embree-3.13.3)
# SET(ospray_DIR ${BASE_OSP}ospray/lib/cmake/ospray-2.10.0)
# SET(rkcommon_DIR ${BASE_OSP}rkcommon/lib/cmake/rkcommon-1.10.0)
# SET(RKCOMMON_INCLUDE_DIR ${BASE_OSP}rkcommon/include)
# SET(OSPRAY_LIBRARIES ${BASE_OSP}ospray/lib/libospray.so.2.10.0)


# FIND_PACKAGE(ospray REQUIRED HINT ${ospray_DIR})
# FIND_PACKAGE(embree REQUIRED HINT ${embree_DIR})

# # get_cmake_property(_variableNames VARIABLES)
# # foreach (_variableName ${_variableNames})
# #     message(STATUS "${_variableName}=${${_variableName}}")
# # endforeach()


# SET(RASTERIZE_LIBS ${EMBREE_LIBRARIES} ${OSPRAY_LIBRARIES} ${RKCOMMON_LIBRARY})
# SET(RASTERIZE_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include"
#     ${OSPRAY_INCLUDE_DIR} ${EMBREE_INCLUDE_DIRS} ${RKCOMMON_INCLUDE_DIR} )


# MESSAGE(STATUS "RASTERIZE LIBS: ${RASTERIZE_LIBS}")
# MESSAGE(STATUS "OSPRAY LIBS: ${OSPRAY_LIBRARIES}")
# MESSAGE(STATUS "EMBREE LIBS: ${EMBREE_LIBRARIES}")
# FILE(GLOB RASTERIZE_SOURCES "src/*.cpp")


# ADD_LIBRARY(rasterize SHARED ${RASTERIZE_SOURCES})
# TARGET_INCLUDE_DIRECTORIES(rasterize PUBLIC ${RASTERIZE_INCLUDE_DIRS})
# TARGET_LINK_LIBRARIES(rasterize ${RASTERIZE_LIBS} pthread jpeg gdal)



# # source file setup

# add_subdirectory(include)
# add_subdirectory(src)

# if (BUILD_EXAMPLES)
# 	message(STATUS "Building examples...")

# 	ADD_EXECUTABLE(ospTutorial ${RASTERIZE_SOURCES}
# 	"example/ospTutorial.cpp")
# 	TARGET_LINK_LIBRARIES(ospTutorial ${RASTERIZE_LIBS} pthread jpeg gdal)
# 	TARGET_INCLUDE_DIRECTORIES(ospTutorial PUBLIC ${RASTERIZE_INCLUDE_DIRS})

# 	ADD_EXECUTABLE(readGeoTiff ${RASTERIZE_SOURCES}
# 	"example/readGeoTiff.cpp")
# 	TARGET_LINK_LIBRARIES(readGeoTiff ${RASTERIZE_LIBS} pthread jpeg gdal)
# 	TARGET_INCLUDE_DIRECTORIES(readGeoTiff PUBLIC ${RASTERIZE_INCLUDE_DIRS})
# endif()


# INSTALL(TARGETS rasterize LIBRARY DESTINATION lib)
# INSTALL(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/" DESTINATION include
#     FILES_MATCHING PATTERN "*.h")
