// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2008 Petr Kulhanek, kulhanek@enzim.hu
//    Copyright (C) 2005 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2004 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <errno.h>
#include <string.h>
#include <ErrorSystem.hpp>
#include <PeriodicTable.hpp>
#include <FileName.hpp>

#include <SmallTimeAndDate.hpp>
#include <AmberMaskAtoms.hpp>
#include <AmberMaskResidues.hpp>
#include <AmberTrajectory.hpp>
#include <PrincipalAxes.hpp>
#include <Transformation.hpp>
#include <stack>

#include "TopCrdManip.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrdManip::CTopCrdManip(void)
{
    // image options
    ImageToOrigin = false;
    AtomBasedImaging = false;
    FamiliarImaging = false;

    // center options
    CenterToOrigin = false;
    CenterNoMass = false;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrdManip::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        printf("\n");
        printf("# ==============================================================================\n");
        printf("# topcrdmanip (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        if( Options.GetArgInCrdName() != "-" ) {
            printf("# Input coordinates  : %s\n",(const char*)Options.GetArgInCrdName());
        } else {
            printf("# Input coordinates  : - (standard input)\n");
        }
        if( Options.GetArgOutCrdName() != "-" ) {
            printf("# Output coordinates : %s\n",(const char*)Options.GetArgOutCrdName());
        } else {
            printf("# Output coordinates : - (standard output)\n");
        }
        printf("# Command            : %s\n",(const char*)Options.GetArgCommand());
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    // load coordinates
    Coordinates.AssignTopology(&Topology);
    if( Coordinates.Load(Options.GetArgInCrdName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                (const char*)Options.GetArgInCrdName());
        return(false);
    }

    // update box
    Topology.BoxInfo.SetBoxDimmensions(Coordinates.GetBox());
    Topology.BoxInfo.SetBoxAngles(Coordinates.GetAngles());
    Topology.BoxInfo.UpdateBoxMatrices();

    // execute command
    bool result = true;
    if( Options.GetArgCommand() == "identity" ) {
        result = true;
    } else if( Options.GetArgCommand() == "center" ) {
        result &= CenterCoordinates();
    }

    else if( Options.GetArgCommand() == "image" ) {
        result &= ImageCoordinates();
    }

    else if( Options.GetArgCommand() == "swap" ) {
        result &= SwapCoordinates();
    }

    else if( Options.GetArgCommand() == "move" ) {
        result &= MoveCoordinates();
    }

    else if( Options.GetArgCommand() == "moveto" ) {
        result &= MoveToCoordinates();
    }

    else if( Options.GetArgCommand() == "principal" ) {
        result &= Principal();
    }

    else if( Options.GetArgCommand() == "bondrot" ) {
        result &= BondRot();
    }

    else {
        fprintf(stderr,">>> ERROR: Command %s is not supported!\n",
                (const char*)Options.GetArgCommand());
        result = false;
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to process command: %s\n",
                (const char*)Options.GetArgCommand());
        fprintf(stderr,"           Final coordinates will not be saved!\n");
        ES_ERROR("some error occured during coordinate manipulation");
        return(false);
    }

    // save coordinates
    if( Coordinates.Save(Options.GetArgOutCrdName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to save specified coordinates: %s\n",
                (const char*)Options.GetArgOutCrdName());
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# %s terminated at %s\n",(const char*)Options.GetProgramName(),(const char*)dt.GetSDateAndTime());
        fprintf(stdout,"# ==============================================================================\n");
    }

    if( Options.GetOptVerbose() || ErrorSystem.IsError() ) {
        ErrorSystem.PrintErrors(stderr);
        fprintf(stdout,"\n");
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::ImageCoordinates(void)
{
    // decode options
    if( DecodeImageOptions() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    // do imaging
    if( AtomBasedImaging == true ) {
        for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
            CPoint pos = Coordinates.GetPosition(i);
            pos = Topology.BoxInfo.ImagePoint(pos,0,0,0,ImageToOrigin,FamiliarImaging);
            Coordinates.SetPosition(i,pos);
        }
    } else {
        Topology.InitMoleculeIndexes();
        int start_index = 0;
        while( start_index < Topology.AtomList.GetNumberOfAtoms() ) {
            int mol_id = Topology.AtomList.GetAtom(start_index)->GetMoleculeIndex();

            // calculate molecule COM
            CPoint  com;
            double  tmass = 0.0;
            int     count = 0;
            for(int i=start_index; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
                if( mol_id != Topology.AtomList.GetAtom(i)->GetMoleculeIndex() ) break;
                double mass = Topology.AtomList.GetAtom(i)->GetMass();
                com += Coordinates.GetPosition(i)*mass;
                tmass += mass;
                count++;
            }
            if( tmass != 0.0 ) com /= tmass;

            // image com
            CPoint  icom;
            icom = Topology.BoxInfo.ImagePoint(com,0,0,0,ImageToOrigin,FamiliarImaging);
            icom = icom - com;

            // move molecule
            for(int i=start_index; i < start_index+count; i++) {
                CPoint pos = Coordinates.GetPosition(i);
                pos += icom;
                Coordinates.SetPosition(i,pos);
            }
            start_index += count;
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodeImageOptions(void)
{
    for(int i=4; i < Options.GetNumberOfProgArgs(); i++) {
        if( Options.GetProgArg(i) == "origin" ) {
            ImageToOrigin = true;
            continue;
        }
        if( Options.GetProgArg(i) == "familiar" ) {
            FamiliarImaging = true;
            continue;
        }
        if( Options.GetProgArg(i) == "byatom" ) {
            AtomBasedImaging = true;
            continue;
        }
        // unrecognized options
        fprintf(stderr,">>> ERROR: Image option '%s' is not supported!\n",
                (const char*)Options.GetProgArg(i));
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::CenterCoordinates(void)
{
    // decode options
    if( DecodeCenterOptions() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask;

    mask.AssignTopology(&Topology);
    mask.AssignCoordinates(&Coordinates);

    if( mask.SetMask(CenterMask) == false ) {
        ES_ERROR("unable to set mask");
        return(false);
    }

    // calculate COM
    CPoint  com;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask.GetSelectedAtom(i) == NULL ) continue;
        double mass = 1.0;
        if( CenterNoMass == false ) mass = Topology.AtomList.GetAtom(i)->GetMass();
        com += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com /= tmass;

    // move com
    CPoint dmov;

    dmov -= com;
    if( CenterToOrigin == false ) {
        dmov += Topology.BoxInfo.GetBoxCenter();
    }

    // move atoms
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CPoint pos = Coordinates.GetPosition(i);
        pos += dmov;
        Coordinates.SetPosition(i,pos);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodeCenterOptions(void)
{
    if( Options.GetNumberOfProgArgs() >= 5 ) {
        CenterMask = Options.GetProgArg(4);
    }

    for(int i=5; i < Options.GetNumberOfProgArgs(); i++) {
        if( Options.GetProgArg(i) == "origin" ) {
            CenterToOrigin = true;
            continue;
        }
        if( Options.GetProgArg(i) == "nomass" ) {
            CenterNoMass = true;
            continue;
        }
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::SwapCoordinates(void)
{
    // decode options
    if( DecodeSwapOptions() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask1;
    CAmberMaskAtoms     mask2;

    mask1.AssignTopology(&Topology);
    mask1.AssignCoordinates(&Coordinates);

    if( mask1.SetMask(SwapMask1) == false ) {
        ES_ERROR("unable to set mask 1");
        return(false);
    }

    mask2.AssignTopology(&Topology);
    mask2.AssignCoordinates(&Coordinates);

    if( mask2.SetMask(SwapMask2) == false ) {
        ES_ERROR("unable to set mask 2");
        return(false);
    }

    // calculate COM
    CPoint  com1;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com1 += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com1 /= tmass;

    CPoint  com2;
    tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask2.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com2 += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com2 /= tmass;

    CPoint shift;

    shift = com1 - com2;

    // move atoms
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        CPoint pos = Coordinates.GetPosition(i);
        pos -= shift;
        Coordinates.SetPosition(i,pos);
    }
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask2.GetSelectedAtom(i) == NULL ) continue;
        CPoint pos = Coordinates.GetPosition(i);
        pos += shift;
        Coordinates.SetPosition(i,pos);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodeSwapOptions(void)
{
    if( Options.GetNumberOfProgArgs() != 6 ) {
        return(false);
    }

    SwapMask1 = Options.GetProgArg(4);
    SwapMask2 = Options.GetProgArg(5);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::MoveCoordinates(void)
{
    // decode options
    if( DecodeMoveOptions() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask;

    mask.AssignTopology(&Topology);
    mask.AssignCoordinates(&Coordinates);

    if( mask.SetMask(MoveMask) == false ) {
        ES_ERROR("unable to set mask");
        return(false);
    }

    CPoint shift;

    shift = MoveData;

    // move atoms
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask.GetSelectedAtom(i) == NULL ) continue;
        CPoint pos = Coordinates.GetPosition(i);
        pos += shift;
        Coordinates.SetPosition(i,pos);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodeMoveOptions(void)
{
    if( Options.GetNumberOfProgArgs() != 8 ) {
        return(false);
    }

    MoveMask = Options.GetProgArg(4);

    MoveData.x = Options.GetProgArg(5).ToDouble();
    MoveData.y = Options.GetProgArg(6).ToDouble();
    MoveData.z = Options.GetProgArg(7).ToDouble();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::MoveToCoordinates(void)
{
    // decode options
    if( DecodeMoveToOptions() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask;

    mask.AssignTopology(&Topology);
    mask.AssignCoordinates(&Coordinates);

    if( mask.SetMask(MoveMask) == false ) {
        ES_ERROR("unable to set mask");
        return(false);
    }

    // calculate COM
    CPoint  com;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com /= tmass;

    CPoint shift;

    shift = MoveData - com;

    // move atoms
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask.GetSelectedAtom(i) == NULL ) continue;
        CPoint pos = Coordinates.GetPosition(i);
        pos += shift;
        Coordinates.SetPosition(i,pos);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodeMoveToOptions(void)
{
    if( Options.GetNumberOfProgArgs() != 8 ) {
        return(false);
    }

    MoveMask = Options.GetProgArg(4);

    MoveData.x = Options.GetProgArg(5).ToDouble();
    MoveData.y = Options.GetProgArg(6).ToDouble();
    MoveData.z = Options.GetProgArg(7).ToDouble();

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::Principal(void)
{
    // decode options
    if( DecodePrincipal() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask;

    mask.AssignTopology(&Topology);
    mask.AssignCoordinates(&Coordinates);

    if( mask.SetMask(PrincipalMask) == false ) {
        ES_ERROR("unable to set mask");
        return(false);
    }

    // calculate COM
    CPoint  com;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com /= tmass;


    CPrincipalAxes  princip_axes;

    for(int i=0; i  < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        princip_axes.AddPoint(Coordinates.GetPosition(i)-com, mass);
    }

    princip_axes.CalcPrincipalAxes();

    // C -> X, B -> Y, A -> Z (the largest moment is aligned witw z-axis
    CPoint aaxis = princip_axes.GetCAxis();
    CPoint baxis = princip_axes.GetBAxis();
    CPoint caxis = princip_axes.GetAAxis();

    // http://answers.google.com/answers/threadview/id/556169.html
    // 3-point alignment, coordinate conversion, ...
    // [Barycentric Coordinates], http://www.cap-lore.com/MathPhys/bcc.html

    CTransformation align_trans;

    align_trans.Field[0][0] = aaxis.x;
    align_trans.Field[1][0] = aaxis.y;
    align_trans.Field[2][0] = aaxis.z;
    align_trans.Field[0][1] = baxis.x;
    align_trans.Field[1][1] = baxis.y;
    align_trans.Field[2][1] = baxis.z;
    align_trans.Field[0][2] = caxis.x;
    align_trans.Field[1][2] = caxis.y;
    align_trans.Field[2][2] = caxis.z;

    CTransformation trans;

    trans.Translate(-com);
    trans *= align_trans;
    trans.Translate(com);

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        Coordinates.SetPosition(i,trans.GetTransform(Coordinates.GetPosition(i)));
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodePrincipal(void)
{
    if( Options.GetNumberOfProgArgs() != 5 ) {
        return(false);
    }

    PrincipalMask = Options.GetProgArg(4);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdManip::BondRot(void)
{
    // decode options
    if( DecodeBondRot() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask1;

    mask1.AssignTopology(&Topology);
    mask1.AssignCoordinates(&Coordinates);

    if( mask1.SetMask(AtmMask1) == false ) {
        ES_ERROR("unable to set mask 1");
        return(false);
    }

    if( mask1.GetNumberOfSelectedAtoms() != 1 ){
        ES_ERROR("too many atoms in mask1");
        return(false);
    }

    CPoint p1;
    int    at1 = -1;
    for(int i=0; i < mask1.GetNumberOfTopologyAtoms(); i++){
        if( mask1.IsAtomSelected(i) ){
            p1 = Coordinates.GetPosition(i);
            at1 = i;
            break;
        }
    }

    CAmberMaskAtoms     mask2;

    mask2.AssignTopology(&Topology);
    mask2.AssignCoordinates(&Coordinates);

    if( mask2.SetMask(AtmMask2) == false ) {
        ES_ERROR("unable to set mask 2");
        return(false);
    }

    if( mask2.GetNumberOfSelectedAtoms() != 1 ){
        ES_ERROR("too many atoms in mask2");
        return(false);
    }
    CPoint p2;
    int    at2 = -1;
    for(int i=0; i < mask2.GetNumberOfTopologyAtoms(); i++){
        if( mask2.IsAtomSelected(i) ){
            p2 = Coordinates.GetPosition(i);
            at2 = i;
            break;
        }
    }

    // prepare transformation matrix
    CPoint          direction;
    CTransformation trans;

    direction = p2-p1;

    direction.Normalize();

    trans.Translate(p2*(-1.0));
    trans.Rotate(Angle,direction);
    trans.Translate(p2);

    std::stack<int>     astack;
    std::vector<bool>   processed;
    processed.resize(Topology.AtomList.GetNumberOfAtoms());

    Topology.BuidListOfNeighbourAtoms();

    // generate initial seed
    processed[at1] = true;
    processed[at2] = true;

    astack.push(at2);

    while( ! astack.empty() ){

        int at = astack.top();
        astack.pop();

      //  std::cout << at << std::endl;

        // transform
        Coordinates.SetPosition(at,trans.GetTransform(Coordinates.GetPosition(at)));

        // add neighbours
        CAmberAtom* p_at = Topology.AtomList.GetAtom(at);
        for(int j=0; j < p_at->GetNumberOfNeighbourAtoms(); j++ ){
            int at2 = p_at->GetNeighbourAtomIndex(j);
            if( ! processed[at2] ) astack.push(at2);
            processed[at2] = true;
        }

    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdManip::DecodeBondRot(void)
{
    if( Options.GetNumberOfProgArgs() != 7 ) {
        return(false);
    }

    AtmMask1 = Options.GetProgArg(4);
    AtmMask2 = Options.GetProgArg(5);
    Angle = Options.GetProgArg(6).ToDouble()/180.0*M_PI;

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

