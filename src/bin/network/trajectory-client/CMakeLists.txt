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

QT5_USE_MODULES(trajectory-client Core)
TARGET_LINK_LIBRARIES(trajectory-client
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            trajectory-client
        DESTINATION
            bin
        )

