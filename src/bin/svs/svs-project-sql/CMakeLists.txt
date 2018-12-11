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
QT5_USE_MODULES(svs-project-sql Core)
ADD_DEPENDENCIES(svs-project-sql cats_shared)

TARGET_LINK_LIBRARIES(svs-project-sql
        readline ncurses
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-project-sql
        DESTINATION
            bin
        )

