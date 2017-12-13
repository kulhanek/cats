# ==============================================================================
#  VScreen CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_CREATE_SRC
        main.cpp
        CreatePrj.cpp
        CreatePrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(vs-project-create ${VS_PROJECT_CREATE_SRC})

TARGET_LINK_LIBRARIES(vs-project-create ${CHEMINFO_LIBRARIES})

INSTALL(TARGETS
            vs-project-create
        DESTINATION
            bin
        )
