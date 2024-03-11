# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_CREATE_SRC
        main.cpp
        CreatePrj.cpp
        CreatePrjOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-project-create ${VS_PROJECT_CREATE_SRC})
ADD_DEPENDENCIES(svs-project-create cats_shared)

TARGET_LINK_LIBRARIES(svs-project-create Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-create
        DESTINATION
            bin
        )
