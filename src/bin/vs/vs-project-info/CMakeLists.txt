# ==============================================================================
#  VScreen CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_INFO_SRC
        main.cpp
        InfoPrj.cpp
        InfoPrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(vs-project-info ${VS_PROJECT_INFO_SRC})

TARGET_LINK_LIBRARIES(vs-project-info ${CHEMINFO_LIBRARIES})

INSTALL(TARGETS
            vs-project-info
        DESTINATION
            bin
        )
