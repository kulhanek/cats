// =============================================================================
// VScreen - Virtual Screening Tools
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

#include <stdio.h>
#include <ErrorSystem.hpp>
#include <SmallTime.hpp>
#include <SmallTimeAndDate.hpp>
#include <FileSystem.hpp>
#include <FileName.hpp>
#include <sstream>
#include <Transformation.hpp>
#include <iomanip>

#include "MolRmsd.hpp"
#include "MolRmsdOptions.hpp"

// openbabel
#include "openbabel/mol.h"
#include "openbabel/obconversion.h"
#include "openbabel/generic.h"
#include "openbabel/forcefield.h"

using namespace std;
using namespace OpenBabel;

//------------------------------------------------------------------------------

CMolRmsd MolRmsd;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CMolRmsd::CMolRmsd(void)
{
    NumberOfCouples = 0;
}

//------------------------------------------------------------------------------

CMolRmsd::~CMolRmsd(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CMolRmsd::Init(int argc,char* argv[])
{
    // encode program options, all check procedures are done inside of CMolRmsdOptions
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if(result != SO_CONTINUE) return(result);

    // set output stream
    MsgOut.Attach(cout);
    MsgOut.Verbosity(CVerboseStr::low);
    if( Options.GetOptVerbose() ) MsgOut.Verbosity(CVerboseStr::high);

    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();
    MsgOut << high;
    MsgOut << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# xyzfit started at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# Reference molecule name : " << Options.GetProgArg(0) << endl;
    MsgOut << "# Analyzed molecule name  : " << Options.GetProgArg(1) << endl;
    if( Options.GetNumberOfProgArgs() > 2 ){
    MsgOut << "# Output molecule name    : " << Options.GetProgArg(2) << endl;
    }
    MsgOut << "# Pattern                 : " << Options.GetOptPattern() << endl;

    return(SO_CONTINUE);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMolRmsd::Run(void)
{
    MsgOut << endl;
    MsgOut << "::::::::::::::::::::::::::::::: Processing Data ::::::::::::::::::::::::::::::" << endl;

    // read molecules --------------------------------
    MsgOut << endl;
    MsgOut << "1) Reading molecule #1 ..." << endl;
    if(Ref.ReadMol(Options.GetProgArg(0),Options.GetOptRefFormat()) == false) return(false);
    MsgOut << "   Number of atoms = " << Ref.NumAtoms() << endl;

    MsgOut << endl;
    MsgOut << "2) Reading molecule #2 ..." << endl;
    if(Str.ReadMol(Options.GetProgArg(1),Options.GetOptStrFormat()) == false) return(false);
    MsgOut << "   Number of atoms = " << Str.NumAtoms() << endl;

    if(DecodePattern() == false) return(false);
    if( Options.GetOptNoFit() == false ){
        if(FitStructures() == false) return(false);
    } else {
    MsgOut << endl;
    MsgOut << "4) Fitting structure ... disabled upon user request" << endl;
    }
    CalcRMSD();

    return(true);
}

//------------------------------------------------------------------------------

bool CMolRmsd::DecodePattern(void)
{
    MsgOut << endl;
    MsgOut << "3) Decoding pattern ..." << endl;

    if(Options.GetOptPattern() == "identity") {
        if(Ref.NumAtoms() != Str.NumAtoms()) {
            ES_ERROR("number of atoms in both molecules is not the same");
            return(false);
        }
        NumberOfCouples = Ref.NumAtoms();
    } else {
        // count number of couples in pattern
        NumberOfCouples = 1;
        for(unsigned int i=0; i < Options.GetOptPattern().GetLength(); i++) {
            if(Options.GetOptPattern()[i] == ',') NumberOfCouples++;
        }
    }
    MsgOut << "   Number of couples = " << NumberOfCouples << endl;

    // allocate fit arrays
    bool result = true;
    RefData.CreateVector(NumberOfCouples);
    StrData.CreateVector(NumberOfCouples);
    if(result == false) {
        ES_ERROR("unable to allocate memory for pattern");
        return(false);
    }

    if(Options.GetOptPattern() == "identity") {
        for(int i=0; i < NumberOfCouples; i++) {
            RefData[i].x = Ref.GetAtom(i+1)->GetX();
            RefData[i].y = Ref.GetAtom(i+1)->GetY();
            RefData[i].z = Ref.GetAtom(i+1)->GetZ();
            StrData[i].x = Str.GetAtom(i+1)->GetX();
            StrData[i].y = Str.GetAtom(i+1)->GetY();
            StrData[i].z = Str.GetAtom(i+1)->GetZ();
        }
    } else {
        CSmallString buffer(Options.GetOptPattern());
        char* p_current = strtok(buffer.GetBuffer(),",");
        for(int i=0; i < NumberOfCouples; i++) {
            if(p_current == NULL) {
                ES_ERROR("couple definition is missing after comma");
                return(false);
            }
            // get couple indexes
            unsigned int  a1=0;
            unsigned int  a2=0;
            char mchar=' ';
            sscanf(p_current,"%u%c%u",&a1,&mchar,&a2);
            if(mchar != ':') {
                CSmallString error;
                error << "incorrect separator between numbers: '" << mchar << "' provided but ':' expected";
                ES_ERROR(error);
                return(false);
            }
            if((a1 <= 0) || (a1 > Ref.NumAtoms())) {
                CSmallString error;
                error << "illegal index for reference atom: " << a1;
                ES_ERROR(error);
                return(false);
            }
            if((a2 <= 0) || (a2 > Str.NumAtoms())) {
                CSmallString error;
                error << "illegal index for superimposed atom: " << a2;
                ES_ERROR(error);
                return(false);
            }
            // copy data
            MsgOut << "   Map: " << a1 << " <- " << a2 << endl;

            RefData[i].x = Ref.GetAtom(a1)->GetX();
            RefData[i].y = Ref.GetAtom(a1)->GetY();
            RefData[i].z = Ref.GetAtom(a1)->GetZ();
            StrData[i].x = Str.GetAtom(a2)->GetX();
            StrData[i].y = Str.GetAtom(a2)->GetY();
            StrData[i].z = Str.GetAtom(a2)->GetZ();

            p_current = strtok(NULL,",");
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CMolRmsd::FitStructures(void)
{

    MsgOut << endl;
    MsgOut << "4) Fitting structure ..." << endl;
    // check if selected atoms are in origin
    CPoint  centre1;
    CPoint  centre2;

    for(int i=0; i < NumberOfCouples; i++) {
        centre1 += RefData[i];
        centre2 += StrData[i];
    }

    centre1 = centre1 / NumberOfCouples;
    centre2 = centre2 / NumberOfCouples;

    MsgOut << "   Reference atom centre is : " << centre1.x << " " << centre1.y << " " << centre1.z << endl;
    MsgOut << "   Fitted atom centre is    : " << centre2.x << " " << centre2.y << " " << centre2.z << endl;
    MsgOut << "   Moving to origin ..." << endl;

    for(int i=0; i < NumberOfCouples; i++) {
        RefData[i] -= centre1;
        StrData[i] -= centre2;
    }

    MsgOut << "   Quaterion fitting ..." << endl;

    CTransformation trans;

    trans.QuaternionFit(RefData,StrData);

    MsgOut << "   Data transformation ..." << endl;

    for(int i=0; i < NumberOfCouples; i++) {
        trans.Apply(StrData[i]);
    }

    if( Options.GetNumberOfProgArgs() == 3 ){
        // transform the whole structure
        OBAtomIterator it = Str.BeginAtoms();
        OBAtomIterator ie = Str.EndAtoms();

        while( it != ie ){
            OBAtom* p_atom = *it;
            CPoint pos(p_atom->GetX(),p_atom->GetY(),p_atom->GetZ());
            pos -= centre2;
            trans.Apply(pos);
            pos += centre1;
            p_atom->SetVector(pos.x,pos.y,pos.z);
            it++;
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

void CMolRmsd::CalcRMSD(void)
{
    MsgOut << endl;
    MsgOut << "4) Calculating RMSD ..." << endl;

    double rmsd = 0;

    for(int i=0; i < NumberOfCouples; i++) {
        rmsd += Square(RefData[i]-StrData[i]);
    }

    rmsd = sqrt(rmsd / NumberOfCouples);

    if( Options.GetOptVerbose() ){
        MsgOut << "   RMSD = " << fixed << setprecision(5) << rmsd << endl;
    } else {
        MsgOut << low;
        MsgOut << fixed << setprecision(5) << rmsd;
        MsgOut << high;
    }

    if( Options.GetNumberOfProgArgs() == 3 ){
        MsgOut << endl;
        MsgOut << "5) Saving superimposed structure ..." << endl;
        Str.WriteMol(Options.GetProgArg(2),Options.GetOptOutFormat());
    }
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CMolRmsd::Finalize(void)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    MsgOut << "# ==============================================================================" << endl;
    MsgOut << "# xyzfit terminated at " << dt.GetSDateAndTime() << endl;
    MsgOut << "# ==============================================================================" << endl;

    if( ErrorSystem.IsError() || Options.GetOptVerbose() ){
        MsgOut << low;
        ErrorSystem.PrintErrors(stderr);
    }

    MsgOut << endl;
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

