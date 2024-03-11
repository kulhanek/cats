# ==============================================================================
#  VScreen CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(MOL_RMSD_SRC
        main.cpp
        MolRmsd.cpp
        MolRmsdOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(xyzfit ${MOL_RMSD_SRC})
ADD_DEPENDENCIES(xyzfit cats_shared)

TARGET_LINK_LIBRARIES(xyzfit Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            xyzfit
        DESTINATION
            bin
        )
