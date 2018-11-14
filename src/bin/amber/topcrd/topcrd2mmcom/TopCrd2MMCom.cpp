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
#include <ctype.h>
#include <ErrorSystem.hpp>
#include <PeriodicTable.hpp>
#include <FileName.hpp>
#include <SmallTimeAndDate.hpp>
#include <AmberMaskAtoms.hpp>
#include <AmberMaskResidues.hpp>
#include <AmberTrajectory.hpp>
#include "TopCrd2MMCom.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTopCrd2MMCom::CTopCrd2MMCom(void)
{
    Oniom = false;
    PrintHrmBndParamsForWater = true;
    CalcEb = true;
    CalcEa = true;
    CalcEd = true;
    CalcEvdw = true;
    CalcEel = true;
    LowLinkAtomName = "H";
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTopCrd2MMCom::Init(int argc,char* argv[])
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
        printf("# topcrd2mmcom (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        printf("# Topology name      : %s\n",(const char*)Options.GetArgTopologyName());
        if( Options.GetArgCrdName() != "-" ) {
            printf("# Input coordinates  : %s\n",(const char*)Options.GetArgCrdName());
        } else {
            printf("# Input coordinates  : - (standard input)\n");
        }
        if( Options.GetArgOutName() != "-" ) {
            printf("# Output com file    : %s\n",(const char*)Options.GetArgOutName());
        } else {
            printf("# Output com file    : - (standard output)\n");
        }
        printf("# ------------------------------------------------------------------------------\n");
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
        printf("# ------------------------------------------------------------------------------\n");
        printf("\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2MMCom::Run(void)
{
    // load topology
    if( Topology.Load(Options.GetArgTopologyName()) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    // load coordinates
    Coordinates.AssignTopology(&Topology);
    if( Coordinates.Load(Options.GetArgCrdName(),false,AMBER_RST_UNKNOWN) == false ) {
        fprintf(stderr,">>> ERROR: Unable to load specified coordinates: %s\n",
                (const char*)Options.GetArgCrdName());
        return(false);
    }

    // assign topology and coordinates to the mask -------------
    HighAmberMask.AssignTopology(&Topology);
    HighAmberMask.AssignCoordinates(&Coordinates);

    // init mask ----------------------------------------------
    bool result = true;

    // by default all atoms are selected
    HighAmberMask.SelectAllAtoms();

    // do we have user specified mask?
    if( Options.IsOptMaskSpecSet() == true ) {
        result = HighAmberMask.SetMask(Options.GetOptMaskSpec());
        Oniom = true;
    }

    // do we have user specified mask?
    if( Options.IsOptMaskFileSet() == true ) {
        result = HighAmberMask.SetMaskFromFile(Options.GetOptMaskFile());
        Oniom = true;
    }

    if( result == false ) {
        fprintf(stderr,">>> ERROR: Unable to set specified mask!\n");
        return(false);
    }

    // open output file
    if( Options.GetArgOutName() != "-" ) {
        if( (OutputFile = fopen(Options.GetArgOutName(),"w")) == NULL ) {
            fprintf(stderr,">>> ERROR: Unable to open output file (%s)!\n",strerror(errno));
            return(false);
        }
    } else {
        OutputFile = stdout;
    }

    DetermineBoundaries();
    MakeTypeMap();

    result = WriteDataToMMCom();

    // close output file if necessary
    if( Options.GetArgOutName() != "-" ) {
        fclose(OutputFile);
    }

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTopCrd2MMCom::Finalize(void)
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

bool CTopCrd2MMCom::DetermineBoundaries(void)
{
    if( Options.GetOptVerbose() ) {
        printf("Bonds crossing ONIOM boundaries ..\n");
        printf("\n");
        printf(" -------Low----- / -----High----- \n");
        printf("   ID  Name Type    ID  Name Type \n");
        printf("==================================\n");
    }

    int count = 0;

    for(int i=0; i < Topology.BondList.GetNumberOfBonds(); i++) {
        CAmberBond* p_bond = Topology.BondList.GetBond(i);
        bool cross_bond =   HighAmberMask.IsAtomSelected(p_bond->GetIB()) !=
                            HighAmberMask.IsAtomSelected(p_bond->GetJB());
        if( cross_bond == true ) {
            count++;
            CAmberAtom* p_at1 = Topology.AtomList.GetAtom(p_bond->GetIB());
            CAmberAtom* p_at2 = Topology.AtomList.GetAtom(p_bond->GetJB());
            if( HighAmberMask.IsAtomSelected(p_bond->GetIB()) == true ) {
                LowLinkAtoms.InsertToEnd(p_at2);
                if( Options.GetOptVerbose() ) {
                    printf("%6d %4s %4s %6d %4s %4s\n",
                           p_at2->GetAtomIndex()+1,p_at2->GetName(),
                           p_at2->GetType(),p_at1->GetAtomIndex()+1,
                           p_at1->GetName(),p_at1->GetType());
                }
            } else {
                LowLinkAtoms.InsertToEnd(p_at1);
                if( Options.GetOptVerbose() ) {
                    printf("%6d %4s %4s %6d %4s %4s\n",
                           p_at1->GetAtomIndex()+1,p_at1->GetName(),
                           p_at1->GetType(),p_at2->GetAtomIndex()+1,
                           p_at2->GetName(),p_at2->GetType());
                }
            }
        }
    }

    if( Options.GetOptVerbose() ) {
        if( count == 0 ) printf(" -- none --\n");
        printf("\n");
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2MMCom::MakeTypeMap(void)
{
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        TypeMap[p_atom->GetType()] = p_atom->GetType();
    }

    // remap
    std::map<CSmallString,CSmallString>::iterator it = TypeMap.begin();
    std::map<CSmallString,CSmallString>::iterator ie = TypeMap.end();

    int i = 0;
    while( it != ie ){
        char t1 = 'A';
        char t2 = 'A';
        t1 += i % 26;
        t2 += i / 26;
        CSmallString nt;
        nt << t2 << t1;
        if( nt == "BQ" ){ // BQ is special type
            i++;
            continue; // repeate
        }
        it->second = nt;
        it++;
        i++;
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteDataToMMCom(void)
{
    if( WriteHeader() == false ) return(false);

    if( WriteCoords() == false ) return(false);

    if( WriteConnect() == false ) return(false);

    if( WriteTypes() == false ) return(false);

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteHeader(void)
{
    CFileName chkname(Options.GetArgOutName());
    if( chkname.GetFileNameExt() == ".com" ) {
        chkname = chkname.GetFileNameWithoutExt();
    }
    fprintf(OutputFile,"%%chk=%s.chk\n",(const char*)chkname);
    fprintf(OutputFile,"%%mem=30MW\n");
    if( Oniom == true ) {
        fprintf(OutputFile,"# ONIOM(PM3:AMBER=SoftOnly) SP Geom=Connect\n");
    } else {
        fprintf(OutputFile,"# AMBER=SoftOnly SP Geom=Connect\n");
    }
    fprintf(OutputFile,"\n");
    if( Oniom == true ) {
        if( Options.IsOptMaskSpecSet() == true ) {
            fprintf(OutputFile,"Topology       : %s\nCoordinates    : %s\nHigh Layer Mask: %s\n",
                    (const char*)Options.GetArgTopologyName(),
                    (const char*)Options.GetArgCrdName(),(const char*)Options.GetOptMaskSpec());
        }
        if( Options.IsOptMaskFileSet() == true ) {
            fprintf(OutputFile,"Topology       : %s\nCoordinates    : %s\nHigh Layer Mask file: %s\n",
                    (const char*)Options.GetArgTopologyName(),
                    (const char*)Options.GetArgCrdName(),(const char*)Options.GetOptMaskFile());
        }
    } else {
        fprintf(OutputFile,"Topology   : %s\nCoordinates: %s\n",
                (const char*)Options.GetArgTopologyName(),
                (const char*)Options.GetArgCrdName());
    }
    fprintf(OutputFile,"\n");

    // calculate total charge
    double fcrg = 0;
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        fcrg += p_atom->GetStandardCharge();
    }
    int crg = (int)fcrg;

    if( Oniom == true ) {
        fprintf(OutputFile,"%d 1 FIX 1\n",crg);
    } else {
        fprintf(OutputFile,"%d 1\n",crg);
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteCoords(void)
{
    // write coordinates --------------------
    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);
        int z = PeriodicTable.SearchZByMass(p_atom->GetMass());
        const char* p_symbol = PeriodicTable.GetSymbol(z);
        char type[5];
        type[4]='\0';
        strncpy(type,TypeMap[p_atom->GetType()].GetBuffer(),4);
        for(int j=3; j > 0; j--) {
            if( isspace(type[j]) != 0 ) type[j] = '\0';
        }
        double charge = p_atom->GetStandardCharge();
        if( CalcEel == false ) {
            charge = 0.0;
        }
        if( Oniom == true ) {
            if( HighAmberMask.IsAtomSelected(i) == true ) {
                fprintf(OutputFile,"%s-%s-%-.4f %12.6f %12.6f %12.6f H\n",
                        p_symbol,type,charge,Coordinates.GetPosition(i).x,
                        Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
            } else {
                if( LowLinkAtoms.IsInList(p_atom) == true ) {
                    fprintf(OutputFile,"%s-%s-%-.4f %12.6f %12.6f %12.6f L %s-%s\n",
                            p_symbol,type,charge,Coordinates.GetPosition(i).x,
                            Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z,
                            (const char*)LowLinkAtomName,type);
                } else {
                    fprintf(OutputFile,"%s-%s-%-.4f %12.6f %12.6f %12.6f L\n",
                            p_symbol,type,charge,Coordinates.GetPosition(i).x,
                            Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
                }
            }
        } else {
            fprintf(OutputFile,"%s-%s-%07.4f %12.6f %12.6f %12.6f\n",
                    p_symbol,type,charge,Coordinates.GetPosition(i).x,
                    Coordinates.GetPosition(i).y,Coordinates.GetPosition(i).z);
        }
    }
    fprintf(OutputFile,"\n");

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteConnect(void)
{

    CSimpleList<CAmberAtom>* p_atom_bonds = new CSimpleList<CAmberAtom>[Topology.AtomList.GetNumberOfAtoms()];

    if( p_atom_bonds == NULL ) return(false);

    for(int i=0; i < Topology.BondList.GetNumberOfBonds(); i++) {
        CAmberBond* p_bond = Topology.BondList.GetBond(i);
        int ib = p_bond->GetIB();
        int jb = p_bond->GetJB();
        CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(ib);
        CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(jb);

        p_atom_bonds[ib].InsertToEnd(p_atom2);
        p_atom_bonds[jb].InsertToEnd(p_atom1);
    }

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++ ) {
        CSimpleIterator<CAmberAtom> I(p_atom_bonds[i]);
        CAmberAtom* p_second_atom;

        fprintf(OutputFile,"% 5d ",i+1);
        while( (p_second_atom = I.Current()) != NULL ) {
            fprintf(OutputFile,"% 5d 1.0 ",p_second_atom->GetAtomIndex()+1);
            I++;
        }
        fprintf(OutputFile,"\n");
    }

    fprintf(OutputFile,"\n");

    delete[] p_atom_bonds;

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteTypes(void)
{
    if( WriteBondTypes() == false ) return(false);

    if( WriteAngleTypes() == false ) return(false);

    if( WriteDihedralTypes() == false ) return(false);

    if( WriteImproperDihedralTypes() == false ) return(false);

    if( WriteNBTypes() == false ) return(false);

    fprintf(OutputFile,"\n");

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteBondTypes(void)
{
    CBondMMType* BondMMTypes = new CBondMMType[Topology.BondList.GetNumberOfBondTypes()];

    if( BondMMTypes == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to allocate memory in WriteBondTypes !\n");
        fprintf(stderr,"\n");
        return(false);
    }

    for(int i=0; i < Topology.BondList.GetNumberOfBonds(); i++) {
        CAmberBond* p_bond = Topology.BondList.GetBond(i);
        int ib = p_bond->GetIB();
        int jb = p_bond->GetJB();
        CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(ib);
        CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(jb);

        int icb = p_bond->GetICB();
        CAmberBondType* p_bond_type = Topology.BondList.GetBondType(icb);

        if( BondMMTypes[icb].BondType == NULL ) {
            BondMMTypes[icb].BondType = p_bond_type;
            strncpy(BondMMTypes[icb].AType,p_atom1->GetType(),4);
            strncpy(BondMMTypes[icb].BType,p_atom2->GetType(),4);

            double force_k = 0.0;

            if ( CalcEb == true ) {
                force_k = p_bond_type->GetRK();
            }

            fprintf(OutputFile,"HrmStr1    %4s %4s      %10.3f %10.5f\n",
                    TypeMap[BondMMTypes[icb].AType].GetBuffer(),
                    TypeMap[BondMMTypes[icb].BType].GetBuffer(),
                    force_k,p_bond_type->GetREQ());
        } else {
            if( ! ( ( (strncmp(BondMMTypes[icb].AType,p_atom1->GetType(),4) == 0) &&
                      (strncmp(BondMMTypes[icb].BType,p_atom2->GetType(),4) == 0) ) ||
                    ( (strncmp(BondMMTypes[icb].AType,p_atom2->GetType(),4) == 0) &&
                      (strncmp(BondMMTypes[icb].BType,p_atom1->GetType(),4) == 0) ) ) ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Bond violation in WriteBondTypes !\n");
                fprintf(stderr,"\n");
                delete[] BondMMTypes;
                BondMMTypes = NULL;
                return(false);
            }
        }
    }

    delete[] BondMMTypes;
    BondMMTypes = NULL;

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteAngleTypes(void)
{
    CAngleMMType* AngleMMTypes = new CAngleMMType[Topology.AngleList.GetNumberOfAngleTypes()];

    if( AngleMMTypes == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to allocate memory in WriteAngleTypes !\n");
        fprintf(stderr,"\n");
        return(false);
    }

    for(int i=0; i < Topology.AngleList.GetNumberOfAngles(); i++) {
        CAmberAngle* p_angle = Topology.AngleList.GetAngle(i);
        int it = p_angle->GetIT();
        int jt = p_angle->GetJT();
        int kt = p_angle->GetKT();
        CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(it);
        CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(jt);
        CAmberAtom* p_atom3 = Topology.AtomList.GetAtom(kt);

        int ict = p_angle->GetICT();
        CAmberAngleType* p_angle_type = Topology.AngleList.GetAngleType(ict);

        if( AngleMMTypes[ict].AngleType == NULL ) {
            AngleMMTypes[ict].AngleType = p_angle_type;
            strncpy(AngleMMTypes[ict].AType,p_atom1->GetType(),4);
            strncpy(AngleMMTypes[ict].BType,p_atom2->GetType(),4);
            strncpy(AngleMMTypes[ict].CType,p_atom3->GetType(),4);

            double force_k = 0.0;
            if ( CalcEa == true ) {
                force_k = p_angle_type->GetTK();
            }

            fprintf(OutputFile,"HrmBnd1    %4s %4s %4s %10.3f %10.5f\n",
                    TypeMap[AngleMMTypes[ict].AType].GetBuffer(),TypeMap[AngleMMTypes[ict].BType].GetBuffer(),
                    TypeMap[AngleMMTypes[ict].CType].GetBuffer(),
                    force_k,p_angle_type->GetTEQ()*180.0/M_PI);
        } else {
            if( ! (
                        (strncmp(AngleMMTypes[ict].BType,p_atom2->GetType(),4) == 0) &&
                        (
                            (
                                (strncmp(AngleMMTypes[ict].AType,p_atom1->GetType(),4) == 0) &&
                                (strncmp(AngleMMTypes[ict].CType,p_atom3->GetType(),4) == 0)
                            ) ||
                            (
                                (strncmp(AngleMMTypes[ict].AType,p_atom3->GetType(),4) == 0) &&
                                (strncmp(AngleMMTypes[ict].CType,p_atom1->GetType(),4) == 0)
                            )
                        )
                    )
              ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Angle violation in WriteAngleTypes !\n");
                fprintf(stderr,"\n");
                delete[] AngleMMTypes;
                AngleMMTypes = NULL;
                return(false);
            }
        }
    }
    // print angle bends for water
    if( PrintHrmBndParamsForWater == true ) {
        fprintf(OutputFile,"HrmBnd1    HW   OW   HW       0.000     0.0000\n");
        fprintf(OutputFile,"HrmBnd1    HW   HW   OW       0.000     0.0000\n");
    }

    delete[] AngleMMTypes;
    AngleMMTypes = NULL;

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteDihedralTypes(void)
{
    CSimpleList<CDihedralMMType> DihedralMMType;

    for(int i=0; i < Topology.DihedralList.GetNumberOfDihedrals(); i++) {
        CAmberDihedral* p_dihedral = Topology.DihedralList.GetDihedral(i);
        int ip = p_dihedral->GetIP();
        int jp = p_dihedral->GetJP();
        int kp = p_dihedral->GetKP();
        int lp = p_dihedral->GetLP();

        // only normal and terminal dihedrals are processed
        if( (p_dihedral->GetType() != 0)&&(p_dihedral->GetType() != -1) ) continue;

        CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(ip);
        CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(jp);
        CAmberAtom* p_atom3 = Topology.AtomList.GetAtom(kp);
        CAmberAtom* p_atom4 = Topology.AtomList.GetAtom(lp);

        // find type
        CSimpleIterator<CDihedralMMType> D(DihedralMMType);
        CDihedralMMType* p_type;

        bool found = false;

        while( (p_type = D.Current()) != NULL ) {
            if( (
                        (strncmp(p_type->AType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->BType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->DType,p_atom4->GetType(),4) == 0)
                    ) ||
                    (
                        (strncmp(p_type->DType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->BType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->AType,p_atom4->GetType(),4) == 0)
                    ) ) {
                found = true;
                break;
            }
            D++;
        }

        int icp = p_dihedral->GetICP();
        CAmberDihedralType* p_dihedral_type = Topology.DihedralList.GetDihedralType(icp);

        if( found == false ) {
            // register type
            p_type = new CDihedralMMType;
            if( p_type == NULL ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Unable to allocate memory in WriteDihedralTypes !\n");
                fprintf(stderr,"\n");
                return(false);
            }
            int pn = (int)(p_dihedral_type->GetPN()-1);
            p_type->DihedralType[pn] = p_dihedral_type;
            strncpy(p_type->AType,p_atom1->GetType(),4);
            strncpy(p_type->BType,p_atom2->GetType(),4);
            strncpy(p_type->CType,p_atom3->GetType(),4);
            strncpy(p_type->DType,p_atom4->GetType(),4);
            DihedralMMType.InsertToEnd(p_type,0,true);
        } else {
            int pn = (int)(p_dihedral_type->GetPN()-1);
            if( (pn < 0) || (pn > 3) ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: PN is out of legal range in WriteDihedralTypes !\n");
                fprintf(stderr,"\n");
                return(false);
            }
            if( p_type->DihedralType[pn] == NULL ) {
                p_type->DihedralType[pn] = p_dihedral_type;
            }
            if( p_type->DihedralType[pn] != p_dihedral_type ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Dihedral violation in WriteDihedralTypes !\n");
                fprintf(stderr,"\n");
                return(false);
            }
        }
    }

    CSimpleIterator<CDihedralMMType> D(DihedralMMType);
    CDihedralMMType* p_type;

    while( (p_type = D.Current()) != NULL ) {
        double force_k1 = 0.0;
        double force_k2 = 0.0;
        double force_k3 = 0.0;
        double force_k4 = 0.0;

        if ( CalcEd == true ) {
            if( p_type->DihedralType[0] != NULL ) {
                force_k1 = p_type->DihedralType[0]->GetPK();
            }
            if( p_type->DihedralType[1] != NULL ) {
                force_k2 = p_type->DihedralType[1]->GetPK();
            }
            if( p_type->DihedralType[2] != NULL ) {
                force_k3 = p_type->DihedralType[2]->GetPK();
            }
            if( p_type->DihedralType[3] != NULL ) {
                force_k4 = p_type->DihedralType[3]->GetPK();
            }
        }

        int phase1 = 0;
        int phase2 = 0;
        int phase3 = 0;
        int phase4 = 0;

        if( p_type->DihedralType[0] != NULL ) {
            phase1 = (int)(180.0*p_type->DihedralType[0]->GetPHASE()/M_PI);
        }
        if( p_type->DihedralType[1] != NULL ) {
            phase2 = (int)(180.0*p_type->DihedralType[1]->GetPHASE()/M_PI);
        }
        if( p_type->DihedralType[2] != NULL ) {
            phase3 = (int)(180.0*p_type->DihedralType[2]->GetPHASE()/M_PI);
        }
        if( p_type->DihedralType[3] != NULL ) {
            phase4 = (int)(180.0*p_type->DihedralType[3]->GetPHASE()/M_PI);
        }

        fprintf(OutputFile,"AmbTrs    %4s %4s %4s %4s %3d %3d"
                " %3d %3d %14.7f %14.7f %14.7f %14.7f 1.0 \n",
                TypeMap[p_type->AType].GetBuffer(),TypeMap[p_type->BType].GetBuffer(),TypeMap[p_type->CType].GetBuffer(),TypeMap[p_type->DType].GetBuffer(),
                phase1,phase2,phase3,phase4,force_k1,force_k2,force_k3,force_k4);
        D++;
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteImproperDihedralTypes(void)
{
    CSimpleList<CImproperDihedralMMType> ImproperDihedralMMType;

    for(int i=0; i < Topology.DihedralList.GetNumberOfDihedrals(); i++) {
        CAmberDihedral* p_dihedral = Topology.DihedralList.GetDihedral(i);
        int ip = p_dihedral->GetIP();
        int jp = p_dihedral->GetJP();
        int kp = p_dihedral->GetKP();
        int lp = p_dihedral->GetLP();

        // only improper and terminal improper dihedrals are processed
        if( (p_dihedral->GetType() != 1)&&(p_dihedral->GetType() != -2) ) continue;

        CAmberAtom* p_atom1 = Topology.AtomList.GetAtom(ip);
        CAmberAtom* p_atom2 = Topology.AtomList.GetAtom(jp);
        CAmberAtom* p_atom3 = Topology.AtomList.GetAtom(kp);
        CAmberAtom* p_atom4 = Topology.AtomList.GetAtom(lp);

        // find type
        CSimpleIterator<CImproperDihedralMMType> D(ImproperDihedralMMType);
        CImproperDihedralMMType* p_type;

        bool found = false;

        while( (p_type = D.Current()) != NULL ) {
            if( (
                        (strncmp(p_type->AType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->BType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->DType,p_atom4->GetType(),4) == 0)
                    ) ||
                    (
                        (strncmp(p_type->DType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->BType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->AType,p_atom4->GetType(),4) == 0)
                    )||
                    (
                        (strncmp(p_type->DType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->BType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->AType,p_atom4->GetType(),4) == 0)
                    )||
                    (
                        (strncmp(p_type->BType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->DType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->AType,p_atom4->GetType(),4) == 0)
                    ) ||
                    (
                        (strncmp(p_type->BType,p_atom1->GetType(),4) == 0) &&
                        (strncmp(p_type->AType,p_atom2->GetType(),4) == 0) &&
                        (strncmp(p_type->CType,p_atom3->GetType(),4) == 0) &&
                        (strncmp(p_type->DType,p_atom4->GetType(),4) == 0)
                    )) {
                found = true;
                break;
            }
            D++;
        }

        int icp = p_dihedral->GetICP();
        CAmberDihedralType* p_dihedral_type = Topology.DihedralList.GetDihedralType(icp);

        if( found == false ) {
            // register type
            p_type = new CImproperDihedralMMType;
            if( p_type == NULL ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Unable to allocate memory in WriteImproperDihedralTypes !\n");
                fprintf(stderr,"\n");
                return(false);
            }
            p_type->DihedralType = p_dihedral_type;
            strncpy(p_type->AType,p_atom1->GetType(),4);
            strncpy(p_type->BType,p_atom2->GetType(),4);
            strncpy(p_type->CType,p_atom3->GetType(),4);
            strncpy(p_type->DType,p_atom4->GetType(),4);
            ImproperDihedralMMType.InsertToEnd(p_type,0,true);
        } else {
            if( p_type->DihedralType != p_dihedral_type ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Improper dihedral violation in WriteImproperDihedralTypes !\n");
                fprintf(stderr,"\n");
                return(false);
            }
        }
    }

    CSimpleIterator<CImproperDihedralMMType> D(ImproperDihedralMMType);
    CImproperDihedralMMType* p_type;

    while( (p_type = D.Current()) != NULL ) {

        double force_k1 = 0.0;
        if ( CalcEd == true ) {
            force_k1 = p_type->DihedralType->GetPK();
        }

        double phase1 = 0;
        phase1 = (int)(180.0*p_type->DihedralType->GetPHASE()/M_PI);

        double pn1 = 0;
        pn1 = p_type->DihedralType->GetPN();

        fprintf(OutputFile,"ImpTrs    %4s %4s %4s %4s %14.7f %6.2f %3.1f\n",
                TypeMap[p_type->AType].GetBuffer(),TypeMap[p_type->BType].GetBuffer(),TypeMap[p_type->CType].GetBuffer(),TypeMap[p_type->DType].GetBuffer(),
                force_k1,phase1,pn1);
        D++;
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CTopCrd2MMCom::WriteNBTypes(void)
{
    CSimpleList<const char> Types;

    // go through atom list

    for(int i=0; i < Topology.AtomList.GetNumberOfAtoms(); i++) {

        CAmberAtom* p_atom = Topology.AtomList.GetAtom(i);

        // find type
        CSimpleIterator<const char> T(Types);
        const char* p_type;
        bool found = false;
        while((p_type = T.Current()) != NULL) {
            if( strcmp(p_type,p_atom->GetType()) == 0 ) {
                found = true;
                break;
            }
            T++;
        }
        if( found == false ) {
            if( Topology.NonBondedList.GetNonBondedType(p_atom,p_atom) < 0 ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: 10-12 potential is not supported !\n");
                fprintf(stderr,"\n");
                return(false);
            }

            double a,b,eps,rii;

            a = Topology.NonBondedList.GetAParam(p_atom,p_atom);
            b = Topology.NonBondedList.GetBParam(p_atom,p_atom);

            eps = 0.0;
            rii = 0.0;
            if( a != 0.0 ) {
                if (CalcEvdw == true) eps = b*b / (4.0 * a);
                rii = pow(2*a/b,1.0/6.0) * 0.5;
            }

            fprintf(OutputFile,"VdW %s %14.7f %14.7f\n",TypeMap[p_atom->GetType()].GetBuffer(),rii,eps);
            Types.InsertToEnd(p_atom->GetType());
        }
    }

    fprintf(OutputFile,"NonBon 3 1 0 0  0.0000000  0.0000000 "
            "%10.7f  0.0000000  0.0000000 %10.7f\n",
            1.0/Options.GetOptSCNB(),1.0/Options.GetOptSCEE());

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CBondMMType::CBondMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    BondType=NULL;
}

//---------------------------------------------------------------------------

CAngleMMType::CAngleMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    memset(CType,0,5);
    AngleType=NULL;
}

//---------------------------------------------------------------------------

CDihedralMMType::CDihedralMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    memset(CType,0,5);
    memset(DType,0,5);
    for(int i=0; i<4; i++) DihedralType[i] = NULL;
}

//---------------------------------------------------------------------------

CImproperDihedralMMType::CImproperDihedralMMType(void)
{
    memset(AType,0,5);
    memset(BType,0,5);
    memset(CType,0,5);
    memset(DType,0,5);
    DihedralType = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


