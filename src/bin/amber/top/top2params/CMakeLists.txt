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
ADD_DEPENDENCIES(top2params cats_shared)

QT5_USE_MODULES(top2params Core)
TARGET_LINK_LIBRARIES(top2params
        ${CATS_LIBS})

INSTALL(TARGETS
            top2params
        DESTINATION
            bin
        )
