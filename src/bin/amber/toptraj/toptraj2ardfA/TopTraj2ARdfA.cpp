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

#include "TopTraj2ARdfA.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopTraj2ARdfA::CTopTraj2ARdfA(void)
{
    TotalNumberOfSnapshots = 0;
    NumberOfSnapshots = 0;
    NumberOfSoluteMols = 0;

    PLN.CreateMatrix(3,3);
    EigV.CreateVector(3);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopTraj2ARdfA::Init(int argc,char* argv[])
{
    // encode program options
    int result = Options.ParseCmdLine(argc,argv);

    // should we exit or was it error?
    if( result != SO_CONTINUE ) return(result);

    // print header --------------------------------------------------------------
    if( Options.GetOptVerbose() ) {
        printf("\n");
        PrintHeader(stdout);
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopTraj2ARdfA::Run(void)
{
    // create histogram
    Histogram.CreateMatrix(Options.GetOptNBins1(),Options.GetOptNBins2());
    Histogram.SetZero();

    // load topology
    if( Topology.Load(Options.GetArgTopName()) == false ) {
        CSmallString error;
        error << "unable to load topology " << Options.GetArgTopName();
        ES_TRACE_ERROR(error);
        return(false);
    }

    CAmberMaskResidues solute_mask;
    solute_mask.AssignTopology(&Topology);
    solute_mask.AssignCoordinates(&Coordinates);

    if( solute_mask.SetMask(Options.GetArgSoluteMask()) == false ) {
        ES_TRACE_ERROR("unable to set solute mask");
        return(false);
    }

    NumberOfSoluteMols = solute_mask.GetNumberOfSelectedResidues();

    CAmberMaskResidues solvent_mask;
    solvent_mask.AssignTopology(&Topology);
    solvent_mask.AssignCoordinates(&Coordinates);

    if( solvent_mask.SetMask(Options.GetArgSolventMask()) == false ) {
        ES_TRACE_ERROR("unable to set solute mask");
        return(false);
    }

    // open trajectory
    Coordinates.AssignTopology(&Topology);
    Coordinates.Create();
    Trajectory.AssignTopology(&Topology);
    Trajectory.AssignRestart(&Coordinates);
    if( Trajectory.OpenTrajectoryFile(Options.GetArgTrajName(),AMBER_TRAJ_UNKNOWN,AMBER_TRAJ_CXYZB,AMBER_TRAJ_READ) == false ) {
        CSmallString error;
        error << "unable to open trajectory " << Options.GetArgTrajName();
        ES_ERROR(error);
        return(false);
    }

    // do analysis
    while( Trajectory.ReadSnapshot() == true ) {
        TotalNumberOfSnapshots++;
        if( TotalNumberOfSnapshots % Options.GetOptFrequency() != 0 ) continue;

        if( Options.GetOptVerbose() ){
            fprintf(stdout,"Snapshot number: % 6d\r",TotalNumberOfSnapshots);
            fflush(stdout);
        }
        NumberOfSnapshots++;

        // update box
        Topology.BoxInfo.SetBoxDimmensions(Coordinates.GetBox());
        Topology.BoxInfo.SetBoxAngles(Coordinates.GetAngles());
        Topology.BoxInfo.UpdateBoxMatrices();

        // solute x solvent
        for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
            if( solute_mask.GetSelectedResidue(i) == NULL ) continue;
            for(int j=0; j < Topology.ResidueList.GetNumberOfResidues(); j++) {
                if( solvent_mask.GetSelectedResidue(j) == NULL ) continue;
                if( Calculate(i,j) == false ) return(false);
            }
        }
    }
    printf("\n");

    if( NumberOfSnapshots <= 0 ){
        CSmallString error;
        error << "no snapshots";
        ES_ERROR(error);
        return(false);
    }

    if( NumberOfSoluteMols <= 0 ){
        CSmallString error;
        error << "no solute/solvent molecule";
        ES_ERROR(error);
        return(false);
    }

    // write histogram
    FILE* p_fout = fopen(Options.GetArgOutputName(),"w");
    if( p_fout == NULL ){
        CSmallString error;
        error << "unable to open output file " << Options.GetArgOutputName();
        ES_ERROR(error);
        return(false);
    }

    PrintHeader(p_fout);

    double dch1 = (Options.GetOptStopValue1() - Options.GetOptStartValue1())/Options.GetOptNBins1();
    double dch2 = (Options.GetOptStopValue2() - Options.GetOptStartValue2())/Options.GetOptNBins2();

    for(int i=0; i < Options.GetOptNBins1(); i++) {
        for(int j=0; j < Options.GetOptNBins2(); j++) {
            double x = i*dch1 + dch1*0.5;
            double y = j*dch2 + dch2*0.5;
            double z = Histogram[i][j];
            if( Options.GetOptHistogram() ){
                z = z / ((double) (NumberOfSnapshots * NumberOfSoluteMols));
                fprintf(p_fout,"%10.2f %10.2f %16.8f\n",x,y,z);
            } else {
                z = z / (4.0*M_PI*x*x*sin(y)*dch1*dch2*Options.GetOptDensity());
                z = z / ((double) (NumberOfSnapshots * NumberOfSoluteMols));
                fprintf(p_fout,"%10.2f %10.2f %16.8f\n",x,y,z);
            }
        }
        fprintf(p_fout,"\n");
    }

    fclose(p_fout);

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopTraj2ARdfA::Finalize(void)
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

bool CTopTraj2ARdfA::Calculate(int residA,int residB)
{
    if( residA == residB ) return(true);

    CAmberResidue* p_res1 = Topology.ResidueList.GetResidue(residA);
    CAmberResidue* p_res2 = Topology.ResidueList.GetResidue(residB);

    if( p_res2->GetNumberOfAtoms() <= 3 ){
        CSmallString error;
        error << "solvent residue " << residB+1 << " does not contain three or more atoms";
        ES_ERROR(error);
        return(false);
    }

    // calculate COMs -----------------------------------
    CPoint  com1;
    double  tmass = 0.0;

    for(int i=0; i < p_res1->GetNumberOfAtoms(); i++) {
        int atid = p_res1->GetFirstAtomIndex() + i;
        double mass = Topology.AtomList.GetAtom(atid)->GetMass();
        com1 += Coordinates.GetPosition(atid)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com1 /= tmass;

    CPoint  com2;
    tmass = 0.0;

    for(int i=0; i < p_res2->GetNumberOfAtoms(); i++) {
        int atid = p_res2->GetFirstAtomIndex() + i;
        double mass = Topology.AtomList.GetAtom(atid)->GetMass();
        com2 += Coordinates.GetPosition(atid)*mass;
        tmass += mass;
    }
    if( tmass != 0.0 ) com2 /= tmass;

    CPoint dvec;
    dvec = Topology.BoxInfo.ImageVector(com2 - com1);

    double distance = Size(dvec);
    if( distance > Options.GetOptStopValue1() ) return(true);


    // calculate plane normal --------------------
    PLN.SetZero();

    //! construct matrix:
    for(int i=0; i < p_res2->GetNumberOfAtoms(); i++) {
        int atid = p_res2->GetFirstAtomIndex() + i;
        double mass = Topology.AtomList.GetAtom(atid)->GetMass();
        CPoint pos = Coordinates.GetPosition(atid);

        PLN[0][0] += mass*(pos.x - com2.x)*(pos.x - com2.x);
        PLN[1][1] += mass*(pos.y - com2.y)*(pos.y - com2.y);
        PLN[2][2] += mass*(pos.z - com2.z)*(pos.z - com2.z);

        PLN[0][1] += mass*(pos.x - com2.x)*(pos.y - com2.y);
        PLN[0][2] += mass*(pos.x - com2.x)*(pos.z - com2.z);
        PLN[1][2] += mass*(pos.y - com2.y)*(pos.z - com2.z);
    }

    PLN[1][0] = PLN[0][1];
    PLN[2][0] = PLN[0][2];
    PLN[2][1] = PLN[1][2];

    //! calculate eignevalues and eigenvectors of matrix
    EigV.SetZero();

    //! now solve eigenproblem
    int info = CLapack::syev('V','L',PLN,EigV);

    if( info != 0 ) {
        ES_ERROR("unable to diagonalize matrix");
        return(false);
    }

    // best plane normal vector
    CPoint ivec2;
    ivec2.x = PLN[0][0];
    ivec2.y = PLN[1][0];
    ivec2.z = PLN[2][0];

    // calculate angle ---------------------------
    double angle = Angle(ivec2,dvec);
    if( angle > M_PI/2.0 ){
        angle = M_PI - angle;   // remove axis flipping
    }

    // find histogram channels
    double dch1 = (Options.GetOptStopValue1() - Options.GetOptStartValue1())/Options.GetOptNBins1();
    double dch2 = (Options.GetOptStopValue2() - Options.GetOptStartValue2())/Options.GetOptNBins2();

    int index1 = (int)((distance - Options.GetOptStartValue1())/dch1);
    if( ! ((index1 >= 0) && (index1 < Options.GetOptNBins1())) ) {
        if( distance == Options.GetOptStopValue1() ) {
            index1 = Options.GetOptNBins1()-1;
        } else {
            index1 = -1;
        }
    }

    int index2 = (int)((angle - Options.GetOptStartValue2())/dch2);
    if( ! ((index2 >= 0) && (index2 < Options.GetOptNBins2())) ) {
        if( angle == Options.GetOptStopValue2() ) {
            index2 = Options.GetOptNBins2()-1;
        } else {
            index2 = -1;
        }
    }

    if( (index1 > -1 ) && (index2 > -1) ){
        Histogram[index1][index2]++;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopTraj2ARdfA::PrintHeader(FILE* p_fout)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    fprintf(p_fout,"# ==============================================================================\n");
    fprintf(p_fout,"# toptraj2ardfA (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
    fprintf(p_fout,"# ==============================================================================\n");
    fprintf(p_fout,"#\n");
    fprintf(p_fout,"# Topology name          : %s\n",(const char*)Options.GetArgTopName());
    fprintf(p_fout,"# Input trajectory       : %s\n",(const char*)Options.GetArgTrajName());
    fprintf(p_fout,"# Output histogram       : %s\n",(const char*)Options.GetArgOutputName());
    fprintf(p_fout,"# Solute mask            : %s\n",(const char*)Options.GetArgSoluteMask());
    fprintf(p_fout,"# Solvent mask           : %s\n",(const char*)Options.GetArgSolventMask());
    fprintf(p_fout,"# Snapshot frequency     : %d\n",Options.GetOptFrequency());
    fprintf(p_fout,"# ------------------------------------------------------------------------------\n");
    fprintf(p_fout,"# Distance start         : %f\n",Options.GetOptStartValue1());
    fprintf(p_fout,"# Distance stop          : %f\n",Options.GetOptStopValue1());
    fprintf(p_fout,"# Distance nbins         : %d\n",Options.GetOptNBins1());
    fprintf(p_fout,"# Angle start            : %f\n",Options.GetOptStartValue2());
    fprintf(p_fout,"# Angle stop             : %f\n",Options.GetOptStopValue2());
    fprintf(p_fout,"# Angle nbins            : %d\n",Options.GetOptNBins2());
    fprintf(p_fout,"# Solvent number density : %f\n",Options.GetOptDensity());
    fprintf(p_fout,"# Histogram only         : %s\n",bool_to_str(Options.GetOptHistogram()));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

