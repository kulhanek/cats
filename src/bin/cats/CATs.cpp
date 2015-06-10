// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <Terminal.hpp>

#include "CATs.hpp"

#include <QTextStream>
#include <QFile>

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
#include <QVolumeData.hpp>

// i/o suuport --------------------------------
#include <QOFile.hpp>

//------------------------------------------------------------------------------

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

const char*  CCATs::Prompt = NULL;
char*        CCATs::Line = NULL;
QString      CCATs::Contents;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CCATs::CCATs(void)
{
    QCATsScriptable::CATsEngine = &Engine;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CCATs::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    vout.Attach(Console);
    if( Options.GetOptVerbose() ) {
        vout.Verbosity(CVerboseStr::high);
    } else {
        vout.Verbosity(CVerboseStr::low);
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCATs::Run(void)
{
    // register objects --------------------------
    QCATs::Register(Engine);

    // core support -------------------------------
    QTopology::Register(Engine);
    QSnapshot::Register(Engine);
    QSelection::Register(Engine);
    QRSelection::Register(Engine);
    QTrajectory::Register(Engine);
    QTrajPool::Register(Engine);
    QOBMol::Register(Engine);
    QAtom::Register(Engine);
    QResidue::Register(Engine);

    // network support ----------------------------
    QNetTrajectory::Register(Engine);
    QNetResults::Register(Engine);

    // geometry support ---------------------------
    QAverageSnapshot::Register(Engine);
    QPoint::Register(Engine);
    QSimpleVector::Register(Engine);
    QTransformation::Register(Engine);
    QGeometry::Register(Engine);
    QPMFLib::Register(Engine);
    QCovarMatrix::Register(Engine);

    // data analysis ------------------------------
    QPropSum::Register(Engine);
    QLinStat::Register(Engine);
    QMinMax::Register(Engine);
    QHistogram::Register(Engine);
    QVolumeData::Register(Engine);

    // i/o suuport --------------------------------
    QOFile::Register(Engine);

    PrintWelcomeText();

    // evaluate script ---------------------------
    if( (CTerminal::IsTerminal(stdin) && (Options.GetNumberOfProgArgs() == 0)) || Options.GetOptInteractive() ) {
        if( RunInteractive() == false ) return(false);
    } else {
        if( Options.GetNumberOfProgArgs() == 0 ) {
            QTextStream stream(stdin);
            if( RunNonInteractive(stream) == false ) return(false);
        } else {
            QFile file(QString(Options.GetProgArg(0)));
            if( file.open(QIODevice::ReadOnly) == false ) {
                return(false);
            }
            QTextStream stream(&file);
            if( RunNonInteractive(stream) == false ) return(false);
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

void CCATs::PrintWelcomeText(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    CSmallString time_and_date;
    time_and_date = dt.GetSDateAndTime();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "#                     CATs - Conversion And Analysis Tools                      " << endl;
    vout << "# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    vout << "# Started at " << time_and_date << endl;
    vout << "# ==============================================================================" << endl;
    vout << endl;
}

//------------------------------------------------------------------------------

void CCATs::PrintFinalText(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    CSmallString time_and_date;
    time_and_date = dt.GetSDateAndTime();

    vout << endl;
    vout << "# ==============================================================================" << endl;
    vout << "# Finished at " << time_and_date;
    if( QCATs::ExitValue == 0 ){
        vout << " <b><blue>sucessfully</blue></b>." << endl;
    } else {
        vout << " <b><red>with an error</red></b>!" << endl;
    }
    vout << "# ==============================================================================" << endl;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCATs::RunNonInteractive(QTextStream& stream)
{
    // set script arguments
    for(int i=1; i < Options.GetNumberOfProgArgs(); i++ ){
        QCATs::SetScriptArgument(QString(Options.GetProgArg(i)));
    }

    int lineno = 1;
    // remove first line if contains '#!'
    Contents = stream.readLine();

    if( (Contents.size() > 0) && (Contents[0] == '#') ) {
        Contents = stream.readAll();
        lineno++;
    } else {
        Contents += "\n";
        Contents += stream.readAll();
    }

    Engine.evaluate(Contents,"",lineno);

    if( Engine.hasUncaughtException() == true ){
        QCATs::ExitValue = -1;
        return(false);
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CCATs::RunInteractive(void)
{
    // this is used in mixed mode noninteractive/interactive execution
    if( QCATs::ExitScript == true ) {
        return(true);
    }

    QTextStream stream(stderr);

    Contents = "";
    Prompt = "[cats]$ ";
    while( RLGets() != NULL ) {
        Contents += QString(Line);

        // test syntax
        QScriptSyntaxCheckResult syntax = QScriptEngine::checkSyntax(Contents);
        switch(syntax.state()) {
        case QScriptSyntaxCheckResult::Error:
            if( syntax.errorMessage() != "" ) {
                stream << "cats: " << syntax.errorMessage() << endl;
            } else {
                stream << "cats: syntax error" << endl;
            }

            Contents = "";
            Prompt = "[cats]$ ";
            break;

        case QScriptSyntaxCheckResult::Intermediate:
            Prompt = "\\ ";
            break;

        case QScriptSyntaxCheckResult::Valid:
            QScriptValue rvalue = Engine.evaluate(Contents);
            if( Engine.hasUncaughtException() ) {
                stream << "cats: " << rvalue.toString() << endl;
            }
            Contents = "";
            Prompt = "[cats]$ ";
            break;
        }
        // should we exit?
        if( QCATs::ExitScript == true ) {
            return(true);
        }
    }

    printf("\n");
    Engine.clearExceptions();

    return(true);
}

//------------------------------------------------------------------------------

/* Read a string, and return a pointer to it. Returns NULL on EOF. */
char* CCATs::RLGets(void)
{
    // If the buffer has already been allocated,
    // return the memory to the free pool.
    if( Line ) {
        free (Line);
        Line = NULL;
    }

    // Get a line from the user.
    Line = readline(Prompt);

    // If the line has any text in it,
    //   save it on the history.
    if( Line && *Line ) {
        add_history(Line);
    }

    return(Line);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CCATs::Finalize(void)
{
    if( Engine.hasUncaughtException() == true ) {
        QScriptValue evalue = Engine.uncaughtException();
        QTextStream stream(stderr);
        stream << "cats: line " << Engine.uncaughtExceptionLineNumber() << " - " << evalue.toString() << endl;
    }

    PrintFinalText();

    if( Options.GetOptVerbose() || ErrorSystem.IsError() || ( (QCATs::ExitValue != 0) && ErrorSystem.IsError() ) ) {
        ErrorSystem.PrintErrors(stderr);
    }

    vout << endl;

    return(true);
}

//------------------------------------------------------------------------------

int CCATs::GetExitValue(void)
{
    return(QCATs::ExitValue);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

