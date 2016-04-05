# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(CATS_IDE_SRC
        main.cpp
	IDEWindow.cpp
	SyntaxHighlighter.cpp
	JSEngineThread.cpp
	StdoutWatcher.cpp
        resources.qrc
        )

# final build ------------------------------------------------------------------
QT5_WRAP_UI(CATS_IDE_SRC IDEWindow.ui)

ADD_EXECUTABLE(cats_ide_inter ${CATS_IDE_SRC})
ADD_DEPENDENCIES(cats_ide_inter cats_shared)

QT5_USE_MODULES(cats_ide_inter Core Widgets Script ScriptTools)
SET_TARGET_PROPERTIES(cats_ide_inter PROPERTIES
                        OUTPUT_NAME cats-ide
                        )

TARGET_LINK_LIBRARIES(cats_ide_inter
        readline
	${QT_LIBRARIES}
        ${CATS_LIBS}
	${SYSTEM_LIBS}
        )

INSTALL(TARGETS
            cats_ide_inter
        DESTINATION
            bin
        )

