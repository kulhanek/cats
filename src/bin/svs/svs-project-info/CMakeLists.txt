# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_INFO_SRC
        main.cpp
        InfoPrj.cpp
        InfoPrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-project-info ${VS_PROJECT_INFO_SRC})
QT5_USE_MODULES(svs-project-info Core)

TARGET_LINK_LIBRARIES(svs-project-info
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-info
        DESTINATION
            bin
        )
