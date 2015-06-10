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
QT5_USE_MODULES(pdb2fasta Core)
TARGET_LINK_LIBRARIES(pdb2fasta
        ${CATS_LIBS}
        )

INSTALL(TARGETS
            pdb2fasta
        DESTINATION
            bin
        )
