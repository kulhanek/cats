# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(RS_SERVER_SRC
        main.cpp
        RSServer.cpp
        RSServerOptions.cpp
        RSProcessor.cpp
        RSFactory.cpp
        SOpWriteData.cpp
        SOpFlushServerData.cpp
        SOpGetStatistics.cpp
        SOpRegisterClient.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(result-server ${RS_SERVER_SRC})
ADD_DEPENDENCIES(result-server cats_shared)

TARGET_LINK_LIBRARIES(result-server Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            result-server
        DESTINATION
            bin
        )

