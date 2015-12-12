# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2CPMD_SRC
        main.cpp
        TopCrd2Cpmd.cpp
        TopCrd2CpmdInput.cpp
        TopCrd2CpmdGromos.cpp
        TopCrd2CpmdOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrd2cpmd ${TOPCRD2CPMD_SRC})
ADD_DEPENDENCIES(topcrd2cpmd cats_shared)

QT5_USE_MODULES(topcrd2cpmd Core)
TARGET_LINK_LIBRARIES(topcrd2cpmd
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topcrd2cpmd
        DESTINATION
            bin
        )
