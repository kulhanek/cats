# ==============================================================================
# OBSymChg CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(PROG_SRC main.cpp options.cpp)

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(obsymchg ${PROG_SRC})
ADD_DEPENDENCIES(obsymchg cats_shared)

TARGET_LINK_LIBRARIES(obsymchg Qt5::Core
        ${OPEN_BABEL_LIB}
        ${CATS_LIBS}
        ${SCIMAFIC_CLIB_NAME}
        ${HIPOLY_LIB_NAME}
        )

INSTALL(TARGETS
            obsymchg
        RUNTIME DESTINATION
            bin
        )

