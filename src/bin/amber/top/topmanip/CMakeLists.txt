# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPMANIP_SRC
        main.cpp
        TopManip.cpp
        TopManipOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topmanip ${TOPMANIP_SRC})
ADD_DEPENDENCIES(topmanip cats_shared)

QT5_USE_MODULES(topmanip Core)
TARGET_LINK_LIBRARIES(topmanip
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topmanip
        DESTINATION
            bin
        )
