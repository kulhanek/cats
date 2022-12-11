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
#include <XYZStructure.hpp>

#include "TopCrd2Crd.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrd2Crd::CTopCrd2Crd(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrd2Crd::Init(int argc,char* argv[])
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
        printf("# topcrd2crd (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
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
        printf("# Input format       : %s\n",(const char*)Options.GetOptInputFormat());
        printf("# Output format      : %s\n",(const char*)Options.GetOptOutputFormat());
        if( Options.IsOptMaskSpecSet() == true ) {
        printf("# Mask specification : %s\n",(const char*)Options.GetOptMaskSpec());
        }
        if( Options.IsOptMaskFileSet() == true ) {
        printf("# Mask file name     : %s\n",(const char*)Options.GetOptMaskFile());
        }
        if( (Options.IsOptMaskSpecSet() != true) && (Options.IsOptMaskFileSet() != true) ) {
        printf("# Mask specification : all atoms\n");
        }
        if( Options.IsOptMaskSpecSet() == true ) {
        printf("# Charge mask spec.  : %s\n",(const char*)Options.GetOptChrgMaskSpec());
        }
        if( Options.IsOptMaskFileSet() == true ) {
        printf("# Charge mask file   : %s\n",(const char*)Options.GetOptChrgMaskFile());
        }
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Crd::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        CSmallString error;
        error << "unable to load specified topology: " << Options.GetArgTopologyName();
        ES_ERROR(error);
        return(false);
    }

    // open input file
    FILE* p_fin = NULL;
    if( Options.GetArgCrdName() != "-" ) {
        if( (p_fin = fopen(Options.GetArgCrdName(),"r")) == NULL ) {
            CSmallString error;
            error << "unable to open input file " << Options.GetArgCrdName();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fin = stdin;
    }

    // load coordinates
    Coordinates.AssignTopology(&Topology);

    bool recognized = false;
    bool result = false;
    if( Options.GetOptInputFormat() == "crd" ) {
        recognized = true;
        if( p_fin == stdin ) {
            result = ReadCRD(p_fin);
        } else {
            fclose(p_fin);
            p_fin = NULL;
            result = ReadCRD(Options.GetArgCrdName()); // NetCDF requires to use a file directly
        }
    }
    if( Options.GetOptInputFormat() == "xyz" ) {
        recognized = true;
        result = ReadXYZ(p_fin);
    }

    if( p_fin != NULL ){
        fclose(p_fin);
        p_fin = NULL;
    }

    if( recognized == false ) {
        // format is not supported - here only silent error
        // this should be handled in CTopCrd2CrdOptions::CheckOptions with error
        // message for user
        CSmallString error;
        error << "not implemented input format " << Options.GetOptInputFormat();
        ES_ERROR(error);
        return(false);
    }

    if( result == false ) {
        CSmallString error;
        error << "an error occured in reading of input coordinates";
        ES_ERROR(error);
        return(false);
    }

    // assign topology and coordinates to the mask -------------
    Mask.AssignTopology(&Topology);
    Mask.AssignCoordinates(&Coordinates);

    // init mask ----------------------------------------------
    result = true;

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

    // assign topology and coordinates to the mask -------------
    ChrgMask.AssignTopology(&Topology);
    ChrgMask.AssignCoordinates(&Coordinates);

    // init charge mask ----------------------------------------------
    // do we have user specified mask?
    if( Options.IsOptChrgMaskSpecSet() == true ) {
        result = ChrgMask.SetMask(Options.GetOptChrgMaskSpec());
    }

    // do we have user specified mask?
    if( Options.IsOptChrgMaskFileSet() == true ) {
        result = ChrgMask.SetMaskFromFile(Options.GetOptChrgMaskFile());
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified charge mask!\n");
        return(false);
    }

    // open output file
    FILE* p_fout;
    if( Options.GetArgOutName() != "-" ) {
        if( (p_fout = fopen(Options.GetArgOutName(),"w")) == NULL ) {
            CSmallString error;
            error << "unable to open output file " << Options.GetArgOutName();
            error << " (" << strerror(errno) << ")";
            ES_ERROR(error);
            return(false);
        }
    } else {
        p_fout = stdout;
    }

    // convert coordinates
    recognized = false;
    if( Options.GetOptOutputFormat() == "crd" ) {
        recognized = true;
        result = WriteCRD(p_fout,false);
    }
    if( Options.GetOptOutputFormat() == "bcrd" ) {
        // close the output file we need to open it as a netcdf file
        if( Options.GetArgOutName() != "-" ) fclose(p_fout);
        p_fout = NULL;
        recognized = true;
        result = WriteCRD(p_fout,true);
    }
    if( Options.GetOptOutputFormat() == "pdb" ) {
        recognized = true;
        result = WritePDB(p_fout);
    }
    if( Options.GetOptOutputFormat() == "pqr" ) {
        recognized = true;
        result = WritePQR(p_fout);
    }
    if( Options.GetOptOutputFormat() == "xyz" ) {
        recognized = true;
        result = WriteXYZ(p_fout);
    }
    if( Options.GetOptOutputFormat() == "xyzr" ) {
        recognized = true;
        result = WriteXYZR(p_fout);
    }
    if( Options.GetOptOutputFormat() == "com" ) {
        recognized = true;
        result = WriteCOM(p_fout);
    }
    if( Options.GetOptOutputFormat() == "orca" ) {
        recognized = true;
        result = WriteORCA(p_fout);
    }
    if( Options.GetOptOutputFormat() == "pc" ) {
        recognized = true;
        result = WritePC(p_fout);
    }
    if( Options.GetOptOutputFormat() == "rbox" ) {
        recognized = true;
        result = WriteRBOX(p_fout);
    }
    if( Options.GetOptOutputFormat() == "traj" ) {
        recognized = true;
        result = WriteTRAJ(p_fout);
    }
    if( Options.GetOptOutputFormat() == "adf" ) {
        recognized = true;
        result = WriteADF(p_fout);
    }
    if( Options.GetOptOutputFormat() == "adfgeo" ) {
        recognized = true;
        result = WriteADFGeo(p_fout);
    }
    if( Options.GetOptOutputFormat() == "adffrag" ) {
        recognized = true;
        result = WriteADFFrag(p_fout);
    }
    if( Options.GetOptOutputFormat() == "adf3drism" ) {
        recognized = true;
        result = WriteADF3DRISM(p_fout);
    }

    // close output file if necessary
    if( Options.GetArgOutName() != "-" ) {
        if( p_fout ) fclose(p_fout);
    }

    if( recognized == false ) {
        // format is not supported - here only silent error
        // this should be handled in CTopCrd2CrdOptions::CheckOptions with error
        // message for user
        CSmallString error;
        error << "not implemented output format " << Options.GetOptOutputFormat();
        ES_ERROR(error);
        return(false);
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Crd::Finalize(void)
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

bool CTopCrd2Crd::ReadCRD(FILE* p_fin)
{
    if( Coordinates.Load(p_fin) == false ) {
        CSmallString error;
        error << "unable to load input coordinates (crd): " << Options.GetArgCrdName();
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::ReadCRD(const CSmallString& name)
{
    if( Coordinates.Load(name,false,AMBER_RST_UNKNOWN) == false ) {
        CSmallString error;
        error << "unable to load input coordinates (crd): " << Options.GetArgCrdName();
        ES_ERROR(error);
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::ReadXYZ(FILE* p_fin)
{
    CXYZStructure xyz_str;

    if( xyz_str.Load(p_fin) == false ) {
        CSmallString error;
        error << "unable to load input coordinates (xyz): " << Options.GetArgCrdName();
        ES_ERROR(error);
        return(false);
    }

    // check number of atoms
    Coordinates.Create();
    if( Coordinates.GetNumberOfAtoms() != xyz_str.GetNumberOfAtoms() ){
        CSmallString error;
        error << "inconsistent number of atoms: topology " << Coordinates.GetNumberOfAtoms();
        error << "; coordinates " << xyz_str.GetNumberOfAtoms();
        ES_ERROR(error);
        return(false);
    }

    // copy coordinates
    for(int i=0; i < xyz_str.GetNumberOfAtoms(); i++){
        Coordinates.SetPosition(i,xyz_str.GetPosition(i));
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2Crd::WriteCRD(FILE* p_fout,bool binary)
{
    CAmberTopology cut_topology;
    CAmberRestart  cut_coordinates;

    // prepare fake cut topology
    cut_topology.AtomList.InitFields(Mask.GetNumberOfSelectedAtoms(),0,0,0);

    // assign topology to coordinates
    cut_coordinates.AssignTopology(&cut_topology);
    if( cut_coordinates.Create() == false ) {
        return(false);
    }

    // copy box if the whole system is selected or when requested
    if( Options.IsOptCopyBoxSet() || (Mask.GetNumberOfSelectedAtoms() == Topology.AtomList.GetNumberOfAtoms())  ){
        cut_topology.BoxInfo = Topology.BoxInfo;
    }

    // copy coordinates and optionally velocities
    int j=0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( Mask.IsAtomSelected(i) == false ) continue;

        CPoint pos = Coordinates.GetPosition(i);
        cut_coordinates.SetPosition(j,pos);

        if( (Coordinates.AreVelocitiesLoaded() == true) && (!Options.IsOptNoVelocitiesSet()) ) {
            CPoint vel = Coordinates.GetVelocity(i);
            cut_coordinates.SetVelocity(j,vel);
        }

        j++;
    }

    // copy box if neccessary
    if( Coordinates.IsBoxPresent() == true ) {
        cut_coordinates.SetBox(Coordinates.GetBox());
        cut_coordinates.SetAngles(Coordinates.GetAngles());
    }

    // copy time
    cut_coordinates.SetTime(Coordinates.GetTime());

    bool result;
    if( binary ){
        result = cut_coordinates.Save(Options.GetArgOutName(),false,AMBER_RST_NETCDF);
    } else {
        result = cut_coordinates.Save(p_fout);
    }

    return(result);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WritePDB(FILE* p_fout)
{

    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

    CSmallTimeAndDate time;
    time.GetActualTimeAndDate();

    // init indexes for TER and chain
    Topology.InitMoleculeIndexes();

    // write header
    WritePDBRemark(p_fout,"File generated with topcrd2crd --output pdb");
    WritePDBRemark(p_fout,"=== Topology ===");
    WritePDBRemark(p_fout,Options.GetArgTopologyName());
    WritePDBRemark(p_fout,"=== Coordinates ===");
    WritePDBRemark(p_fout,Options.GetArgCrdName());
    WritePDBRemark(p_fout,"=== Date ===");
    WritePDBRemark(p_fout,time.GetSDateAndTime());
    WritePDBRemark(p_fout,"=== Number of selected atoms ===");
    CSmallString tmp;
    tmp.IntToStr(Mask.GetNumberOfSelectedAtoms());
    WritePDBRemark(p_fout,tmp);
    WritePDBRemark(p_fout,"=== Mask ===");
    WritePDBRemark(p_fout,Mask.GetMask());

    int last_mol_id = -1;

    int resid = 0;
    int oldresid = 0;
    int atid = 0;
    char chain_id = 'A';
    double occ=1.0;
    double tfac=0.0;
    int seg_id = 1;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom != NULL ) {
            if( last_mol_id == -1 ) {
                last_mol_id = p_atom->GetMoleculeIndex();
            } else {
                if( last_mol_id != p_atom->GetMoleculeIndex() ) {
                    fprintf(p_fout,"TER\n");
                    chain_id++;
                    seg_id++;
                    last_mol_id = p_atom->GetMoleculeIndex();
                }
            }
            if( chain_id > 'Z' ){
                chain_id = 'A';
            }
            if( strncmp(p_atom->GetResidue()->GetName(),"WAT ",4) == 0 ){
                chain_id = 'W';
            }
            if( Options.GetOptReindex() ){
                if( oldresid != p_atom->GetResidue()->GetIndex()+1 ){
                    resid++;
                    oldresid = p_atom->GetResidue()->GetIndex()+1;
                }
                atid++;
            } else {
                resid = p_atom->GetResidue()->GetIndex()+1;
                atid =  i+1;
            }
            if( seg_id > 99 ){
                seg_id = 1;
            }
            if( atid > 999999 ) {
                atid = 1;
            }
            if( resid > 9999 ){
                resid = 1;
            }
            if( Options.GetOptNoChains() ) {
                chain_id = ' ';
            }
            fprintf(p_fout,"ATOM  %5d %4s %4s%c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f     P%02d%4s\n",
                    atid,GetPDBAtomName(p_atom,p_atom->GetResidue()),GetPDBResName(p_atom,p_atom->GetResidue()),
                    chain_id,
                    resid,
                    Coordinates.GetPosition(i).x,Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z,
                    occ,tfac,seg_id,PeriodicTable.GetSymbol(p_atom->GuessZ()));
        }
    }

    fprintf(p_fout,"TER\n");

    return(true);
}

//------------------------------------------------------------------------------

const char* CTopCrd2Crd::GetPDBResName(CAmberAtom* p_atom,CAmberResidue* p_res)
{
    static char name[5];

    // clear name
    memset(name,0,5);

    // direct use
    if( Options.GetOptMangleNames() == "charmm" ){
        if( strncmp(p_res->GetName(),"HIE ",4) == 0 ) return("HSE ");
        if( strncmp(p_res->GetName(),"HIP ",4) == 0 ) return("HSP ");
        if( strncmp(p_res->GetName(),"HID ",4) == 0 ) return("HSD ");
        memcpy(name,p_res->GetName(),4);
        return(name);
    }

    // no change
    memcpy(name,p_res->GetName(),4);
    return(name);
}

//------------------------------------------------------------------------------

const char* CTopCrd2Crd::GetPDBAtomName(CAmberAtom* p_atom,CAmberResidue* p_res)
{
    static char name[5];

    // clear name
    memset(name,0,5);

    // direct use
    if( Options.GetOptMangleNames() == "none" ){
        memcpy(name,p_atom->GetName(),4);
        return(name);
    }

    // pdb usage
    if( Options.GetOptMangleNames() == "pdb" ){
        memcpy(name,p_atom->GetPDBName(),4);
        return(name);
    }

    // charmm usage
    if( Options.GetOptMangleNames() == "charmm" ){
        // is residue terminal
        bool terminal = false;

        int resid = p_res->GetIndex();
        if( resid == Topology.ResidueList.GetNumberOfResidues() - 1 ){
            terminal = true;
        } else {
            CAmberResidue* p_nextres = Topology.ResidueList.GetResidue(resid+1);
            CAmberAtom* p_nextatm = Topology.AtomList.GetAtom(p_nextres->GetFirstAtomIndex());
            terminal = p_nextatm->GetMoleculeIndex() != p_atom->GetMoleculeIndex();
        }

        // terminal hydrogens
        if( strncmp(p_atom->GetName(),"H1  ",4) == 0 ) return("HT1 ");
        if( strncmp(p_atom->GetName(),"H2  ",4) == 0 ) return("HT2 ");
        if( strncmp(p_atom->GetName(),"H3  ",4) == 0 ) return("HT3 ");

        // terminal COO(-)
        if( strncmp(p_atom->GetName(),"OXT ",4) == 0 ) return("OT2 ");
        if( (strncmp(p_atom->GetName(),"O   ",4) == 0) && terminal ) return("OT1 ");

        // backbone hydrogen
        if( strncmp(p_atom->GetName(),"H   ",4) == 0 ) return("HN  ");

        // renumbering
        if( strncmp(p_atom->GetName(),"HA2 ",4) == 0 ) return("HA1 ");
        if( strncmp(p_atom->GetName(),"HA3 ",4) == 0 ) return("HA2 ");

        if( (strncmp(p_atom->GetName(),"HB2 ",4) == 0) && (strncmp(p_res->GetName(),"ALA ",4) != 0) ) return("HB1 ");
        if( (strncmp(p_atom->GetName(),"HB3 ",4) == 0) && (strncmp(p_res->GetName(),"ALA ",4) != 0) ) return("HB2 ");

        if( strncmp(p_atom->GetName(),"HC2 ",4) == 0 ) return("HB1 ");
        if( strncmp(p_atom->GetName(),"HC3 ",4) == 0 ) return("HB2 ");

        if( (strncmp(p_atom->GetName(),"HD2 ",4) == 0) && (strncmp(p_res->GetName(),"LYS ",4) == 0) ) return("HD1 ");
        if( (strncmp(p_atom->GetName(),"HD3 ",4) == 0) && (strncmp(p_res->GetName(),"LYS ",4) == 0) ) return("HD2 ");

        if( (strncmp(p_atom->GetName(),"HD2 ",4) == 0) && (strncmp(p_res->GetName(),"PRO ",4) == 0) ) return("HD1 ");
        if( (strncmp(p_atom->GetName(),"HD3 ",4) == 0) && (strncmp(p_res->GetName(),"PRO ",4) == 0) ) return("HD2 ");

        if( (strncmp(p_atom->GetName(),"HD2 ",4) == 0) && (strncmp(p_res->GetName(),"ARG ",4) == 0) ) return("HD1 ");
        if( (strncmp(p_atom->GetName(),"HD3 ",4) == 0) && (strncmp(p_res->GetName(),"ARG ",4) == 0) ) return("HD2 ");

        if( (strncmp(p_atom->GetName(),"HE2 ",4) == 0) && (strncmp(p_res->GetName(),"LYS ",4) == 0) ) return("HE1 ");
        if( (strncmp(p_atom->GetName(),"HE3 ",4) == 0) && (strncmp(p_res->GetName(),"LYS ",4) == 0) ) return("HE2 ");

        if( (strncmp(p_atom->GetName(),"HG2 ",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) != 0) ) return("HG1 ");
        if( (strncmp(p_atom->GetName(),"HG3 ",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) != 0) ) return("HG2 ");

        if( (strncmp(p_atom->GetName(),"HG12",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) == 0) ) return("HG11");
        if( (strncmp(p_atom->GetName(),"HG13",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) == 0) ) return("HG12");

        if( (strncmp(p_atom->GetName(),"HD11",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) == 0) ) return("HD1 ");
        if( (strncmp(p_atom->GetName(),"HD12",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) == 0) ) return("HD2 ");
        if( (strncmp(p_atom->GetName(),"HD13",4) == 0) && (strncmp(p_res->GetName(),"ILE ",4) == 0) ) return("HD3 ");

        if( (strncmp(p_atom->GetName(),"HG  ",4) == 0) && (strncmp(p_res->GetName(),"CYS ",4) == 0) ) return("HG1 ");

        // use original name
        memcpy(name,p_atom->GetName(),4);

        return(name);
    }

    return(name);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WritePQR(FILE* p_fout)
{

    //123456 78901 2 3456 7 8901 2 3456 7890 1234567 89012345 67890123456789012345678901234567890
    //ATOM     145    N     VAL  A   25       32.433   16.336   57.540  1.00 11.92      A1   N

    CSmallTimeAndDate time;
    time.GetActualTimeAndDate();

    // write header
    WritePDBRemark(p_fout,"File generated with topcrd2crd --output pqr");
    WritePDBRemark(p_fout,"=== Topology ===");
    WritePDBRemark(p_fout,Options.GetArgTopologyName());
    WritePDBRemark(p_fout,"=== Coordinates ===");
    WritePDBRemark(p_fout,Options.GetArgCrdName());
    WritePDBRemark(p_fout,"=== Date ===");
    WritePDBRemark(p_fout,time.GetSDateAndTime());
    WritePDBRemark(p_fout,"=== Number of selected atoms ===");
    CSmallString tmp;
    tmp.IntToStr(Mask.GetNumberOfSelectedAtoms());
    WritePDBRemark(p_fout,tmp);
    WritePDBRemark(p_fout,"=== Mask ===");
    WritePDBRemark(p_fout,Mask.GetMask());

    int atid = 0;
    int resid = 0;
    int oldresid = 0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;

        if( Options.GetOptReindex() ){
            if( oldresid != p_atom->GetResidue()->GetIndex()+1 ){
                resid++;
                oldresid = p_atom->GetResidue()->GetIndex()+1;
            }
            atid++;
        } else {
            resid = p_atom->GetResidue()->GetIndex()+1;
            atid =  i+1;
        }

        if( atid > 999999 ) {
            atid = 1;
        }
        if( resid > 9999 ){
            resid = 1;
        }

        fprintf(p_fout,"ATOM  %5d %4s %4s %4d    %8.3f%8.3f%8.3f %6.3f%6.3f\n",
                atid,(const char*)p_atom->GetPDBName(),p_atom->GetResidue()->GetName(),
                resid,
                Coordinates.GetPosition(i).x,Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z,
                p_atom->GetStandardCharge(),p_atom->GetRadius()*Options.GetOptRScale()+Options.GetOptROffset());
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteXYZ(FILE* p_fout)
{
    fprintf(p_fout,"%d\n",Mask.GetNumberOfSelectedAtoms());
    fprintf(p_fout,"xyz file created by topcrd2crd --output xyz\n");

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        int z = p_atom->GuessZ();
        if( z == 0 ) z = 1;
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        CPoint pos = Coordinates.GetPosition(i);
        fprintf(p_fout,"%2s %14.7f %14.7f %14.7f\n",p_symbol,
                pos.x,pos.y,pos.z);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteXYZR(FILE* p_fout)
{

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        fprintf(p_fout,"%8.3f%8.3f%8.3f %6.3f\n",
                Coordinates.GetPosition(i).x,Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z,
                p_atom->GetRadius()*Options.GetOptRScale()+Options.GetOptROffset());
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteCOM(FILE* p_fout)
{
    CFileName basename = Options.GetArgOutName();
    if( Options.GetArgOutName() == "-" ) {
        basename = "output.com";
    }
    basename = basename.GetFileNameWithoutExt();

    fprintf(p_fout,"%%chk=%s.chk\n",(const char*)basename);
    fprintf(p_fout,"# <- setup for method ->\n");
    fprintf(p_fout,"\n");
    fprintf(p_fout,"Top(%s) Crd(%s) Mask(%s)\n",
            (const char*)Options.GetArgTopologyName(),
            (const char*)Options.GetArgCrdName(),
            (const char*)Mask.GetMask());
    fprintf(p_fout,"\n");

    // calculate total charge
    double fcrg = 0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        fcrg += p_atom->GetStandardCharge();
    }
    int crg = round(fcrg);

    fprintf(p_fout,"%d 1\n",crg);

    // write coordinates --------------------
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        int z = p_atom->GuessZ();
        if( z == 0 ) z = 1;
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        fprintf(p_fout,"%2s %12.6f %12.6f %12.6f\n",
                p_symbol,Coordinates.GetPosition(i).x,
                Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
    }

    fprintf(p_fout,"\n");

    // write charges --------------------
    if( ChrgMask.GetNumberOfSelectedAtoms() > 0 ){
        for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
            CAmberAtom* p_atom = ChrgMask.GetSelectedAtom(i);
            if( p_atom == NULL ) continue;
            fprintf(p_fout,"   %12.6f %12.6f %12.6f %12.6f\n",
                    Coordinates.GetPosition(i).x,
                    Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z,p_atom->GetStandardCharge());
        }
        fprintf(p_fout,"\n");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteORCA(FILE* p_fout)
{
    fprintf(p_fout,"! <- setup for method ->\n");
    fprintf(p_fout,"\n");
    fprintf(p_fout,"# Top(%s) Crd(%s) Mask(%s)\n",
            (const char*)Options.GetArgTopologyName(),
            (const char*)Options.GetArgCrdName(),
            (const char*)Mask.GetMask());
    fprintf(p_fout,"\n");

    // calculate total charge
    double fcrg = 0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        fcrg += p_atom->GetStandardCharge();
    }
    int crg = round(fcrg);

    fprintf(p_fout,"*xyz %d 1\n",crg);

    // write coordinates --------------------
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        int z = p_atom->GuessZ();
        if( z == 0 ) z = 1;
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        fprintf(p_fout,"%2s              %12.6f %12.6f %12.6f\n",
                p_symbol,Coordinates.GetPosition(i).x,
                Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
    }

    // write charges --------------------
    if( ChrgMask.GetNumberOfSelectedAtoms() > 0 ){
        for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
            CAmberAtom* p_atom = ChrgMask.GetSelectedAtom(i);
            if( p_atom == NULL ) continue;
            fprintf(p_fout," Q %12.6f %12.6f %12.6f %12.6f\n",
                    p_atom->GetStandardCharge(),
                    Coordinates.GetPosition(i).x,Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
        }
    }

    fprintf(p_fout,"*\n\n");

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WritePC(FILE* p_fout)
{
    fprintf(p_fout,"%d\n",Mask.GetNumberOfSelectedAtoms());

    // write charges --------------------
    if( Mask.GetNumberOfSelectedAtoms() > 0 ){
        for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
            CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
            if( p_atom == NULL ) continue;
            fprintf(p_fout,"%12.6f %12.6f %12.6f %12.6f\n",
                    p_atom->GetStandardCharge(),
                    Coordinates.GetPosition(i).x,Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
        }
    }

    fprintf(p_fout,"\n");

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteRBOX(FILE* p_fout)
{

    fprintf(p_fout,"3 rbox %d\n",Mask.GetNumberOfSelectedAtoms());
    fprintf(p_fout,"%d\n",Mask.GetNumberOfSelectedAtoms());

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        CPoint pos;
        pos = Coordinates.GetPosition(i);
        fprintf(p_fout,"%16.12f %16.12f %16.12f\n",pos.x,pos.y,pos.z);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteTRAJ(FILE* p_fout)
{
    CAmberTopology     cut_topology;
    CAmberTrajectory   cut_trajectory;
    CAmberRestart      cut_restart;

    // prepare fake cut topology
    cut_topology.AtomList.InitFields(Mask.GetNumberOfSelectedAtoms(),0,0,0);

    // assign topology to trajectory and snapshot
    cut_trajectory.AssignTopology(&cut_topology);
    cut_restart.AssignTopology(&cut_topology);

    // assign trajectory to file stream
    cut_trajectory.AssignTrajectoryToFile(p_fout,AMBER_TRAJ_UNKNOWN,AMBER_TRAJ_CXYZB,AMBER_TRAJ_WRITE);
    cut_trajectory.AssignRestart(&cut_restart);

    // copy box if the whole system is selected or when requested
    if( Options.IsOptCopyBoxSet() || (Mask.GetNumberOfSelectedAtoms() == Topology.AtomList.GetNumberOfAtoms())  ){
        cut_topology.BoxInfo = Topology.BoxInfo;
    }

    // copy coordinates and optionally velocities
    int j=0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        if( Mask.IsAtomSelected(i) == false ) continue;
        CPoint pos = Coordinates.GetPosition(i);
        cut_restart.SetPosition(j,pos);
        j++;
    }

    // copy box if neccessary
    if( Coordinates.IsBoxPresent() == true ){
        cut_restart.SetBox(Coordinates.GetBox());
        cut_restart.SetAngles(Coordinates.GetAngles());
    }

    bool result;
    result = cut_trajectory.WriteSnapshot();

    return(result);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WritePDBRemark(FILE* p_file,const CSmallString& string)
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

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteADFGeo(FILE* p_fout)
{
    fprintf(p_fout,"ATOMS Cartesian\n");
    int j=1;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        int z = p_atom->GuessZ();
        if( z == 0 ) z = 1;
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        CPoint pos = Coordinates.GetPosition(i);
        fprintf(p_fout,"% 6d %2s %20.7f %20.7f %20.7f\n",j,p_symbol,pos.x,pos.y,pos.z);
        j++;
    }
    fprintf(p_fout,"END\n\n");

    double total_charge = 0.0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom != NULL ) {
            total_charge += p_atom->GetStandardCharge();
        }
    }
    int tcharge = round(total_charge);
    fprintf(p_fout,"CHARGE %d\n\n",tcharge);

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteADF(FILE* p_fout)
{
    if( WriteADFGeo(p_fout) == false ) return(false);

    fprintf(p_fout,"XC\n");
    fprintf(p_fout,"    GGA PBE\n");
    fprintf(p_fout,"    DISPERSION Grimme3 BJDAMP\n");
    fprintf(p_fout,"END\n\n");

    fprintf(p_fout,"BASIS\n");
    fprintf(p_fout,"    Type DZP\n");
    fprintf(p_fout,"    Core Small\n");
    fprintf(p_fout,"END\n");

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteADFFrag(FILE* p_fout)
{
    Topology.InitMoleculeIndexes();

    fprintf(p_fout,"#!/usr/bin/env infinity-env\n\n");
    fprintf(p_fout,"module add adf\n\n");

    int f=0;
    int curr_frag = -1;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        if( curr_frag != p_atom->GetMoleculeIndex() ){
            curr_frag = p_atom->GetMoleculeIndex();
            f++;
        } else {
            continue;
        }

        fprintf(p_fout,"#-------------------------------------------------------------------------------\n");
        fprintf(p_fout,"# FRAGMENT %d\n",f);
        fprintf(p_fout,"#-------------------------------------------------------------------------------\n\n");
        fprintf(p_fout,"adf > F%d.out << EOF\n\n",f);
        fprintf(p_fout,"TITLE FRAGMENT%d\n\n",f);
        fprintf(p_fout,"ATOMS Cartesian\n");
        int j=1;
        double total_charge = 0.0;
        for(int k = 0; k < Topology.AtomList.GetNumberOfAtoms(); k++) {
            CAmberAtom* p_atom = Mask.GetSelectedAtom(k);
            if( p_atom == NULL ) continue;
            if( p_atom->GetMoleculeIndex() != curr_frag ) continue;
            int z = p_atom->GuessZ();
            if( z == 0 ) z = 1;
            const char* p_symbol = PeriodicTable.GetSymbol(z);
            CPoint pos = Coordinates.GetPosition(k);
            fprintf(p_fout,"% 6d %2s %20.7f %20.7f %20.7f\n",j,p_symbol,pos.x,pos.y,pos.z);
            j++;
            total_charge += p_atom->GetStandardCharge();
        }
        fprintf(p_fout,"END\n\n");

        int tcharge = round(total_charge);
        fprintf(p_fout,"CHARGE %d\n\n",tcharge);

        fprintf(p_fout,"XC\n");
        fprintf(p_fout,"    GGA PBE\n");
        fprintf(p_fout,"    DISPERSION Grimme3 BJDAMP\n");
        fprintf(p_fout,"END\n\n");

        fprintf(p_fout,"BASIS\n");
        fprintf(p_fout,"    Type TZ2P\n");
        fprintf(p_fout,"END\n\n");
        fprintf(p_fout,"EOF\n\n");
        fprintf(p_fout,"mv TAPE21 F%d.TAPE21\n\n",f);
    }

    fprintf(p_fout,"#-------------------------------------------------------------------------------\n");
    fprintf(p_fout,"# COMPLEX\n");
    fprintf(p_fout,"#-------------------------------------------------------------------------------\n\n");


    fprintf(p_fout,"adf > COMPLEX.out << EOF\n\n");
    fprintf(p_fout,"TITLE COMPLEX\n\n");

    fprintf(p_fout,"ATOMS Cartesian\n");
    int j=1;
    f=0;
    int prev_frag = 0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        if( prev_frag == 0 ){
            prev_frag = p_atom->GetMoleculeIndex();
            f++;
        }
        int z = p_atom->GuessZ();
        if( z == 0 ) z = 1;
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        CPoint pos = Coordinates.GetPosition(i);
        if( prev_frag != p_atom->GetMoleculeIndex() ){
            prev_frag = p_atom->GetMoleculeIndex();
            f++;
        }
        fprintf(p_fout,"% 6d %2s %20.7f %20.7f %20.7f f=F%d\n",j,p_symbol,pos.x,pos.y,pos.z,f);
        j++;
    }
    fprintf(p_fout,"END\n\n");

    double total_charge = 0.0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom != NULL ) {
            total_charge += p_atom->GetStandardCharge();
        }
    }
    int tcharge = round(total_charge);
    fprintf(p_fout,"CHARGE %d\n\n",tcharge);

    fprintf(p_fout,"FRAGMENTS\n");
    f=0;
    prev_frag = 0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        if( prev_frag != p_atom->GetMoleculeIndex() ){
            prev_frag = p_atom->GetMoleculeIndex();
            f++;
        } else {
            continue;
        }
        fprintf(p_fout,"    F%d F%d.TAPE21\n",f,f);
    }
    fprintf(p_fout,"END\n");

    fprintf(p_fout,"XC\n");
    fprintf(p_fout,"    GGA PBE\n");
    fprintf(p_fout,"    DISPERSION Grimme3 BJDAMP\n");
    fprintf(p_fout,"END\n\n");

    fprintf(p_fout,"BASIS\n");
    fprintf(p_fout,"    Type TZ2P\n");
    fprintf(p_fout,"END\n\n");

    fprintf(p_fout,"EOF\n\n");

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2Crd::WriteADF3DRISM(FILE* p_fout)
{
    CPoint min,max;

    fprintf(p_fout,"Title 3DRISM\n\n");

    fprintf(p_fout,"ATOMS Cartesian\n");
    int j=1;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom == NULL ) continue;
        int z = p_atom->GuessZ();
        if( z == 0 ) z = 1;
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        CPoint pos = Coordinates.GetPosition(i);

        double a,b,eps,rii,sig;

        a = Topology.NonBondedList.GetAParam(p_atom,p_atom);
        b = Topology.NonBondedList.GetBParam(p_atom,p_atom);

        eps = 0.0;
        rii = 0.0;
        if( a != 0.0 ) {
            eps = b*b / (4.0 * a);
            rii = pow(2*a/b,1.0/6.0) * 0.5;
        }
        sig = rii*2.0/1.122; // 1.122 = 2^1/6
        fprintf(p_fout,"% 6d %2s %20.7f %20.7f %20.7f SigU=%5.3f EpsU=%6.4f\n",j,p_symbol,pos.x,pos.y,pos.z,sig,eps);

        if( j == 1 ){
            min = pos;
            max = pos;
        } else {
            if( min.x > pos.x ) min.x = pos.x;
            if( min.y > pos.y ) min.y = pos.y;
            if( min.z > pos.z ) min.z = pos.z;
            if( max.x < pos.x ) max.x = pos.x;
            if( max.y < pos.y ) max.y = pos.y;
            if( max.z < pos.z ) max.z = pos.z;
        }

        j++;
    }
    fprintf(p_fout,"END\n\n");

    double total_charge = 0.0;
    for(int i = 0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {
        CAmberAtom* p_atom = Mask.GetSelectedAtom(i);
        if( p_atom != NULL ) {
            total_charge += p_atom->GetStandardCharge();
        }
    }
    int tcharge = round(total_charge);
    fprintf(p_fout,"CHARGE %d\n\n",tcharge);

    fprintf(p_fout,"STOFIT\n");
    fprintf(p_fout,"XC\n");
    fprintf(p_fout,"    GGA PBE\n");
    fprintf(p_fout,"    DISPERSION Grimme3 BJDAMP\n");
    fprintf(p_fout,"END\n\n");

    fprintf(p_fout,"BASIS\n");
    fprintf(p_fout,"    Type DZP\n");
    fprintf(p_fout,"    Core Small\n");
    fprintf(p_fout,"END\n\n");

    fprintf(p_fout,"RISM solute\n");
    fprintf(p_fout,"    RISM1D\n");
    fprintf(p_fout,"        FLUIDPARAM temper=298.0 DielConst=78.497 UTotDens=1/A3 0.03333\n");
    fprintf(p_fout,"    SUBEND\n");

    fprintf(p_fout,"    SOLVENT TIP3P\n");
    fprintf(p_fout,"        UNITS      uWeight=g/mol  ULJsize=A  ULJenergy=kcal/mol Ucoord=A Udens=1/A3\n");
    fprintf(p_fout,"        Parameters Weight=18.015  nAtoms=2\n");
    fprintf(p_fout,"        1    -0.834000   3.152    0.1520     0.000000  0.000000  0.000000\n");
    fprintf(p_fout,"        2     0.417000   1.238    0.0152     0.957200  0.000000  0.000000\n");
    fprintf(p_fout,"                                            -0.239988 -0.926627  0.000000\n");
    fprintf(p_fout,"        DenSpe=0.03333\n");
    fprintf(p_fout,"    SUBEND\n");

    // bounding box
    CPoint box = (max-min)*2.5;

    fprintf(p_fout,"    SOLUTE\n");
    fprintf(p_fout,"        BOXSIZE %5.1f %5.1f %5.1f\n",box.x,box.y,box.z);
    fprintf(p_fout,"        BOXGRID   64   64   64\n");
    fprintf(p_fout,"    SUBEND\n");
    fprintf(p_fout,"END\n\n");

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

