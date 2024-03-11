# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_PROJECT_SQL_SRC
        main.cpp
        VSProjectSQL.cpp
        VSProjectSQLOptions.cpp
        shell.c
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-project-sql ${VS_PROJECT_SQL_SRC})
ADD_DEPENDENCIES(svs-project-sql cats_shared)

TARGET_LINK_LIBRARIES(svs-project-sql Qt5::Core
        readline ncurses
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-sql
        DESTINATION
            bin
        )

