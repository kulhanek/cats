# ==============================================================================
#  VScreen CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_ALTER_SRC
        main.cpp
        AlterPrj.cpp
        AlterPrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(vs-project-alter ${VS_PROJECT_ALTER_SRC})

TARGET_LINK_LIBRARIES(vs-project-alter ${CHEMINFO_LIBRARIES})

INSTALL(TARGETS
            vs-project-alter
        DESTINATION
            bin
        )
