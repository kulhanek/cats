# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(RESULT_CLIENT_SRC
        main.cpp
        ResClient.cpp
        ResClientOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(result-client ${RESULT_CLIENT_SRC})
QT5_USE_MODULES(result-client Core)
TARGET_LINK_LIBRARIES(result-client
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            result-client
        DESTINATION
            bin
        )
