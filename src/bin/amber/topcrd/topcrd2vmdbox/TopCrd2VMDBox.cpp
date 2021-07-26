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

#include "TopCrd2VMDBox.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrd2VMDBox::CTopCrd2VMDBox(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrd2VMDBox::Init(int argc,char* argv[])
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
        printf("# topcrd2vmdbox (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        if( Options.GetArgCrdName() != "-" ) {
            printf("# Input coordinates  : %s\n",(const char*)Options.GetArgCrdName());
        } else {
            printf("# Input coordinates  : - (standard input)\n");
        }
        if( Options.GetArgOutName() != "-" ) {
            printf("# Output file        : %s\n",(const char*)Options.GetArgOutName());
        } else {
            printf("# Output file        : - (standard output)\n");
        }
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2VMDBox::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    // load coordinates
    Coordinates.AssignTopology(&Topology);
    if( Coordinates.Load(Options.GetArgCrdName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                (const char*)Options.GetArgCrdName());
        return(false);
    }

    // init mask ----------------------------------------------
    bool result = true;

    // open output file
    FILE* p_fout;
    if( Options.GetArgOutName() != "-" ) {
        if( (p_fout = fopen(Options.GetArgOutName(),"w")) == NULL ) {
            fprintf(stderr,">>> ERROR: Unable to open output file (%s)!\n",strerror(errno));
            return(false);
        }
    } else {
        p_fout = stdout;
    }

    // print box
    if( (Topology.BoxInfo.GetType() == AMBER_BOX_OCTAHEDRAL) &&
            (Options.GetOptFamiliar() == true) ) {
        WriteFamiliarTruncatedOctahedralBox(p_fout);
    } else {
        WriteBox(p_fout);
    }

    // close output file if necessary
    if( Options.GetArgOutName() != "-" ) {
        fclose(p_fout);
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2VMDBox::Finalize(void)
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

bool CTopCrd2VMDBox::WriteBox(FILE* p_vmdbox)
{
    fprintf(p_vmdbox,"# box information\n");
    fprintf(p_vmdbox,"set mol_id %d\n",Options.GetOptMolID());

    CPoint p1,p2,p3,p4,p5,p6,p7,p8;

    p1.x = 0.0;
    p1.y = 0.0;
    p1.z = 0.0;

    p2.x = 1.0;
    p2.y = 0.0;
    p2.z = 0.0;

    p3.x = 1.0;
    p3.y = 1.0;
    p3.z = 0.0;

    p4.x = 0.0;
    p4.y = 1.0;
    p4.z = 0.0;

    p5.x = 0.0;
    p5.y = 0.0;
    p5.z = 1.0;

    p6.x = 1.0;
    p6.y = 0.0;
    p6.z = 1.0;

    p7.x = 1.0;
    p7.y = 1.0;
    p7.z = 1.0;

    p8.x = 0.0;
    p8.y = 1.0;
    p8.z = 1.0;

    CTransformation trans;

    GetBoxVectors(Coordinates.GetBox(),Coordinates.GetAngles());

    trans *= BoxTrans;

    if( Options.GetOptOrigin() == true ) {
        trans.Translate(- (AVector + BVector + CVector) * 0.5);
    }

    trans.Translate( AVector * Options.GetOptKX()
                     + BVector * Options.GetOptKY()
                     + CVector * Options.GetOptKZ() );

    trans.Apply(p1);
    trans.Apply(p2);
    trans.Apply(p3);
    trans.Apply(p4);
    trans.Apply(p5);
    trans.Apply(p6);
    trans.Apply(p7);
    trans.Apply(p8);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p1.x,p1.y,p1.z,p2.x,p2.y,p2.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p2.x,p2.y,p2.z,p3.x,p3.y,p3.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p3.x,p3.y,p3.z,p4.x,p4.y,p4.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p4.x,p4.y,p4.z,p1.x,p1.y,p1.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p5.x,p5.y,p5.z,p6.x,p6.y,p6.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p6.x,p6.y,p6.z,p7.x,p7.y,p7.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p7.x,p7.y,p7.z,p8.x,p8.y,p8.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p8.x,p8.y,p8.z,p5.x,p5.y,p5.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p1.x,p1.y,p1.z,p5.x,p5.y,p5.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p2.x,p2.y,p2.z,p6.x,p6.y,p6.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p3.x,p3.y,p3.z,p7.x,p7.y,p7.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p4.x,p4.y,p4.z,p8.x,p8.y,p8.z);

    fprintf(p_vmdbox,"\n");

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2VMDBox::WriteFamiliarTruncatedOctahedralBox(FILE* p_vmdbox)
{
    fprintf(p_vmdbox,"# box information\n");
    fprintf(p_vmdbox,"set mol_id %d\n",Options.GetOptMolID());

    // 24 vertices
    CPoint p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24;

    p1.x  =  1.0;
    p1.y  =  0.0;
    p1.z  =  2.0;

    p2.x  =  0.0;
    p2.y  =  1.0;
    p2.z  =  2.0;

    p3.x  = -1.0;
    p3.y  =  0.0;
    p3.z  =  2.0;

    p4.x  =  0.0;
    p4.y  = -1.0;
    p4.z  =  2.0;

    p5.x  =  1.0;
    p5.y  =  0.0;
    p5.z  = -2.0;

    p6.x  =  0.0;
    p6.y  =  1.0;
    p6.z  = -2.0;

    p7.x  = -1.0;
    p7.y  =  0.0;
    p7.z  = -2.0;

    p8.x  =  0.0;
    p8.y  = -1.0;
    p8.z  = -2.0;

    p9.x  =  1.0;
    p9.y  =  2.0;
    p9.z  =  0.0;

    p10.x =  0.0;
    p10.y =  2.0;
    p10.z =  1.0;

    p11.x = -1.0;
    p11.y =  2.0;
    p11.z =  0.0;

    p12.x =  0.0;
    p12.y =  2.0;
    p12.z = -1.0;

    p13.x =  1.0;
    p13.y = -2.0;
    p13.z =  0.0;

    p14.x =  0.0;
    p14.y = -2.0;
    p14.z =  1.0;

    p15.x = -1.0;
    p15.y = -2.0;
    p15.z =  0.0;

    p16.x =  0.0;
    p16.y = -2.0;
    p16.z = -1.0;

    p17.x =  2.0;
    p17.y =  1.0;
    p17.z =  0.0;

    p18.x =  2.0;
    p18.y =  0.0;
    p18.z =  1.0;

    p19.x =  2.0;
    p19.y = -1.0;
    p19.z =  0.0;

    p20.x =  2.0;
    p20.y =  0.0;
    p20.z = -1.0;

    p21.x = -2.0;
    p21.y =  1.0;
    p21.z =  0.0;

    p22.x = -2.0;
    p22.y =  0.0;
    p22.z =  1.0;

    p23.x = -2.0;
    p23.y = -1.0;
    p23.z =  0.0;

    p24.x = -2.0;
    p24.y =  0.0;
    p24.z = -1.0;

    CTransformation trans;

    // scale box -----------------------------------
    double box_size = Coordinates.GetBox().x;

    double scale = box_size / (2.0 * sqrt(3.0));

    trans.Scale(scale,scale,scale);

    // rotate box ----------------------------------
    CSimpleVector<CPoint> ref;
    CSimpleVector<CPoint> sys;

    ref.CreateVector(3);
    sys.CreateVector(3);

    GetBoxVectors(Coordinates.GetBox(),Coordinates.GetAngles());

    ref[0] = AVector;
    ref[1] = BVector;
    ref[2] = CVector;

    sys[0] = Norm(CPoint(1.0,1.0,1.0)*box_size);
    sys[1] = Norm(CPoint(-1.0,-1.0,1.0)*box_size);
    sys[2] = Norm(CPoint(1.0,-1.0,-1.0)*box_size);

    trans.QuaternionFit(ref,sys);

    if( Options.GetOptOrigin() == false ) {
        trans.Translate( (AVector + BVector + CVector) * 0.5);
    }

    trans.Translate( AVector * Options.GetOptKX()
                     + BVector * Options.GetOptKY()
                     + CVector * Options.GetOptKZ() );

    trans.Apply(p1);
    trans.Apply(p2);
    trans.Apply(p3);
    trans.Apply(p4);
    trans.Apply(p5);
    trans.Apply(p6);
    trans.Apply(p7);
    trans.Apply(p8);
    trans.Apply(p9);
    trans.Apply(p10);
    trans.Apply(p11);
    trans.Apply(p12);
    trans.Apply(p13);
    trans.Apply(p14);
    trans.Apply(p15);
    trans.Apply(p16);
    trans.Apply(p17);
    trans.Apply(p18);
    trans.Apply(p19);
    trans.Apply(p20);
    trans.Apply(p21);
    trans.Apply(p22);
    trans.Apply(p23);
    trans.Apply(p24);

    // 36 edges
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p1.x,p1.y,p1.z,p2.x,p2.y,p2.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p2.x,p2.y,p2.z,p3.x,p3.y,p3.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p3.x,p3.y,p3.z,p4.x,p4.y,p4.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p4.x,p4.y,p4.z,p1.x,p1.y,p1.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p5.x,p5.y,p5.z,p6.x,p6.y,p6.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p6.x,p6.y,p6.z,p7.x,p7.y,p7.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p7.x,p7.y,p7.z,p8.x,p8.y,p8.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p8.x,p8.y,p8.z,p5.x,p5.y,p5.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p9.x,p9.y,p9.z,p10.x,p10.y,p10.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p10.x,p10.y,p10.z,p11.x,p11.y,p11.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p11.x,p11.y,p11.z,p12.x,p12.y,p12.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p12.x,p12.y,p12.z,p9.x,p9.y,p9.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p13.x,p13.y,p13.z,p14.x,p14.y,p14.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p14.x,p14.y,p14.z,p15.x,p15.y,p15.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p15.x,p15.y,p15.z,p16.x,p16.y,p16.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p16.x,p16.y,p16.z,p13.x,p13.y,p13.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p17.x,p17.y,p17.z,p18.x,p18.y,p18.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p18.x,p18.y,p18.z,p19.x,p19.y,p19.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p19.x,p19.y,p19.z,p20.x,p20.y,p20.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p20.x,p20.y,p20.z,p17.x,p17.y,p17.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p21.x,p21.y,p21.z,p22.x,p22.y,p22.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p22.x,p22.y,p22.z,p23.x,p23.y,p23.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p23.x,p23.y,p23.z,p24.x,p24.y,p24.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p24.x,p24.y,p24.z,p21.x,p21.y,p21.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p1.x,p1.y,p1.z,p18.x,p18.y,p18.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p2.x,p2.y,p2.z,p10.x,p10.y,p10.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p3.x,p3.y,p3.z,p22.x,p22.y,p22.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p4.x,p4.y,p4.z,p14.x,p14.y,p14.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p5.x,p5.y,p5.z,p20.x,p20.y,p20.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p6.x,p6.y,p6.z,p12.x,p12.y,p12.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p7.x,p7.y,p7.z,p24.x,p24.y,p24.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p8.x,p8.y,p8.z,p16.x,p16.y,p16.z);

    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p9.x,p9.y,p9.z,p17.x,p17.y,p17.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p11.x,p11.y,p11.z,p21.x,p21.y,p21.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p13.x,p13.y,p13.z,p19.x,p19.y,p19.z);
    fprintf(p_vmdbox,"graphics $mol_id line {%f %f %f} {%f %f %f}\n",
            p15.x,p15.y,p15.z,p23.x,p23.y,p23.z);

    return(true);
}

//---------------------------------------------------------------------------

void CTopCrd2VMDBox::GetBoxVectors(const CPoint& sizes,const CPoint& angles)
{
    double cos_a = cos(angles.x*M_PI/180.0);
    double sin_a = sin(angles.x*M_PI/180.0);

    AVector.x = sizes.x;
    AVector.y = 0.0;
    AVector.z = 0.0;

    BVector.x = sizes.y*cos_a;
    BVector.y = sizes.y*sin_a;
    BVector.z = 0.0;

    CVector.x = sizes.y*cos_a;
    CVector.y = (sizes.x*sizes.y*cos_a - CVector.x*BVector.x) / BVector.y;
    CVector.z = sqrt(sizes.z*sizes.z - CVector.x*CVector.x - CVector.y*CVector.y);

    BoxTrans.Field[0][0] = AVector.x;
    BoxTrans.Field[0][1] = AVector.y;
    BoxTrans.Field[0][2] = AVector.z;
    BoxTrans.Field[0][3] = 0.0;

    BoxTrans.Field[1][0] = BVector.x;
    BoxTrans.Field[1][1] = BVector.y;
    BoxTrans.Field[1][2] = BVector.z;
    BoxTrans.Field[1][3] = 0.0;

    BoxTrans.Field[2][0] = CVector.x;
    BoxTrans.Field[2][1] = CVector.y;
    BoxTrans.Field[2][2] = CVector.z;
    BoxTrans.Field[2][3] = 0.0;

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

