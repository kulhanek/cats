# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOP2QMFIXHX_SRC
        main.cpp
        QMFixHX.cpp
        QMFixHXOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(top2qmfixhx ${TOP2QMFIXHX_SRC})
ADD_DEPENDENCIES(top2qmfixhx cats_shared)

TARGET_LINK_LIBRARIES(top2qmfixhx Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            top2qmfixhx
        DESTINATION
            bin
        )
