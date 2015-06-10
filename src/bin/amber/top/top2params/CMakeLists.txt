# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOP2PARAMS_SRC
        main.cpp
        Top2Params.cpp
        Top2ParamsOptions.cpp
        MMTypes.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(top2params ${TOP2PARAMS_SRC})
QT5_USE_MODULES(top2params Core)
TARGET_LINK_LIBRARIES(top2params
                ${ASL_LIB_NAME}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS})

INSTALL(TARGETS
            top2params
        DESTINATION
            bin
        )
