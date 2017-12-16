# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(MOL_SPLIT_SRC
        main.cpp
        MolSplit.cpp
        MolSplitOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-split-mol ${MOL_SPLIT_SRC})
QT5_USE_MODULES(svs-split-mol Core)
ADD_DEPENDENCIES(svs-split-mol cats_shared)

TARGET_LINK_LIBRARIES(svs-split-mol ${CATS_LIBS})

INSTALL(TARGETS
            svs-split-mol
        DESTINATION
            bin
        )
