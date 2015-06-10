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
#include <FortranMatrix.hpp>
#include <Lapack.hpp>

#include "TopTraj2Rdf.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopTraj2Rdf::CTopTraj2Rdf(void)
{
    TotalNumberOfSnapshots = 0;
    NumberOfSnapshots = 0;
    NumberOfSoluteMols = 0;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopTraj2Rdf::Init(int argc,char* argv[])
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

bool CTopTraj2Rdf::Run(void)
{
    // create histogram
    Histogram.CreateVector(Options.GetOptNBins());
    Histogram.SetZero();

    // load topology
    if( Topology.Load(Options.GetArgTopName()) == false ) {
        CSmallString error;
        error << "unable to load topology " << Options.GetArgTopName();
        ES_TRACE_ERROR(error);
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

    if( Options.GetOptResidues() ){
        if( RunResidues() == false ) return(false);
    } else {
        if( RunAtoms() == false ) return(false);
    }

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

    double dch = (Options.GetOptStopValue() - Options.GetOptStartValue())/Options.GetOptNBins();

    for(int i=0; i < Options.GetOptNBins(); i++) {
        double x = i*dch + dch*0.5;
        double y = Histogram[i];
        if( Options.GetOptHistogram() ){
            y = y / ((double) (NumberOfSnapshots * NumberOfSoluteMols));
            fprintf(p_fout,"%10.2f %16.8f\n",x,y);
        } else {
            y = y /(4.0*M_PI*x*x*dch*Options.GetOptDensity());
            y = y / ((double) (NumberOfSnapshots * NumberOfSoluteMols));
            fprintf(p_fout,"%10.2f %16.8f\n",x,y);
        }
    }
    fprintf(p_fout,"\n");
    fclose(p_fout);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopTraj2Rdf::RunAtoms(void)
{
    CAmberMaskAtoms solute_mask;
    solute_mask.AssignTopology(&Topology);
    solute_mask.AssignCoordinates(&Coordinates);

    if( solute_mask.SetMask(Options.GetArgSoluteMask()) == false ) {
        ES_TRACE_ERROR("unable to set solute mask");
        return(false);
    }
    NumberOfSoluteMols = solute_mask.GetNumberOfSelectedAtoms();

    CAmberMaskAtoms solvent_mask;
    solvent_mask.AssignTopology(&Topology);
    solvent_mask.AssignCoordinates(&Coordinates);

    if( solvent_mask.SetMask(Options.GetArgSolventMask()) == false ) {
        ES_TRACE_ERROR("unable to set solute mask");
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

        for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
            if( solute_mask.GetSelectedAtom(i) == NULL ) continue;
            for(int j=0; j < Topology.AtomList.GetNumberOfAtoms(); j++) {
                if( solvent_mask.GetSelectedAtom(j) == NULL ) continue;
                if( CalculateAtoms(i,j) == false ) return(false);
            }
        }
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopTraj2Rdf::RunResidues(void)
{
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

        for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++) {
            if( solute_mask.GetSelectedResidue(i) == NULL ) continue;
            for(int j=0; j < Topology.ResidueList.GetNumberOfResidues(); j++) {
                if( solvent_mask.GetSelectedResidue(j) == NULL ) continue;
                if( CalculateResidues(i,j) == false ) return(false);
            }
        }

    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopTraj2Rdf::Finalize(void)
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

bool CTopTraj2Rdf::CalculateAtoms(int atomA,int atomB)
{
    if( atomA == atomB ) return(true);

    if( Options.GetOptExcludeWithinResidues() ){
        if( Topology.AtomList.GetAtom(atomA)->GetResidue() == Topology.AtomList.GetAtom(atomB)->GetResidue() ){
            return(true);
        }
    }

    CPoint dvec;
    dvec = Topology.BoxInfo.ImageVector(Coordinates.GetPosition(atomA) - Coordinates.GetPosition(atomB));

    double distance = Size(dvec);
    if( distance > Options.GetOptStopValue() ) return(true);

    // find histogram channels

    double dch = (Options.GetOptStopValue() - Options.GetOptStartValue())/Options.GetOptNBins();

    int index = (int)((distance - Options.GetOptStartValue())/dch);
    if( ! ((index >= 0) && (index < Options.GetOptNBins())) ) {
        if( distance == Options.GetOptStopValue() ) {
            index = Options.GetOptNBins()-1;
        } else {
            index = -1;
        }
    }

    if( index > -1 ){
        Histogram[index]++;
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopTraj2Rdf::CalculateResidues(int residA,int residB)
{
    if( residA == residB ) return(true);

    CAmberResidue* p_res1 = Topology.ResidueList.GetResidue(residA);
    CAmberResidue* p_res2 = Topology.ResidueList.GetResidue(residB);

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
    if( distance > Options.GetOptStopValue() ) return(true);

    // find histogram channels

    double dch = (Options.GetOptStopValue() - Options.GetOptStartValue())/Options.GetOptNBins();

    int index = (int)((distance - Options.GetOptStartValue())/dch);
    if( ! ((index >= 0) && (index < Options.GetOptNBins())) ) {
        if( distance == Options.GetOptStopValue() ) {
            index = Options.GetOptNBins()-1;
        } else {
            index = -1;
        }
    }

    if( index > -1 ){
        Histogram[index]++;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CTopTraj2Rdf::PrintHeader(FILE* p_fout)
{
    CSmallTimeAndDate dt;
    dt.GetActualTimeAndDate();

    fprintf(p_fout,"# ==============================================================================\n");
    fprintf(p_fout,"# toptraj2rdf (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
    fprintf(p_fout,"# ==============================================================================\n");
    fprintf(p_fout,"#\n");
    fprintf(p_fout,"# Topology name           : %s\n",(const char*)Options.GetArgTopName());
    fprintf(p_fout,"# Input trajectory        : %s\n",(const char*)Options.GetArgTrajName());
    fprintf(p_fout,"# Output histogram        : %s\n",(const char*)Options.GetArgOutputName());
    fprintf(p_fout,"# Solute mask             : %s\n",(const char*)Options.GetArgSoluteMask());
    fprintf(p_fout,"# Solvent mask            : %s\n",(const char*)Options.GetArgSolventMask());
    fprintf(p_fout,"# Residue based masks     : %s\n",bool_to_str(Options.GetOptResidues()));
    fprintf(p_fout,"# Exclude within residues : %s\n",bool_to_str(Options.GetOptExcludeWithinResidues()));
    fprintf(p_fout,"# Snapshot frequency      : %d\n",Options.GetOptFrequency());
    fprintf(p_fout,"# ------------------------------------------------------------------------------\n");
    fprintf(p_fout,"# Distance start          : %f\n",Options.GetOptStartValue());
    fprintf(p_fout,"# Distance stop           : %f\n",Options.GetOptStopValue());
    fprintf(p_fout,"# Distance nbins          : %d\n",Options.GetOptNBins());
    fprintf(p_fout,"# Solvent number density  : %f\n",Options.GetOptDensity());
    fprintf(p_fout,"# Histogram only          : %s\n",bool_to_str(Options.GetOptHistogram()));

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

