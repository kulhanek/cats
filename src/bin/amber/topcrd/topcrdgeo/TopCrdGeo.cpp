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
#include <Lapack.hpp>

#include "TopCrdGeo.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrdGeo::CTopCrdGeo(void)
{
    VectorNumber = 1;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrdGeo::Init(int argc,char* argv[])
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
        printf("# topcrdgeo (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        if( Options.GetProgArg(1) != "-" ) {
            printf("# Input coordinates  : %s\n",(const char*)Options.GetArgCrdName());
        } else {
            printf("# Input coordinates  : - (standard input)\n");
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

bool CTopCrdGeo::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    // load coordinates
    Coordinates.AssignTopology(&Topology);
    if( Coordinates.Load(Options.GetArgCrdName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                (const char*)Options.GetArgCrdName());
        return(false);
    }

    // update box
    Topology.BoxInfo.SetBoxDimmensions(Coordinates.GetBox());
    Topology.BoxInfo.SetBoxAngles(Coordinates.GetAngles());
    Topology.BoxInfo.UpdateBoxMatrices();

    // execute command
    bool result = true;
    if( Options.GetArgCommand() == "com" ) {
        result &= GetCOM();
    }

    else if( Options.GetArgCommand() == "distance" ) {
        result &= GetDistance();
    }

    else if( Options.GetArgCommand() == "toivec" ) {
        result &= GetTOIVector();
    }

    else if( Options.GetArgCommand() == "toimag" ) {
        result &= GetTOIMagnitudes();
    }

    else {
        fprintf(stderr,">>> ERROR: Command %s is not supported!\n",
                (const char*)Options.GetArgCommand());
        result = false;
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to process command: %s\n",
                (const char*)Options.GetArgCommand());
        ES_ERROR("some error occured during geometry measurrement");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdGeo::Finalize(void)
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

bool CTopCrdGeo::GetCOM(void)
{
    // decode options
    if( DecodeGetCOM() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask;

    mask.AssignTopology(&Topology);
    mask.AssignCoordinates(&Coordinates);

    if( mask.SetMask(Mask1) == false ) {
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

    printf("%f %f %f",com.x,com.y,com.z);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdGeo::DecodeGetCOM(void)
{
    if( Options.GetNumOfCommandArgs() != 1 ) {
        return(false);
    }

    Mask1 = Options.GetCommandArg(0);
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdGeo::GetDistance(void)
{
    // decode options
    if( DecodeGetDistance() == false ) {
        ES_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask1,mask2;

    mask1.AssignTopology(&Topology);
    mask1.AssignCoordinates(&Coordinates);

    if( mask1.SetMask(Mask1) == false ) {
        ES_ERROR("unable to set mask1");
        return(false);
    }

    mask2.AssignTopology(&Topology);
    mask2.AssignCoordinates(&Coordinates);

    if( mask2.SetMask(Mask2) == false ) {
        ES_ERROR("unable to set mask2");
        return(false);
    }

    // calculate COM1
    CPoint  com1;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com1 += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com1 /= tmass;

    // calculate COM2
    CPoint  com2;
    tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask2.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com2 += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com2 /= tmass;

    double distance = Size(com2-com1);

    printf("%f",distance);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdGeo::DecodeGetDistance(void)
{
    if( Options.GetNumOfCommandArgs() != 2 ) {
        return(false);
    }

    Mask1 = Options.GetCommandArg(0);
    Mask2 = Options.GetCommandArg(1);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdGeo::GetTOIVector(void)
{
    // decode options
    if( DecodeGetTOIVector() == false ) {
        ES_TRACE_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask1;

    mask1.AssignTopology(&Topology);
    mask1.AssignCoordinates(&Coordinates);

    if( mask1.SetMask(Mask1) == false ) {
        ES_TRACE_ERROR("unable to set mask1");
        return(false);
    }

    // calculate COM1
    CPoint  com1;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com1 += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com1 /= tmass;

    // calculate tensor or inertia
    CFortranMatrix toi;

    toi.CreateMatrix(3,3);
    toi.SetZero();

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        CPoint pos = Coordinates.GetPosition(i);
        toi[0][0] += mass*(pos.y-com1.y)*(pos.y-com1.y) + mass*(pos.z-com1.z)*(pos.z-com1.z);
        toi[1][1] += mass*(pos.x-com1.x)*(pos.x-com1.x) + mass*(pos.z-com1.z)*(pos.z-com1.z);
        toi[2][2] += mass*(pos.x-com1.x)*(pos.x-com1.x) + mass*(pos.y-com1.y)*(pos.y-com1.y);
        toi[0][1] -= mass*(pos.x-com1.x)*(pos.y-com1.y);
        toi[0][2] -= mass*(pos.x-com1.x)*(pos.z-com1.z);
        toi[1][2] -= mass*(pos.y-com1.y)*(pos.z-com1.z);
    }

    toi[1][0] = toi[0][1];
    toi[2][0] = toi[0][2];
    toi[2][1] = toi[1][2];

    //! calculate eignevalues and eigenvectors of matrix
    CVector         v;
    v.CreateVector(3);
    v.SetZero();

    //! now solve eigenproblem
    int info = CLapack::syev('V','L',toi,v);

    if( info != 0 ) {
        fprintf(stderr,">>> ERROR: Unable to diagonalize matrix in CTopCrdGeo::GetPrincipal!\n");
        return(false);
    }

    if( VectorNumber == 0 ) {
        for(int i=0; i < 3; i++) {
            CPoint v1;
            v1.x = toi[0][i];
            v1.y = toi[1][i];
            v1.z = toi[2][i];

            printf("%f %f %f ",v1.x,v1.y,v1.z);
        }
    } else {

        CPoint v1;
        v1.x = toi[0][VectorNumber-1];
        v1.y = toi[1][VectorNumber-1];
        v1.z = toi[2][VectorNumber-1];

        printf("%f %f %f",v1.x,v1.y,v1.z);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdGeo::DecodeGetTOIVector(void)
{
    if( Options.GetNumOfCommandArgs() != 2 ) {
        ES_ERROR("incorrect number of arguments for toivec action");
        return(false);
    }

    Mask1 = Options.GetCommandArg(0);

    if( Options.GetCommandArg(1) == "all" ) {
        VectorNumber = 0;
        return(true);
    }

    if( Options.GetCommandArg(1).ToInt(VectorNumber) == false ) {
        ES_ERROR("vector index is not a number");
        return(false);
    }

    if( (VectorNumber != 1)&&(VectorNumber != 2)&&(VectorNumber != 3) ) {
        ES_ERROR("vector index is not 1, 2, or 3");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrdGeo::GetTOIMagnitudes(void)
{
    // decode options
    if( DecodeTOIMagnitudes() == false ) {
        ES_TRACE_ERROR("unable to decode options");
        return(false);
    }

    CAmberMaskAtoms     mask1;

    mask1.AssignTopology(&Topology);
    mask1.AssignCoordinates(&Coordinates);

    if( mask1.SetMask(Mask1) == false ) {
        ES_TRACE_ERROR("unable to set mask1");
        return(false);
    }

    // calculate COM1
    CPoint  com1;
    double  tmass = 0.0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        com1 += Coordinates.GetPosition(i)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com1 /= tmass;

    // calculate tensor or inertia
    CFortranMatrix toi;

    toi.CreateMatrix(3,3);
    toi.SetZero();

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( mask1.GetSelectedAtom(i) == NULL ) continue;
        double mass = Topology.AtomList.GetAtom(i)->GetMass();
        CPoint pos = Coordinates.GetPosition(i);
        toi[0][0] += mass*(pos.y-com1.y)*(pos.y-com1.y) + mass*(pos.z-com1.z)*(pos.z-com1.z);
        toi[1][1] += mass*(pos.x-com1.x)*(pos.x-com1.x) + mass*(pos.z-com1.z)*(pos.z-com1.z);
        toi[2][2] += mass*(pos.x-com1.x)*(pos.x-com1.x) + mass*(pos.y-com1.y)*(pos.y-com1.y);
        toi[0][1] -= mass*(pos.x-com1.x)*(pos.y-com1.y);
        toi[0][2] -= mass*(pos.x-com1.x)*(pos.z-com1.z);
        toi[1][2] -= mass*(pos.y-com1.y)*(pos.z-com1.z);
    }

    toi[1][0] = toi[0][1];
    toi[2][0] = toi[0][2];
    toi[2][1] = toi[1][2];

    //! calculate eignevalues and eigenvectors of matrix
    CVector         v;

    v.CreateVector(3);
    v.SetZero();

    //! now solve eigenproblem
    int info = CLapack::syev('V','L',toi,v);

    if( info != 0 ) {
        fprintf(stderr,">>> ERROR: Unable to diagonalize matrix in CTopCrdGeo::GetPrincipal!\n");
        return(false);
    }
    if( VectorNumber == 0 ) {
        for(int i=0; i < 3; i++) {
            printf("%f ",v[i]);
        }
    } else {
        printf("%f",v[VectorNumber-1]);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrdGeo::DecodeTOIMagnitudes(void)
{
    if( Options.GetNumOfCommandArgs() != 2 ) {
        ES_ERROR("incorrect number of arguments for toivec action");
        return(false);
    }

    Mask1 = Options.GetCommandArg(0);

    if( Options.GetCommandArg(1) == "all" ) {
        VectorNumber = 0;
        return(true);
    }

    if( Options.GetCommandArg(1).ToInt(VectorNumber) == false ) {
        ES_ERROR("vector index is not a number");
        return(false);
    }

    if( (VectorNumber != 1)&&(VectorNumber != 2)&&(VectorNumber != 3) ) {
        ES_ERROR("vector index is not 1, 2, or 3");
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

