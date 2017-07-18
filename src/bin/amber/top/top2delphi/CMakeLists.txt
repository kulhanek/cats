# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOP2DELPHI_SRC
        main.cpp
        Top2Delphi.cpp
        Top2DelphiOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(top2delphi ${TOP2DELPHI_SRC})
ADD_DEPENDENCIES(top2delphi cats_shared)

QT5_USE_MODULES(top2delphi Core)
TARGET_LINK_LIBRARIES(top2delphi
        ${CATS_LIBS})

INSTALL(TARGETS
            top2delphi
        DESTINATION
            bin
        )
