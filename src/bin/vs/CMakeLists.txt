# ==============================================================================
# ChemInfo CMake File
# ==============================================================================

# general ----------------------------------------
ADD_SUBDIRECTORY(mol-extract)
ADD_SUBDIRECTORY(mol-split)
ADD_SUBDIRECTORY(mol-optimize)
ADD_SUBDIRECTORY(mol-rearrange)
ADD_SUBDIRECTORY(mol-applyres)
ADD_SUBDIRECTORY(mol-rmsd)
#ADD_SUBDIRECTORY(mol-gen3d)

# virtual screening projects ---------------------
ADD_SUBDIRECTORY(vs-project-list)
ADD_SUBDIRECTORY(vs-project-create)
ADD_SUBDIRECTORY(vs-project-info)
ADD_SUBDIRECTORY(vs-project-remove)
ADD_SUBDIRECTORY(vs-project-copy)
ADD_SUBDIRECTORY(vs-project-alter)
ADD_SUBDIRECTORY(vs-project-stat)
ADD_SUBDIRECTORY(vs-project-stat2)
ADD_SUBDIRECTORY(vs-project-fix)
ADD_SUBDIRECTORY(vs-project-add-structures)

ADD_SUBDIRECTORY(vs-client)
ADD_SUBDIRECTORY(vs-server)


