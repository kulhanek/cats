# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(VS_CLIENT_SRC
        main.cpp
        VSClient.cpp
        VSClientOptions.cpp
        COpGetData.cpp
        COpWriteData.cpp
        COpClientPkg.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(svs-client ${VS_CLIENT_SRC})
ADD_DEPENDENCIES(svs-client cats_shared)

TARGET_LINK_LIBRARIES(svs-client Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            svs-client
        DESTINATION
            bin
        )

