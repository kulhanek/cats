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
	CodeEditor.cpp
        resources.qrc
        )

# final build ------------------------------------------------------------------
QT5_WRAP_UI(CATS_IDE_SRC IDEWindow.ui)

ADD_EXECUTABLE(cats_ide_inter ${CATS_IDE_SRC})
ADD_DEPENDENCIES(cats_ide_inter cats_shared)

SET_TARGET_PROPERTIES(cats_ide_inter PROPERTIES
                        OUTPUT_NAME cats-ide
                        )

TARGET_LINK_LIBRARIES(cats_ide_inter
        readline
        Qt5::Core Qt5::Widgets Qt5::Script Qt5::ScriptTools Qt5::WebKit Qt5::WebKitWidgets
	${QT_LIBRARIES}
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            cats_ide_inter
        DESTINATION
            bin
        )

