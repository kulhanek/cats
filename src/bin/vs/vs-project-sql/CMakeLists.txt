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
        COpSelReset.cpp
        COpClientPkg.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(vs-client ${VS_CLIENT_SRC})
QT5_USE_MODULES(vs-client Core)

TARGET_LINK_LIBRARIES(vs-client
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            vs-client
        DESTINATION
            bin
        )

