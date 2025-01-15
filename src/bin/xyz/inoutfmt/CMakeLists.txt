# ==============================================================================
# CATs - Conversion and Analysis Tools
# ==============================================================================

# program objects --------------------------------------------------------------
SET(INOUTFMT_SRC
        main.cpp
        InOutFmt.cpp
        InOutFmtOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(inoutfmt ${INOUTFMT_SRC})
ADD_DEPENDENCIES(inoutfmt cats_shared)

TARGET_LINK_LIBRARIES(inoutfmt Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            inoutfmt
        DESTINATION
            bin
        )
