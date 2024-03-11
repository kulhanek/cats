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

TARGET_LINK_LIBRARIES(top2delphi Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            top2delphi
        DESTINATION
            bin
        )
