# ==============================================================================
# CATs CMake File
# ==============================================================================

SET(CATS_LIB_SRC
        CATsMainHeader.cpp

    # network support ----------------------------
        network/CATsOperation.cpp
        network/result/ResultFile.cpp
        network/result/ResultItem.cpp
        network/result/ResultClient.cpp
        network/result/COpRegisterClient.cpp
        network/result/COpWriteData.cpp
        network/result/COpGetStatistics.cpp
        network/result/COpFlushServerData.cpp

        network/trajectory/TrajectoryClient.cpp
        network/trajectory/COpGetSnapshot.cpp

    # map support --------------------------------
        maps/ResidueMaps.cpp

    # pmflib support -----------------------------
        pmflib/PMFLibCV.cpp
        pmflib/PMFLibCVs.cpp

        pmflib/cvs/cv_acn.cpp
        pmflib/cvs/cv_acyl.cpp
        pmflib/cvs/cv_ang.cpp
        pmflib/cvs/cv_asph.cpp
        pmflib/cvs/cv_axang.cpp
        pmflib/cvs/cv_cang.cpp
        pmflib/cvs/cv_dd.cpp
        pmflib/cvs/cv_dds.cpp
        pmflib/cvs/cv_dih.cpp
        pmflib/cvs/cv_dis.cpp
        pmflib/cvs/cv_ds.cpp
        pmflib/cvs/cv_egap.cpp
        pmflib/cvs/cv_epot.cpp
        pmflib/cvs/cv_mdis.cpp
        pmflib/cvs/cv_mdisc.cpp
        pmflib/cvs/cv_mdisg.cpp
        pmflib/cvs/cv_odis.cpp
        pmflib/cvs/cv_ods.cpp
        pmflib/cvs/cv_pang.cpp
        pmflib/cvs/cv_pmgtd.cpp
        pmflib/cvs/cv_pmogt.cpp
        pmflib/cvs/cv_pos.cpp
        pmflib/cvs/cv_rgyr.cpp
        pmflib/cvs/cv_rmsdl.cpp
        pmflib/cvs/cv_rmsdt.cpp
        pmflib/cvs/cv_sanis.cpp
        pmflib/cvs/cv_plane.cpp
        pmflib/cvs/cv_puck6q.cpp
        pmflib/cvs/cv_puck6t.cpp
        pmflib/cvs/cv_puck6p.cpp
        )

# scripting engine -------------------------------------------------------------

SET(CATS_JSCRIPT_SRC
        jscript/QCATs.cpp
        jscript/QCATsScriptable.cpp

    # core support -------------------------------
        jscript/QTopology.cpp
        jscript/QSnapshot.cpp        
        jscript/QSelection.cpp
        jscript/QRSelection.cpp
        jscript/QAtom.cpp
        jscript/QResidue.cpp
        jscript/QTrajectory.cpp
        jscript/QTrajPool.cpp
        jscript/QOBMol.cpp

    # network support ----------------------------
        jscript/QNetTrajectory.cpp
        jscript/QNetResults.cpp

    # geometry support ---------------------------
        jscript/QAverageSnapshot.cpp
        jscript/QPoint.cpp
        jscript/QSimpleVector.cpp
        jscript/QTransformation.cpp
        jscript/QGeometry.cpp
        jscript/QPMFLibCVs.cpp
        jscript/QCovarMatrix.cpp

    # data analysis ------------------------------   
        jscript/QPropSum.cpp
        jscript/QLinStat.cpp
        jscript/QMinMax.cpp
        jscript/QHistogram.cpp
        jscript/QVolumeData.cpp

    # i/o suuport --------------------------------
        jscript/QOFile.cpp
    )

SET(CATS_LIB_SRC ${CATS_LIB_SRC} ${CATS_JSCRIPT_SRC})

#-------------------------------------------------------------------------------

# create static library --------------------------------------------------------
IF(LIBS_STATIC)
    ADD_LIBRARY(cats_static STATIC ${CATS_LIB_SRC})
    SET_TARGET_PROPERTIES(cats_static PROPERTIES
                            OUTPUT_NAME cats
                            CLEAN_DIRECT_OUTPUT 1)
ENDIF(LIBS_STATIC)

# create shared library --------------------------------------------------------
IF(LIBS_SHARED)
    ADD_DEFINITIONS(-DCATS_BUILDING_DLL)
    ADD_LIBRARY(cats_shared SHARED ${CATS_LIB_SRC})
    SET_TARGET_PROPERTIES(cats_shared PROPERTIES
                            OUTPUT_NAME cats
                            CLEAN_DIRECT_OUTPUT 1
                            VERSION ${CATS_VERSION})
    QT5_USE_MODULES(cats_shared Core Script)

INSTALL(TARGETS
            cats_shared
        DESTINATION
            lib
        )
ENDIF(LIBS_SHARED)

#-------------------------------------------------------------------------------
