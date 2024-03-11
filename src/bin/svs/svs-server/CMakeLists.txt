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
ADD_DEPENDENCIES(svs-server cats_shared)

TARGET_LINK_LIBRARIES(svs-server Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-server
        DESTINATION
            bin
        )
