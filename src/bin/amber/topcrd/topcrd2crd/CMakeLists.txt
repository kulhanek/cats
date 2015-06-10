# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2CRD_SRC
        main.cpp
        TopCrd2Crd.cpp
        TopCrd2CrdOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrd2crd ${TOPCRD2CRD_SRC})
QT5_USE_MODULES(topcrd2crd Core)
TARGET_LINK_LIBRARIES(topcrd2crd
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            topcrd2crd
        DESTINATION
            bin
        )
