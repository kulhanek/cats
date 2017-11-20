// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2015 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <SmallTimeAndDate.hpp>
#include <errno.h>
#include "TopManip.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopManip::CTopManip(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopManip::Init(int argc,char* argv[])
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
        printf("# topcharges (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgTopologyName() == "-" ) {
            printf("# Topology name  : -stdin- (standard input)\n");
        } else {
            printf("# Topology name  : %s\n",(const char*)Options.GetArgTopologyName());
        }
        if( Options.GetArgOutputName() == "-" ) {
            printf("# Output name    : -stdout- (standard output)\n");
        } else {
            printf("# Output name    : %s\n",(const char*)Options.GetArgOutputName());
        }
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopManip::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        CSmallString error;
        error << "unable to load specified topology: " << Options.GetArgTopologyName();
        ES_ERROR(error);
        return(false);
    }

    if( Options.GetOptVerbose() ) Topology.PrintInfo();

    // assign topology and coordinates to the mask -------------
    bool result = true;
    Mask.AssignTopology(&Topology);

    // init mask ----------------------------------------------
    // by default all atoms are selected
    Mask.SelectAllAtoms();

    // do we have user specified mask?
    if( Options.IsOptMaskSpecSet() == true ) {
        result = Mask.SetMask(Options.GetOptMaskSpec());
    }

    // do we have user specified mask?
    if( Options.IsOptMaskFileSet() == true ) {
        result = Mask.SetMaskFromFile(Options.GetOptMaskFile());
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified mask!\n");
        return(false);
    }

    // open output file
    FILE* p_fout;
    if( Options.GetArgOutputName() != "-" ) {
        if( (p_fout = fopen(Options.GetArgOutputName(),"w")) == NULL ) {
            CSmallString error;
            error << "unable to open output file " << Options.GetArgOutputName();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fout = stdout;
    }

    // execute
    bool recognized = true;
    bool save_topology = false;
    if( Options.IsOptGetChargesSet() ){
        result = PrintCharges(p_fout);
    } else if ( Options.IsOptSetChargesSet() ) {
        result = SetCharges();
        save_topology =  result;
    } else {
        recognized = false;
    }

    if( save_topology ){
        if( Topology.Save(p_fout,AMBER_VERSION_NONE) == false ) {
            fprintf(stderr,"\n");
            fprintf(stderr,">>> ERROR: Unable to save modified topology: %s\n",
                    (const char*)Options.GetArgOutputName());
            return(false);
        }
    }

    // close output file if necessary
    if( Options.GetArgOutputName() != "-" ) {
        fclose(p_fout);
    }

    if( recognized == false ) {
        // message for user
        CSmallString error;
        error << "not implemented action or no action specified";
        ES_ERROR(error);
        return(false);
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CTopManip::PrintCharges(FILE* p_fout)
{
    fprintf(p_fout,"#  ID   Name  ResID  Res   Charge \n");
    fprintf(p_fout,"# ----- ---- ------- ---- --------\n");

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        fprintf(p_fout,"%7d %4s %7d %4s %8.5f\n",
                i+1,p_atom->GetName(),p_atom->GetResidue()->GetIndex()+1,p_atom->GetResidue()->GetName(),
                p_atom->GetStandardCharge());
    }
    return(true);
}

//------------------------------------------------------------------------------

bool CTopManip::SetCharges(void)
{
    FILE* p_qin;
    if( (p_qin = fopen(Options.GetOptSetCharges(),"r")) == NULL ) {
        CSmallString error;
        error << "unable to open input file " << Options.GetOptSetCharges();
        error << " (" << strerror(errno) << ")";
        ES_ERROR(error);
        return(false);
    }

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        double ch = 0.0;
        if( fscanf(p_qin,"%lf",&ch) != 1 ){
            CSmallString error;
            error << "unable to read input charge from " << Options.GetOptSetCharges();
            error << " (" << strerror(errno) << ")";
            fclose(p_qin);
            ES_ERROR(error);
            return(false);
        }
        p_atom->SetStandardCharge(ch);
    }

    fclose(p_qin);
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopManip::Finalize(void)
{
    if( Options.GetOptVerbose() ) {
        CSmallTimeAndDate dt;
        dt.GetActualTimeAndDate();

        fprintf(stdout,"\n");
        fprintf(stdout,"# ==============================================================================\n");
        fprintf(stdout,"# topcharges terminated at %s\n",(const char*)dt.GetSDateAndTime());
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

