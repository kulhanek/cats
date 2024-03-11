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
ADD_DEPENDENCIES(svs-project-info cats_shared)

TARGET_LINK_LIBRARIES(svs-project-info Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-info
        DESTINATION
            bin
        )
