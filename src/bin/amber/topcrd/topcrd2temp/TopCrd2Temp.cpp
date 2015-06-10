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

#include "TopCrd2Temp.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrd2Temp::CTopCrd2Temp(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrd2Temp::Init(int argc,char* argv[])
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
        printf("# topcrd2temp (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        if( Options.GetArgCrdName() != "-" ) {
            printf("# Input coordinates  : %s\n",(const char*)Options.GetArgCrdName());
        } else {
            printf("# Input coordinates  : - (standard input)\n");
        }
        if( Options.GetArgOutName() != "-" ) {
            printf("# Output coordinates : %s\n",(const char*)Options.GetArgOutName());
        } else {
            printf("# Output coordinates : - (standard output)\n");
        }
        printf("# ------------------------------------------------------------------------------\n");
        if( Options.GetOptSHAKE() == true ) {
            printf("# SHAKE              : yes\n");
        } else {
            printf("# SHAKE              : no\n");
        }
        printf("# Max temperature    : %f\n",Options.GetOptMaxTemp());
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Temp::Run(void)
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

    if( Topology.InitResidueNumOfBondsWithHydrogen() == false ) {
        ES_ERROR("InitResidueNumOfBondsWithHydrogen failed");
        return(false);
    }

    if( Topology.InitMoleculeIndexes() == false ) {
        ES_ERROR("InitResidueNumOfBondsWithHydrogen failed");
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

    // write results
    if( Options.GetOptOutputFormat() == "pdb" ) {
        // write PDB
        result = WritePDB(p_fout);
    } else if ( Options.GetOptOutputFormat() == "res" ) {
        // write RES
        result = WriteRES(p_fout);
    } else if ( Options.GetOptOutputFormat() == "reh" ) {
        // write REH
        result = WriteREH(p_fout);
    } else {
        fprintf(stderr,">>> ERROR: Unsupported output format (%s)!\n",(const char*)Options.GetOptOutputFormat());
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

bool CTopCrd2Temp::Finalize(void)
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

//------------------------------------------------------------------------------

bool CTopCrd2Temp::WritePDB(FILE* p_fout)
{

    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123 456789 012345 678901234567890
    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540   1.00  11.92       A1   N

    CSmallTimeAndDate time;
    time.GetActualTimeAndDate();

    // write header
    WritePDBRemark(p_fout,"File generated with TopCrdTemp");
    WritePDBRemark(p_fout,"=== Topology ===");
    WritePDBRemark(p_fout,Options.GetArgTopologyName());
    WritePDBRemark(p_fout,"=== Coordinates ===");
    WritePDBRemark(p_fout,Options.GetArgCrdName());
    WritePDBRemark(p_fout,"=== Date ===");
    WritePDBRemark(p_fout,time.GetSDateAndTime());

    int mol_id = -1;
    int last_mol_id = -1;

    CAmberResidue* p_last_res = NULL;
    CAmberResidue* p_res = NULL;

    double temp = 0.0;

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        if( p_atom == NULL ) {
            ES_ERROR("p_atom is NULL");
            return(false);
        }

        mol_id = p_atom->GetMoleculeIndex();

        if( last_mol_id != mol_id ) {
            if( last_mol_id != -1 ) fprintf(p_fout,"TER\n");
            last_mol_id = mol_id;
        }

        p_res = p_atom->GetResidue();
        if( p_last_res != p_res ) {
            temp = GetResTemp(p_res);
            p_last_res = p_res;
        }

        double scale = (temp - Options.GetOptMinTemp())/ ( Options.GetOptMaxTemp() - Options.GetOptMinTemp());

        if( scale > 1.0 ) scale = 1.0;
        if( scale < 0.0 ) scale = 0.0;

        fprintf(p_fout,"ATOM  %5d %4s %4s%5d    %8.3f%8.3f%8.3f %6.4f\n",
                i+1,(const char*)p_atom->GetPDBName(),p_atom->GetResidue()->GetName(),
                p_atom->GetResidue()->GetIndex()+1,
                Coordinates.GetPosition(i).x,Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z,scale);
    }

    fprintf(p_fout,"TER\n");

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Temp::WriteRES(FILE* p_fout)
{
    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++ ) {
        CAmberResidue* p_res = Topology.ResidueList.GetResidue(i);
        if( p_res == NULL ) {
            ES_ERROR("p_res is NULL");
            return(false);
        }
        double temp = GetResTemp(p_res);

        fprintf(p_fout,"%6d %4s %13.3f\n",i+1,p_res->GetName(),temp);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Temp::WriteREH(FILE* p_fout)
{
    for(int i=0; i < Topology.ResidueList.GetNumberOfResidues(); i++ ) {
        CAmberResidue* p_res = Topology.ResidueList.GetResidue(i);
        if( p_res == NULL ) {
            ES_ERROR("p_res is NULL");
            return(false);
        }
        double temp = GetResTemp(p_res);

        fprintf(p_fout,"%13.3f ",temp);
    }

    fprintf(p_fout,"\n");

    return(true);
}

//------------------------------------------------------------------------------

double CTopCrd2Temp::GetResTemp(CAmberResidue* p_res)
{
    if( p_res == NULL ) {
        ES_ERROR("p_res is NULL");
        return(0.0);
    }

    // calculate kinetic energy
    double ekin = 0.0;
    for(int i=0; i < p_res->GetNumberOfAtoms(); i++) {
        int atmid = i + p_res->GetFirstAtomIndex();
        CAmberAtom* p_atm = Topology.AtomList.GetAtom(atmid);
        if( p_atm == NULL ) {
            ES_ERROR("p_atm is NULL");
            return(0.0);
        }
        ekin += 0.5*p_atm->GetMass()*Square(Coordinates.GetVelocity(atmid));
    }

    // calculate number of freedom
    double ndof = p_res->GetNumberOfAtoms()*3;
    if( Options.GetOptSHAKE() ) {
        ndof -= p_res->GetNumberOfBondsWithHydrogen();
    }

    // ekin is in kcal/mol
    // calculate temperature
    double temp = 2.0 * ekin * 4180 / (ndof*8.314);

    return(temp);
}

//------------------------------------------------------------------------------

bool CTopCrd2Temp::WritePDBRemark(FILE* p_file,const CSmallString& string)
{
    int end = string.GetLength();
    int start = 0;

    while( start < end ) {
        CSmallString substr;
        int len = 73;
        if( end - start < 73 ) len = end - start;
        substr = string.GetSubString(start,len);
        fprintf(p_file,"REMARK %s\n",(const char*)substr);
        start += len;
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

