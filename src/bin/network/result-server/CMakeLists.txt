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
QT5_USE_MODULES(result-server Core)

TARGET_LINK_LIBRARIES(result-server
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            result-server
        DESTINATION
            bin
        )

