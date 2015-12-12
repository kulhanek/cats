# ==============================================================================
# PMFLib CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TS_SERVER_SRC
        main.cpp
        TSServer.cpp
        TSServerOptions.cpp
        TSProcessor.cpp
        TSFactory.cpp
        SOpGetSnapshot.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(trajectory-server ${TS_SERVER_SRC})
ADD_DEPENDENCIES(trajectory-server cats_shared)

QT5_USE_MODULES(trajectory-server Core)
TARGET_LINK_LIBRARIES(trajectory-server
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            trajectory-server
        DESTINATION
            bin
        )
