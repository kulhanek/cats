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
ADD_DEPENDENCIES(svs-project-add-structures cats_shared)

TARGET_LINK_LIBRARIES(svs-project-add-structures Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-add-structures
        DESTINATION
            bin
        )
