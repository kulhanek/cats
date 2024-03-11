# ==============================================================================
# CATs CMake File
# ==============================================================================

# program objects --------------------------------------------------------------
SET(PDB2FASTA_SRC
        pdb2fasta.cpp
        pdb2fasta-opts.cpp
        )

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(pdb2fasta ${PDB2FASTA_SRC})
ADD_DEPENDENCIES(pdb2fasta cats_shared)

TARGET_LINK_LIBRARIES(pdb2fasta Qt5::Core
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            pdb2fasta
        DESTINATION
            bin
        )
