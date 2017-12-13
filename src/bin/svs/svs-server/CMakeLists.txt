# ==============================================================================
# PMFLib CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_SERVER_SRC
        main.cpp
        VSServer.cpp
        VSServerOptions.cpp
        VSProcessor.cpp
        VSFactory.cpp
        OpGetData.cpp
        OpWriteData.cpp
        OpClientPkg.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-server ${VS_SERVER_SRC})
QT5_USE_MODULES(svs-server Core)

TARGET_LINK_LIBRARIES(svs-server
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-server
        DESTINATION
            bin
        )
