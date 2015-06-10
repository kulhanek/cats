# ==============================================================================
# Description: try to find fftw3 library
#
# Input: (optional)
#    FFTW3_INCLUDE_PATH
#    FFTW3_LIBRARY_PATH
#
# Output:
#    FFTW3_FOUND
#    FFTW3_INCLUDE_DIR
#    FFTW3_LIBRARY_NAME
#    HAVE_FFTW3
#
#############################################################################

FIND_PATH(FFTW3_INCLUDE_DIR fftw3.h
    /usr/include
    $ENV{FFTW3_INCLUDE_PATH}
    )

IF(NOT DEFINED FFTW3_FOUND)
    IF(FFTW3_INCLUDE_DIR)
	MESSAGE(STATUS "Looking for FFTW3 header file fftw3.h ... found.")
    ELSE(FFTW3_INCLUDE_DIR)
	MESSAGE(STATUS "Looking for FFTW3 header file fftw3.h ... not found.")
    ENDIF(FFTW3_INCLUDE_DIR)
ENDIF(NOT DEFINED FFTW3_FOUND)

FIND_LIBRARY(FFTW3_LIBRARY_NAME
    NAMES fftw3
    PATHS
    /lib64
    /lib
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    $ENV{FFTW3_LIBRARY_PATH}
    )

IF(NOT DEFINED FFTW3_FOUND)
    IF(FFTW3_LIBRARY_NAME)
	MESSAGE(STATUS "Looking for FFTW3 library file fftw3 ... found.")
    ELSE(FFTW3_LIBRARY_NAME)
	MESSAGE(STATUS "Looking for FFTW3 library file fftw3 ... not found.")
	SET(FFTW3_LIBRARY_NAME "")
    ENDIF(FFTW3_LIBRARY_NAME)
ENDIF(NOT DEFINED FFTW3_FOUND)

# ------------------------------------------------------------------------------

IF(FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY_NAME)
    SET(FFTW3_FOUND TRUE CACHE INTERNAL "FFTW3 library" FORCE)
    SET(HAVE_FFTW3 1)
    ADD_DEFINITIONS(-DHAVE_FFTW3)
ELSE(FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY_NAME)
    SET(FFTW3_FOUND FALSE CACHE INTERNAL "FFTW3 library" FORCE)
ENDIF(FFTW3_INCLUDE_DIR AND FFTW3_LIBRARY_NAME)

