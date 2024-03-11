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
ADD_DEPENDENCIES(result-client cats_shared)

TARGET_LINK_LIBRARIES(result-client Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            result-client
        DESTINATION
            bin
        )
