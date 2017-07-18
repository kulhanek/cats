// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <QScriptEngine>
#include <QCATs.hpp>
#include <moc_QCATs.cpp>
#include <boost/format.hpp>
#include <ErrorSystem.hpp>
#include <TerminalStr.hpp>

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
#include <QPoint.hpp>
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
#include <QHistogram2D.hpp>
#include <QVolumeData.hpp>
#include <QThermoIG.hpp>
#include <Qx3DNA.hpp>
#include <QCurvesP.hpp>
#include <QNAStat.hpp>
#include <QMolSurf.hpp>
#include <QTinySpline.hpp>

// i/o suuport --------------------------------
#include <QOFile.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;
using namespace boost::algorithm;

//------------------------------------------------------------------------------

int         QCATs::ExitValue = 0;
bool        QCATs::ExitScript = false;
QStringList QCATs::ScriptArguments;

//------------------------------------------------------------------------------

void RegisterAllCATsClasses(QScriptEngine& engine)
{
    // register engine for QCATsScriptable
    QCATsScriptable::CATsEngine = &engine;

    // register objects --------------------------
    QCATs::Register(engine);

    // core support -------------------------------
    QTopology::Register(engine);
    QSnapshot::Register(engine);
    QSelection::Register(engine);
    QRSelection::Register(engine);
    QTrajectory::Register(engine);
    QTrajPool::Register(engine);
    QOBMol::Register(engine);
    QAtom::Register(engine);
    QResidue::Register(engine);

    // network support ----------------------------
    QNetTrajectory::Register(engine);
    QNetResults::Register(engine);

    // geometry support ---------------------------
    QAverageSnapshot::Register(engine);
    QPoint::Register(engine);
    QSimpleVector::Register(engine);
    QTransformation::Register(engine);
    QGeometry::Register(engine);
    QPMFLib::Register(engine);
    QCovarMatrix::Register(engine);

    // data analysis ------------------------------
    QPropSum::Register(engine);
    QLinStat::Register(engine);
    QMinMax::Register(engine);
    QHistogram::Register(engine);
    QHistogram2D::Register(engine);
    QVolumeData::Register(engine);
    QThermoIG::Register(engine);
    Qx3DNA::Register(engine);
    QCurvesP::Register(engine);
    QNAStat::Register(engine);
    QMolSurf::Register(engine);
    QTinySpline::Register(engine);

    // i/o suuport --------------------------------
    QOFile::Register(engine);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QCATs::Register(QScriptEngine& engine)
{
    QObject* p_obj = new QCATs;
    QScriptValue objectValue = engine.newQObject(p_obj);
    engine.globalObject().setProperty("CATs", objectValue);

    QScriptValue fun;
    fun = engine.newFunction(QCATs::printf);
    engine.globalObject().setProperty("printf", fun);

    fun = engine.newFunction(QCATs::print);
    engine.globalObject().setProperty("print", fun);

    fun = engine.newFunction(QCATs::sprintf);
    engine.globalObject().setProperty("sprintf", fun);

    fun = engine.newFunction(QCATs::exit);
    engine.globalObject().setProperty("exit", fun);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QCATs::QCATs(void)
    : QCATsScriptable("cats")
{

}

//------------------------------------------------------------------------------

void QCATs::SetScriptArgument(const QString& arg)
{
    ScriptArguments << arg;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QCATs::system(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int system(cmd)" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("cmd",1);
    if( value.isError() ) return(value);

    QString cmd;
    value = GetArgAsString("cmd","cmd",1,cmd);
    if( value.isError() ) return(value);


// execute ---------------------------------------
    return( ::system(cmd.toLatin1().constData()) );
}

//------------------------------------------------------------------------------

QScriptValue QCATs::printErrors(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: printErrors()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ErrorSystem.PrintErrors(stdout);
    fprintf(stdout,"\n");

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QCATs::clearErrors(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: clearErrors()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    ErrorSystem.RemoveAllErrors();

    return(value);
}

//------------------------------------------------------------------------------

QScriptValue QCATs::numOfArguments(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: int numOfArguments()" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("",0);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    return(ScriptArguments.count());
}

//------------------------------------------------------------------------------

QScriptValue QCATs::argument(void)
{
    QScriptValue value;

// help ------------------------------------------
    if( IsHelpRequested() ){
        CTerminalStr sout;
        sout << "usage: string argument(index)" << endl;
        sout << "note:  index is counted from one" << endl;
        return(false);
    }

// check arguments -------------------------------
    value = CheckNumberOfArguments("index",1);
    if( value.isError() ) return(value);

    int index = 0;
    value = GetArgAsInt("index","index",1,index);
    if( value.isError() ) return(value);

// execute ---------------------------------------
    if( (index < 1) || (index > ScriptArguments.count() ) ){
        value = ThrowError("index","index is out of legal range");
        return(value);
    }

    return(ScriptArguments.at(index-1));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptValue QCATs::sprintf(QScriptContext* p_context, QScriptEngine* p_engine)
{
    return( sprintf("sprintf",p_context,p_engine).c_str() );
}

//------------------------------------------------------------------------------

QScriptValue QCATs::printf(QScriptContext* p_context, QScriptEngine* p_engine)
{
    std:: string str = sprintf("printf",p_context,p_engine);
    cout << str;
    return(QScriptValue());
}

//------------------------------------------------------------------------------

QScriptValue QCATs::exit(QScriptContext* p_context, QScriptEngine* p_engine)
{
    if( p_context->argumentCount() != 1 ) {
        p_context->throwError("exit(retcode) - illegal number of arguments, only one is expected");
        return(QScriptValue());
    }

    ExitScript = true;

    if( p_context->argument(0).isNumber() ){
        ExitValue = p_context->argument(0).toNumber();
    }

    p_engine->abortEvaluation( p_context->argument(0) );
    return(QScriptValue());
}

//------------------------------------------------------------------------------

const std::string QCATs::sprintf(const CSmallString& fname, QScriptContext* p_context,
                                QScriptEngine* p_engine)
{
    if( p_context->argumentCount() < 1 ) {
        CSmallString error;
        error << fname << "(format[,value1,value2,..]) - illegal number of arguments, at least one is expected";
        p_context->throwError(QString(error));
        return("");
    }

    QString format;
    format = p_context->argument(0).toString();

    // prepare format
    boost::format my_format;
    try {
        my_format.parse(format.toStdString());
    } catch(...) {
        CSmallString error;
        error << fname << "(format[,value1,value2,..]) - unable to parse format";
        p_context->throwError(QString(error));
        return("");
    }

    if( my_format.expected_args() != (p_context->argumentCount() - 1) ){
        CSmallString error;
        error << fname << "(format[,value1,value2,..]) - format requires different number of values (";
        error << my_format.expected_args() << ") than it is provided (" << (p_context->argumentCount() - 1) << ")";
        p_context->throwError(QString(error));
        return("");
    }

    // parse individual arguments
    for(int i=0; i < my_format.expected_args(); i++ ){
        QScriptValue val = p_context->argument(i+1);

        try {
            if( val.isNumber() ){
                double sval = val.toNumber();
                my_format % sval;
            } else {
                QString sval = val.toString();
                my_format % sval.toStdString();
            }
        } catch(...){
            CSmallString error;
            error << fname << "(format[,value1,value2,..]) - unable to process argument (";
            error << i + 1 << ")";
            p_context->throwError(QString(error));
            return("");
        }
    }

    // return string
    return( my_format.str() );
}

//------------------------------------------------------------------------------

QScriptValue QCATs::print(QScriptContext* p_context, QScriptEngine* p_engine)
{
    for(int i = 0; i < p_context->argumentCount(); i++){
        QScriptValue val = p_context->argument(i);
        QString sval = val.toString();
        cout << sval.toStdString();
    }
    cout << endl;
    return(QScriptValue());
}

//------------------------------------------------------------------------------

QScriptValue QCATs::include(void)
{
    QScriptValue value;

//// help ------------------------------------------
//    if( IsHelpRequested() ){
//        CTerminalStr sout;
//        sout << "usage: include(name)" << endl;
//        return(false);
//    }

//// check arguments -------------------------------
//    value = CheckNumberOfArguments("name",1);
//    if( value.isError() ) return(value);

//    QString name;
//    value = GetArgAsString("name","name",1,name);
//    if( value.isError() ) return(value);
//// execute ---------------------------------------

//    string include_path;
//    // inlude current working directory
//    include_path += string(".");
//    include_path += string(":");
//    // include paths provided by
//    include_path += string(getenv("CATS_INCLUDE_PATH"));

//    // split into paths
//    vector<string> paths;
//    split(paths,include_path,any_of(":"));

//    // find script
//    vector<string>::iterator it = paths.begin();
//    vector<string>::iterator ie = paths.end();

//    while( it != ie ){
//        QString full_name =
//        Qfile::exists()
//        it++;
//    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


