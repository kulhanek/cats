# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_ALTER_SRC
        main.cpp
        AlterPrj.cpp
        AlterPrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-project-alter ${VS_PROJECT_ALTER_SRC})
QT5_USE_MODULES(svs-project-alter Core)
ADD_DEPENDENCIES(svs-project-alter cats_shared)

TARGET_LINK_LIBRARIES(svs-project-alter
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-alter
        DESTINATION
            bin
        )
