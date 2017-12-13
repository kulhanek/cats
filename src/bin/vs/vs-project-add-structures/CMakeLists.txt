# ==============================================================================
#  VScreen CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_ADD_SRC
        main.cpp
        AddStrPrj.cpp
        AddStrPrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(vs-project-add-structures ${VS_PROJECT_ADD_SRC})

TARGET_LINK_LIBRARIES(vs-project-add-structures ${CHEMINFO_LIBRARIES})

INSTALL(TARGETS
            vs-project-add-structures
        DESTINATION
            bin
        )
