# ==============================================================================
#  VScreen CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(MOL_SPLIT_SRC
        main.cpp
        MolSplit.cpp
        MolSplitOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(mol-split ${MOL_SPLIT_SRC})

TARGET_LINK_LIBRARIES(mol-split ${CHEMINFO_LIBRARIES})

INSTALL(TARGETS
            mol-split
        DESTINATION
            bin
        )
