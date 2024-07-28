// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2018 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <AmberSubTopology.hpp>

#include "TopRMLA.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopRMLA::CTopRMLA(void)
{

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopRMLA::Init(int argc,char* argv[])
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
        printf("# toprmla (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgOldTopName() == "-" ) {
            printf("# Old topology name  : -stdin- (standard input)\n");
        } else {
            printf("# Old topology name  : %s\n",(const char*)Options.GetArgOldTopName());
        }
            printf("# Coordinate name    : %s\n",(const char*)Options.GetArgCrdName());
        if( Options.GetArgNewTopName() == "-" ) {
            printf("# New topology name  : -stdout- (standard output)\n");
        } else {
            printf("# New topology name  : %s\n",(const char*)Options.GetArgNewTopName());
        }
        printf("# ------------------------------------------------------------------------------\n");
        if( Options.IsOptMaskSpecSet() == true ) {
            printf("# Central atoms      : %s\n",(const char*)Options.GetOptMaskSpec());
        } else {
            printf("# Central atoms      : all atoms\n");
        }
            printf("# Deviation (max)    : %10.4f\n",Options.GetOptMaxDeviation());
            printf("# Linear (min)       : %10.4f\n",Options.GetOptMinLinear());

        printf("# ------------------------------------------------------------------------------\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopRMLA::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgOldTopName(),true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgOldTopName());
        return(false);
    }

    // load coordinates
    Coordinates.AssignTopology(&Topology);
    if( Coordinates.Load(Options.GetArgCrdName(),false,AMBER_RST_UNKNOWN) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                (const char*)Options.GetArgCrdName());
        return(false);
    }

    Mask.AssignTopology(&Topology);
    Mask.AssignCoordinates(&Coordinates);

    // init mask ----------------------------------------------
    bool result = true;

    // by default all atoms are selected
    Mask.SelectAllAtoms();

    // do we have user specified mask?
    if( Options.IsOptMaskSpecSet() == true ) {
        result = Mask.SetMask(Options.GetOptMaskSpec());
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified mask!\n");
        return(false);
    }

// filter angles ...

    if( Options.GetOptVerbose() ){
        fprintf(stdout,"\n");
        fprintf(stdout,"# ANGLES\n");
        fprintf(stdout,"#   IT   Name Type|   JT    Name Type|   KT    Name Type|Topology  Coords    Diff    Status \n");
        fprintf(stdout,"# ------ ---- ----|-------- ---- ----|-------- ---- ----|-------- -------- -------- --------\n");
    }

    for(int i=0; i < Topology.AngleList.GetNumberOfAngles();i++){
        CAmberAngle* p_angle = Topology.AngleList.GetAngle(i);
        if( p_angle == NULL ) continue;
        double a1 = 0.0;
        if( p_angle->GetICT() >= 0 ){
            a1 = Topology.AngleList.GetAngleType(p_angle->GetICT())->GetTEQ() * 180.0 / M_PI;
        }
        double a2 = Angle(Coordinates.GetPosition(p_angle->GetIT()) - Coordinates.GetPosition(p_angle->GetJT()),
                          Coordinates.GetPosition(p_angle->GetKT()) - Coordinates.GetPosition(p_angle->GetJT())) * 180.0 / M_PI;
        if( Options.GetOptVerbose() ){
            CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(p_angle->GetIT());
            CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(p_angle->GetJT());
            CAmberAtom* p_atom3 = Topology.AtomList.GetAtom(p_angle->GetKT());
            fprintf(stdout,"%8d %4s %4s %8d %4s %4s %8d %4s %4s %8.2f %8.2f %8.2f ",
                            p_angle->GetIT()+1,(const char*)p_atom1->GetName(),(const char*)p_atom1->GetType(),
                            p_angle->GetJT()+1,(const char*)p_atom2->GetName(),(const char*)p_atom2->GetType(),
                            p_angle->GetKT()+1,(const char*)p_atom3->GetName(),(const char*)p_atom3->GetType(),a1,a2,a2-a1);
        }

        // is the central atom selected?
        if( Mask.IsAtomSelected(p_angle->GetJT()) == true ){
            if( abs(a1-a2) > Options.GetOptMaxDeviation() ){
                if( Options.GetOptVerbose() ){
                    fprintf(stdout,"REMOVED");
                }
                p_angle->SetICT(-1);

                // remove also dihedral angles
                for(int j=0; j < Topology.DihedralList.GetNumberOfDihedrals();j++){
                    bool removed = false;
                    CAmberDihedral* p_dih = Topology.DihedralList.GetDihedral(j);
                    if( (p_dih->GetIP() == p_angle->GetIT()) &&
                        (p_dih->GetJP() == p_angle->GetJT()) &&
                        (p_dih->GetKP() == p_angle->GetKT()) ){
                        p_dih->SetICP(-1);
                        removed = true;
                    }
                    if( (p_dih->GetIP() == p_angle->GetKT()) &&
                        (p_dih->GetJP() == p_angle->GetJT()) &&
                        (p_dih->GetKP() == p_angle->GetIT()) ){
                        p_dih->SetICP(-1);
                        removed = true;
                    }
                    if( (p_dih->GetJP() == p_angle->GetIT()) &&
                        (p_dih->GetKP() == p_angle->GetJT()) &&
                        (p_dih->GetLP() == p_angle->GetKT()) ){
                        p_dih->SetICP(-1);
                        removed = true;
                    }
                    if( (p_dih->GetJP() == p_angle->GetKT()) &&
                        (p_dih->GetKP() == p_angle->GetJT()) &&
                        (p_dih->GetLP() == p_angle->GetIT()) ){
                        p_dih->SetICP(-1);
                        removed = true;
                    }
                    if( removed && Options.GetOptVerbose() ){
                        fprintf(stdout,"+");
                    }
                }
                if( Options.GetOptVerbose() ){
                    fprintf(stdout,"\n");
                }
            } else {
                if( Options.GetOptVerbose() ){
                    fprintf(stdout,"  OK\n");
                }
            }
        } else {
            if( Options.GetOptVerbose() ){
                fprintf(stdout,"    not selected\n");
            }
        }
    }

// filter dihedrals ...

    if( Options.GetOptVerbose() ){
        fprintf(stdout,"\n");
        fprintf(stdout,"# DIHEDRALS\n");
        fprintf(stdout,"# Mode: 0 - normal dihedral angle | 1 - improper dihedral angle | -1 - terminal dihedral angle | -2 - terminal and improper dihedral angle\n");
        fprintf(stdout,"#   IT   Name Type|   JT    Name Type|   KT    Name Type|    LT   Name Type|Mode  I-J-K    J-K-L    Status \n");
        fprintf(stdout,"# ------ ---- ----|-------- ---- ----|-------- ---- ----|-------- ---- ----|---- -------- -------- --------\n");
    }

    for(int i=0; i < Topology.DihedralList.GetNumberOfDihedrals();i++){
        CAmberDihedral* p_dih = Topology.DihedralList.GetDihedral(i);
        if( p_dih == NULL ) continue;

        double a1 = Angle(Coordinates.GetPosition(p_dih->GetIP()) - Coordinates.GetPosition(p_dih->GetJP()),
                          Coordinates.GetPosition(p_dih->GetKP()) - Coordinates.GetPosition(p_dih->GetJP())) * 180.0 / M_PI;
        double a2 = Angle(Coordinates.GetPosition(p_dih->GetJP()) - Coordinates.GetPosition(p_dih->GetKP()),
                          Coordinates.GetPosition(p_dih->GetLP()) - Coordinates.GetPosition(p_dih->GetKP())) * 180.0 / M_PI;

        if( Options.GetOptVerbose() ){
            CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(p_dih->GetIP());
            CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(p_dih->GetJP());
            CAmberAtom* p_atom3 = Topology.AtomList.GetAtom(p_dih->GetKP());
            CAmberAtom* p_atom4 = Topology.AtomList.GetAtom(p_dih->GetLP());
            fprintf(stdout,"%8d %4s %4s %8d %4s %4s %8d %4s %4s %8d %4s %4s %4d %8.2f %8.2f ",
                            p_dih->GetIP()+1,(const char*)p_atom1->GetName(),(const char*)p_atom1->GetType(),
                            p_dih->GetJP()+1,(const char*)p_atom2->GetName(),(const char*)p_atom2->GetType(),
                            p_dih->GetKP()+1,(const char*)p_atom3->GetName(),(const char*)p_atom3->GetType(),
                            p_dih->GetLP()+1,(const char*)p_atom4->GetName(),(const char*)p_atom4->GetType(),
                            p_dih->GetType(),a1,a2);
        }

        bool selected = false;
        bool removed = false;
        if( Mask.IsAtomSelected(p_dih->GetJP()) == true ){
            selected = true;
            if( (abs(180.0 - a1) < Options.GetOptMinLinear()) || (abs(a1) < Options.GetOptMinLinear()) ){
                p_dih->SetICP(-1);
                removed = true;
            }
        }
        if( Mask.IsAtomSelected(p_dih->GetKP()) == true ){
            selected = true;
            if( (abs(180.0 - a2) < Options.GetOptMinLinear()) || (abs(a2) < Options.GetOptMinLinear()) ){
                p_dih->SetICP(-1);
                removed = true;
            }
        }
        if( Options.GetOptVerbose() ){
            if( selected ){
                if( removed ){
                    fprintf(stdout,"REMOVED\n");
                } else {
                    fprintf(stdout,"  OK\n");
                }
            } else {
                fprintf(stdout,"    not selected\n");
            }
        }
    }

    // clean topology
    Topology.AngleList.RemoveIllegalAngles();
    Topology.DihedralList.RemoveIllegalDihedrals();

    // save topology
    if( Topology.Save(Options.GetArgNewTopName(),AMBER_VERSION_7,true) == false ) {
        fprintf(stderr,">>> ERROR: Unable to save specified topology: %s\n",
                (const char*)Options.GetArgNewTopName());
        return(false);
    }

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopRMLA::Finalize(void)
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

