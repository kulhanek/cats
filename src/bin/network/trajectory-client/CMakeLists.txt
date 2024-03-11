# ==============================================================================
# PMFLib CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRDGET_SRC
        main.cpp
        TrajClient.cpp
        TrajClientOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(trajectory-client ${TOPCRDGET_SRC})
ADD_DEPENDENCIES(trajectory-client cats_shared)

TARGET_LINK_LIBRARIES(trajectory-client Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            trajectory-client
        DESTINATION
            bin
        )

