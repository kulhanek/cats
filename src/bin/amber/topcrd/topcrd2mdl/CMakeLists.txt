# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPCRD2MDL_SRC
        main.cpp
        TopCrd2Mdl.cpp
        TopCrd2MdlOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(topcrd2mdl ${TOPCRD2MDL_SRC})
ADD_DEPENDENCIES(topcrd2mdl cats_shared)

TARGET_LINK_LIBRARIES(topcrd2mdl Qt5::Core
        ${CATS_LIBS})

INSTALL(TARGETS
            topcrd2mdl
        DESTINATION
            bin
        )
