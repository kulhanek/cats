# ==============================================================================
# CATs CMake File
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
PROJECT(CATs)

# ==============================================================================
# conditional setup ------------------------------------------------------------
# ==============================================================================

SET(LIBS_SHARED     ON  CACHE BOOL "Should the shared version of cats library be built?")
SET(LIBS_STATIC     OFF CACHE BOOL "Should the static version of cats library be built?")
SET(TRY_QT_LIB      ON  CACHE BOOL "Should the qt lib be used?")
SET(CATS_NETWORK    ON  CACHE BOOL "Is network available")

# ==============================================================================
# project setup ----------------------------------------------------------------
# ==============================================================================

# define project name ----------------------------
SET(LibBuildVersion_CATs 2.0.x)           # CATs version

# what languages we use --------------------------
ENABLE_LANGUAGE(CXX)

# ==============================================================================
# external dependencies --------------------------------------------------------
# ==============================================================================

IF(NOT DEFINED COMPOSITE_PROJECT)
    SET(DEVELOPMENT_ROOT "$ENV{DEVELOPMENT_ROOT}")
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

IF(NOT DEVELOPMENT_ROOT)
    MESSAGE(FATAL_ERROR "DEVELOPMENT_ROOT variable is not set!")
ENDIF(NOT DEVELOPMENT_ROOT)

# ==============================================================================
# external libraries -----------------------------------------------------------
# ==============================================================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${DEVELOPMENT_ROOT}/projects/cmake-modules/1.0/share/modules")

# boost =======================
FIND_PACKAGE(MyBoost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

FIND_PACKAGE(ReadLine REQUIRED)
INCLUDE_DIRECTORIES(${ReadLine_INCLUDE_DIRS})
LINK_DIRECTORIES(${ReadLine_LIB_DIRS})

# optional
FIND_PACKAGE(FFTW3)
IF(HAVE_FFTW3)
    INCLUDE_DIRECTORIES(${FFTW3_INCLUDE_DIRS})
    LINK_DIRECTORIES(${FFTW3_LIB_DIRS})
ENDIF(HAVE_FFTW3)

# OBCore ====================
SET(OBCORE_ROOT ${DEVELOPMENT_ROOT}/thirdparty/obcore/2.4.0)
INCLUDE_DIRECTORIES(${OBCORE_ROOT}/include SYSTEM)
LINK_DIRECTORIES(${OBCORE_ROOT}/lib)
SET(OBCORE_LIB_NAME obcore)

# NetCDFCore ==================
SET(NETCDF_ROOT ${DEVELOPMENT_ROOT}/thirdparty/netcdfcore/4.0.1)
INCLUDE_DIRECTORIES(${NETCDF_ROOT}/include)
LINK_DIRECTORIES(${NETCDF_ROOT}/lib)
SET(NETCDF_CLIB_NAME cnetcdf)

# HIPOLY ====================
SET(HIPOLY_ROOT ${DEVELOPMENT_ROOT}/projects/hipoly/1.0)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/compress)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/error)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/event)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/file)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/list)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/option)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/string)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/stream)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/thread)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/time)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/xml)
INCLUDE_DIRECTORIES(${HIPOLY_ROOT}/src/lib/hipoly/color)
LINK_DIRECTORIES(${HIPOLY_ROOT}/lib)
SET(HIPOLY_LIB_NAME hipoly)

# SCIMAFIC ====================
SET(SCIMAFIC_ROOT ${DEVELOPMENT_ROOT}/projects/scimafic/1.0)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/maths)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/files)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/chemistry)
INCLUDE_DIRECTORIES(${SCIMAFIC_ROOT}/src/lib/cscimafic/physics)
LINK_DIRECTORIES(${SCIMAFIC_ROOT}/lib)
SET(SCIMAFIC_CLIB_NAME cscimafic)

# PRMFile ===================
SET(PRMFILE_ROOT ${DEVELOPMENT_ROOT}/projects/prmfile/1.0)
INCLUDE_DIRECTORIES(${PRMFILE_ROOT}/src/lib/cprmfile SYSTEM)
LINK_DIRECTORIES(${PRMFILE_ROOT}/lib)
SET(PRMFILE_CLIB_NAME cprmfile)

# NETLib ====================
SET(NETLIB_ROOT ${DEVELOPMENT_ROOT}/projects/netlib/1.0)
INCLUDE_DIRECTORIES(${NETLIB_ROOT}/src/lib/netlib)
INCLUDE_DIRECTORIES(${NETLIB_ROOT}/src/lib/netlib/common)
INCLUDE_DIRECTORIES(${NETLIB_ROOT}/src/lib/netlib/client)
INCLUDE_DIRECTORIES(${NETLIB_ROOT}/src/lib/netlib/server)
INCLUDE_DIRECTORIES(${NETLIB_ROOT}/src/lib/netlib/extra/client)
INCLUDE_DIRECTORIES(${NETLIB_ROOT}/src/lib/netlib/extra/server)
LINK_DIRECTORIES(${NETLIB_ROOT}/lib)
SET(NETLIB_LIB_NAME netlib)

# ASL =========================
SET(ASL_ROOT ${DEVELOPMENT_ROOT}/projects/asl/2.0)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/topology)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/restart)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/trajectory)
INCLUDE_DIRECTORIES(${ASL_ROOT}/src/lib/asl/mask)
LINK_DIRECTORIES(${ASL_ROOT}/lib)
SET(ASL_LIB_NAME asl)

# PMFLib =========================
SET(PMFLIB_ROOT ${DEVELOPMENT_ROOT}/projects/pmflib/6.0)
INCLUDE_DIRECTORIES(${PMFLIB_ROOT}/src/lib/cpmf)
INCLUDE_DIRECTORIES(${PMFLIB_ROOT}/src/lib/fdrivers/cats/)
LINK_DIRECTORIES(${PMFLIB_ROOT}/lib)
LINK_DIRECTORIES(${PMFLIB_ROOT}/lib/drivers)
SET(PMF_LIB_NAME fpmfdrv_cats fpmf cpmf)

# QT5 =========================
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Script REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
SET(HAVE_QT5 1)
ADD_DEFINITIONS(-DHAVE_QT5)

# setup for sqlite
ADD_DEFINITIONS(-DHAVE_READLINE)

# libraries
SET(SYSTEM_LIBS
    ${PMF_LIB_NAME}
    ${NETCDF_CLIB_NAME}
    ${NETLIB_LIB_NAME}
    ${OBCORE_LIB_NAME}
    ${QT_LIBRARIES}
    ${ASL_LIB_NAME}
    ${PRMFILE_CLIB_NAME}
    ${SCIMAFIC_CLIB_NAME}
    ${HIPOLY_LIB_NAME}
    )

SET(CATS_LIBS cats
    ${SYSTEM_LIBS}
    )

# ==============================================================================
# output directories -----------------------------------------------------------
# ==============================================================================

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)

# ==============================================================================
# rpaths -----------------------------------------------------------------------
# ==============================================================================

IF(NOT DEFINED COMPOSITE_PROJECT)
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    # the RPATH to be used when installing
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib/drivers")

    IF(QT_RPATH)
        # the RPATH to be used when installing
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:${QT_RPATH}")
    ELSE(QT_RPATH)

    ENDIF(QT_RPATH)

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

# ==============================================================================
# compiler setups --------------------------------------------------------------
# ==============================================================================

IF(NOT DEFINED COMPOSITE_PROJECT)
    # release setup
    SET(CMAKE_C_FLAGS_RELEASE "-O2")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")

    # debug setup
    SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wno-long-long")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -pedantic-errors -Wno-long-long")

    #SET(CMAKE_BUILD_TYPE Release)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT DEFINED COMPOSITE_PROJECT)

# ==============================================================================
# project subdirectories  ------------------------------------------------------
# ==============================================================================

ADD_SUBDIRECTORY(src)
