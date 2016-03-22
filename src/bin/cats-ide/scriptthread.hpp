#ifndef SCRIPTTHREAD_HPP
#define SCRIPTTHREAD_HPP
#include <QScriptEngineDebugger>

//#include "CATs.hpp"
#include <QCATs.hpp>

// core support -------------------------------
#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>
#include <QRSelection.hpp>
#include <QTrajectory.hpp>
#include <QTrajPool.hpp>
#include <QOBMol.hpp>
#include <QAtom.hpp>
#include <QResidue.hpp>

// network support ----------------------------
#include <QNetTrajectory.hpp>
#include <QNetResults.hpp>

// geometry support ---------------------------
#include <QAverageSnapshot.hpp>
//#include <QPoint.hpp>
#include <QSimpleVector.hpp>
#include <QTransformation.hpp>
#include <QGeometry.hpp>
#include <QPMFLib.hpp>
#include <QCovarMatrix.hpp>

// data analysis ------------------------------
#include <QPropSum.hpp>
#include <QLinStat.hpp>
#include <QMinMax.hpp>
#include <QHistogram.hpp>
#include <QVolumeData.hpp>
#include <QThermoIG.hpp>
#include <Qx3DNA.hpp>
#include <QMolSurf.hpp>

// i/o suuport --------------------------------
#include <QOFile.hpp>
class ScriptThread : public QThread
{
private:
    QScriptEngine JSEngine;
    QScriptValue result;
    QString JSCode;
    QTextEdit textBrowser;
    void run()
    {
        new Q_DebugStream(std::cout, &textBrowser); //Redirect Console output to QTextEdit
        Q_DebugStream::registerQDebugMessageHandler(); //Redirect qDebug() output to QTextEdit

        result = JSEngine.evaluate(JSCode);
        //sleep(5);
    }
public:
    void registerCATs()
    {
        // register objects --------------------------
        QCATs::Register(JSEngine);

        // core support -------------------------------
        QTopology::Register(JSEngine);
        QSnapshot::Register(JSEngine);
        QSelection::Register(JSEngine);
        QRSelection::Register(JSEngine);
        QTrajectory::Register(JSEngine);
        QTrajPool::Register(JSEngine);
        QOBMol::Register(JSEngine);
        QAtom::Register(JSEngine);
        QResidue::Register(JSEngine);

        // network support ----------------------------
        QNetTrajectory::Register(JSEngine);
        QNetResults::Register(JSEngine);

        // geometry support ---------------------------
        QAverageSnapshot::Register(JSEngine);
        //QPoint::Register(JSEngine);
        QSimpleVector::Register(JSEngine);
        QTransformation::Register(JSEngine);
        QGeometry::Register(JSEngine);
        QPMFLib::Register(JSEngine);
        QCovarMatrix::Register(JSEngine);

        // data analysis ------------------------------
        QPropSum::Register(JSEngine);
        QLinStat::Register(JSEngine);
        QMinMax::Register(JSEngine);
        QHistogram::Register(JSEngine);
        QVolumeData::Register(JSEngine);
        QThermoIG::Register(JSEngine);
        Qx3DNA::Register(JSEngine);
        QMolSurf::Register(JSEngine);

        // i/o suuport --------------------------------
        QOFile::Register(JSEngine);
    }
    QString getOutput()
    {
        QString output = textBrowser.document()->toPlainText();
        textBrowser.clear();
        return output;
    }
    QScriptEngine* getEngine()
    {
        return &JSEngine;
    }
    void setCode(QString code)
    {
        JSCode = code;
    }
    QScriptValue getResult()
    {
        return result;
    }
};

#endif // SCRIPTTHREAD_HPP
