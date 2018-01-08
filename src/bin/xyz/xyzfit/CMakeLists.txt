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
QT5_USE_MODULES(xyzfit Core)
ADD_DEPENDENCIES(xyzfit cats_shared)

TARGET_LINK_LIBRARIES(xyzfit
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            xyzfit
        DESTINATION
            bin
        )
