# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPMASK_SRC
        main.cpp
        TopMask.cpp
        TopMaskOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topmask ${TOPMASK_SRC})
ADD_DEPENDENCIES(topmask cats_shared)

QT5_USE_MODULES(topmask Core)
TARGET_LINK_LIBRARIES(topmask
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topmask
        DESTINATION
            bin
        )
