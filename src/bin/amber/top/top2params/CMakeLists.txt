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

TARGET_LINK_LIBRARIES(top2params Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            top2params
        DESTINATION
            bin
        )
