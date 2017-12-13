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
        OpSelReset.cpp
        OpClientPkg.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(vs-server ${VS_SERVER_SRC})

TARGET_LINK_LIBRARIES(vs-server
        ${CHEMINFO_LIBRARIES}
        )

INSTALL(TARGETS
            vs-server
        DESTINATION
            bin
        )
