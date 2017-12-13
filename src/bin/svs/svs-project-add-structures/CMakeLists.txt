# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_ADD_SRC
        main.cpp
        AddStrPrj.cpp
        AddStrPrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-project-add-structures ${VS_PROJECT_ADD_SRC})
QT5_USE_MODULES(svs-project-add-structures Core)

TARGET_LINK_LIBRARIES(svs-project-add-structures
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-add-structures
        DESTINATION
            bin
        )
