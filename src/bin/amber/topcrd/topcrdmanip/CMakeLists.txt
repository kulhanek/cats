# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRDMANIP_SRC
        main.cpp
        TopCrdManip.cpp
        TopCrdManipOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrdmanip ${TOPCRDMANIP_SRC})
QT5_USE_MODULES(topcrdmanip Core)
TARGET_LINK_LIBRARIES(topcrdmanip
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topcrdmanip
        DESTINATION
            bin
        )

