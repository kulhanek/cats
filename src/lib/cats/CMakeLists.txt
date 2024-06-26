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
        )

# scripting engine -------------------------------------------------------------

SET(CATS_JSCRIPT_SRC
        jscript/QCATs.cpp
        jscript/QCATsScriptable.cpp

    # sqlite support -----------------------------
        sqlite3/sqlite3.c

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
        jscript/QPMFLib.cpp
        jscript/QCovarMatrix.cpp

    # data analysis ------------------------------   
        jscript/QPropSum.cpp
        jscript/QLinStat.cpp
        jscript/QMinMax.cpp
        jscript/QHistogram.cpp
        jscript/QHistogram2D.cpp
        jscript/QVolumeData.cpp
        jscript/QThermoIG.cpp
        jscript/QNAHelper.cpp
        jscript/Qx3DNA.cpp
        jscript/QCurvesP.cpp
        jscript/QNAStatHelper.cpp
        jscript/QNAStat.cpp
        jscript/QMolSurf.cpp
        jscript/QTinySpline.cpp

    # i/o suuport --------------------------------
        jscript/QOFile.cpp

    # vs support ---------------------------------
        vs/VSOperation.cpp
        vs/InfMol.cpp

    # helpers ------------------------------------
        jscript/tinyspline/tinyspline.c
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

    TARGET_LINK_LIBRARIES(cats_shared ${SYSTEM_LIBS} Qt5::Core Qt5::Script)

    SET_TARGET_PROPERTIES(cats_shared PROPERTIES
                            OUTPUT_NAME cats
                            CLEAN_DIRECT_OUTPUT 1
                            VERSION ${LibBuildVersion_CATs})

INSTALL(TARGETS
            cats_shared
        DESTINATION
            lib
        )
ENDIF(LIBS_SHARED)

#-------------------------------------------------------------------------------
