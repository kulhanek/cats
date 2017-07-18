# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(TOPTRAJ2RDF_SRC
        main.cpp
        TopTraj2Rdf.cpp
        TopTraj2RdfOptions.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(toptraj2rdf ${TOPTRAJ2RDF_SRC})
QT5_USE_MODULES(toptraj2rdf Core)
TARGET_LINK_LIBRARIES(toptraj2rdf
        ${CATS_LIBS})

INSTALL(TARGETS
            toptraj2rdf
        DESTINATION
            bin
        )

