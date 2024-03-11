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
ADD_DEPENDENCIES(topcrd2crd cats_shared)

TARGET_LINK_LIBRARIES(topcrd2crd Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2crd
        DESTINATION
            bin
        )
