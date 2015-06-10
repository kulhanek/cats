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

#include <ErrorSystem.hpp>
#include <SmallTimeAndDate.hpp>
#include <SimpleList.hpp>
#include <ctype.h>
#include "Top2Params.hpp"
#include "MMTypes.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CTop2Params::CTop2Params(void)
{
    ParamsFile   = NULL;
}

//------------------------------------------------------------------------------

CTop2Params::~CTop2Params(void)
{
    CloseOutputFile();
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int CTop2Params::Init(int argc,char* argv[])
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
        printf("# top2params (CATs utility) started at %s\n",(const char*)dt.GetSDateAndTime());
        printf("# ==============================================================================\n");
        printf("#\n");
        if( Options.GetArgTopologyName() == "-" ) {
            printf("# Topology file name   : -stdin- (standard input)\n");
        } else {
            printf("# Topology file name   : %s\n",(const char*)Options.GetArgTopologyName());
        }
        if( Options.GetArgParamsName() == "-" ) {
            printf("# Parameters file name : -stdout- (standard output)\n");
        } else {
            printf("# Parameters file name : %s\n",(const char*)Options.GetArgParamsName());
        }
        printf("# ------------------------------------------------------------------------------\n");
    }

    return( result );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTop2Params::Run(void)
{
    // load topology
    if( LoadTopology() == false ) return(false);

    // open output files
    if( OpenOutputFile() == false ) return(false);

    bool result = true;

    // print charges
    result &= PrintParams();

    // close output files
    CloseOutputFile();

    return(result);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTop2Params::Finalize(void)
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

bool CTop2Params::LoadTopology(void)
{
    if( Topology.Load(Options.GetArgTopologyName(),true) == false ) {
        fprintf(stderr,"\n");
        fprintf(stderr,">>> ERROR: Unable to load specified topology: %s\n",
                (const char*)Options.GetArgTopologyName());
        return(false);
    }

    return(true);
}

//------------------------------------------------------------------------------

bool CTop2Params::OpenOutputFile(void)
{
    if( Options.GetArgParamsName() == "-" ) {
        ParamsFile = stdout;
    } else {
        if( (ParamsFile = fopen(Options.GetArgParamsName(),"w")) == NULL ) {
            fprintf(stderr,"\n");
            fprintf(stderr," ERROR: Unable to open output file for topology parameters '%s'!\n",(const char*)Options.GetArgParamsName());
            fprintf(stderr,"\n");
            return(false);
        }
    }

    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::PrintParams(void)
{
    fprintf(ParamsFile,"# parameter file generated with top2param from topology '%s'\n",(const char*)Options.GetArgTopologyName());

    if( WriteAtomTypes() == false ) return(false);

    if( WriteBondTypes() == false ) return(false);

    if( WriteAngleTypes() == false ) return(false);

    if( WriteDihedralTypes() == false ) return(false);

    if( WriteImproperDihedralTypes() == false ) return(false);

    if( WriteHBondTypes() == false ) return(false);

    if( WriteNBTypes() == false ) return(false);

    return(true);
}

//------------------------------------------------------------------------------

void CTop2Params::CloseOutputFile(void)
{
    if( Options.GetArgParamsName() != "-" ) {
        if( ParamsFile != NULL ) fclose(ParamsFile);
    }
    ParamsFile = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CTop2Params::WriteAtomTypes(void)
{
    CSimpleList<const char> Types;

    // go through atom list
    fprintf(ParamsFile,"MASS\n");

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
            fprintf(ParamsFile,"%s %7.3f %7.3f\n",p_atom->GetType(),p_atom->GetMass(),p_atom->GetPol());

            Types.InsertToEnd(p_atom->GetType());
        }
    }

    fprintf(ParamsFile,"\n");

    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::WriteBondTypes(void)
{
    CBondMMType* BondMMTypes = new CBondMMType[Topology.BondList.GetNumberOfBondTypes()];

    if( BondMMTypes == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to allocate memory in WriteBondTypes !\n");
        fprintf(stderr,"\n");
        return(false);
    }

    fprintf(ParamsFile,"BOND\n");

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
            force_k = p_bond_type->GetRK();

            char type1[5];
            type1[4]='\0';
            strncpy(type1,p_atom1->GetType(),4);
            for(int j=3; j > 1; j--) {
                if( isspace(type1[j]) != 0 ) type1[j] = '\0';
            }
            char type2[5];
            type2[4]='\0';
            strncpy(type2,p_atom2->GetType(),4);
            for(int j=3; j > 1; j--) {
                if( isspace(type2[j]) != 0 ) type2[j] = '\0';
            }

            fprintf(ParamsFile,"%2s-%2s    %10.3f %10.5f\n", type1,type2,force_k,p_bond_type->GetREQ());
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

    fprintf(ParamsFile,"\n");

    delete[] BondMMTypes;
    BondMMTypes = NULL;

    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::WriteAngleTypes(void)
{
    CAngleMMType* AngleMMTypes = new CAngleMMType[Topology.AngleList.GetNumberOfAngleTypes()];

    if( AngleMMTypes == NULL ) {
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to allocate memory in WriteAngleTypes !\n");
        fprintf(stderr,"\n");
        return(false);
    }

    fprintf(ParamsFile,"ANGL\n");

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
            force_k = p_angle_type->GetTK();

            char type1[5];
            type1[4]='\0';
            strncpy(type1,p_atom1->GetType(),4);
            for(int j=3; j > 1; j--) {
                if( isspace(type1[j]) != 0 ) type1[j] = '\0';
            }
            char type2[5];
            type2[4]='\0';
            strncpy(type2,p_atom2->GetType(),4);
            for(int j=3; j > 1; j--) {
                if( isspace(type2[j]) != 0 ) type2[j] = '\0';
            }
            char type3[5];
            type3[4]='\0';
            strncpy(type3,p_atom3->GetType(),4);
            for(int j=3; j > 1; j--) {
                if( isspace(type3[j]) != 0 ) type3[j] = '\0';
            }

            fprintf(ParamsFile,"%2s-%2s-%2s %10.3f %10.5f\n", type1,type2,type3,force_k,p_angle_type->GetTEQ()*180.0/M_PI);
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

    fprintf(ParamsFile,"\n");

    delete[] AngleMMTypes;
    AngleMMTypes = NULL;

    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::WriteDihedralTypes(void)
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
            if( (pn < 0) || (pn > 4) ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: PN %d is out of legal range <1,5> in WriteDihedralTypes!\n",pn+1);
                fprintf(stderr,"\n");
                return(false);
            }
            p_type->DihedralType[pn] = p_dihedral_type;
            strncpy(p_type->AType,p_atom1->GetType(),4);
            strncpy(p_type->BType,p_atom2->GetType(),4);
            strncpy(p_type->CType,p_atom3->GetType(),4);
            strncpy(p_type->DType,p_atom4->GetType(),4);
            DihedralMMType.InsertToEnd(p_type,0,true);
        } else {
            int pn = (int)(p_dihedral_type->GetPN()-1);
            if( (pn < 0) || (pn > 4) ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: PN %d is out of legal range <1,5> in WriteDihedralTypes!\n",pn+1);
                fprintf(stderr,"\n");
                return(false);
            }
            if( p_type->DihedralType[pn] == NULL ) {
                p_type->DihedralType[pn] = p_dihedral_type;
            }
            if( p_type->DihedralType[pn] != p_dihedral_type ) {
                fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Dihedral violation in WriteDihedralTypes!\n");
                fprintf(stderr,"\n");
                return(false);
            }
        }
    }

    fprintf(ParamsFile,"DIHE\n");

    CSimpleIterator<CDihedralMMType> D(DihedralMMType);
    CDihedralMMType* p_type;

    while( (p_type = D.Current()) != NULL ) {
        int chain_length = 0;

        for(int i=0; i<4; i++) {
            if( p_type->DihedralType[i] != NULL ) chain_length++;
        }

        char type1[5];
        type1[4]='\0';
        strncpy(type1,p_type->AType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type1[j]) != 0 ) type1[j] = '\0';
        }
        char type2[5];
        type2[4]='\0';
        strncpy(type2,p_type->BType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type2[j]) != 0 ) type2[j] = '\0';
        }
        char type3[5];
        type3[4]='\0';
        strncpy(type3,p_type->CType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type3[j]) != 0 ) type3[j] = '\0';
        }
        char type4[5];
        type4[4]='\0';
        strncpy(type4,p_type->DType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type4[j]) != 0 ) type4[j] = '\0';
        }

        for(int i=0; i<5; i++) {
            if( p_type->DihedralType[i] != NULL ) {
                double  force_k,phase,pn;
                force_k = p_type->DihedralType[i]->GetPK();
                phase = 180.0*p_type->DihedralType[i]->GetPHASE()/M_PI;
                pn = i+1;
                if( chain_length > 1 ) pn *= -1;
                fprintf(ParamsFile,"%2s-%2s-%2s-%2s    1 %14.7f   %7.3f   %4.1f\n",type1,type2,type3,type4,force_k,phase,pn);
                chain_length--;
            }
        }

        D++;
    }

    fprintf(ParamsFile,"\n");

    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::WriteImproperDihedralTypes(void)
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

    fprintf(ParamsFile,"IMPR\n");

    CSimpleIterator<CImproperDihedralMMType> D(ImproperDihedralMMType);
    CImproperDihedralMMType* p_type;

    while( (p_type = D.Current()) != NULL ) {

        double force_k1 = 0.0;
        force_k1 = p_type->DihedralType->GetPK();

        double phase1 = 0;
        phase1 = (int)(180.0*p_type->DihedralType->GetPHASE()/M_PI);

        double pn1 = 0;
        pn1 = p_type->DihedralType->GetPN();

        char type1[5];
        type1[4]='\0';
        strncpy(type1,p_type->AType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type1[j]) != 0 ) type1[j] = '\0';
        }
        char type2[5];
        type2[4]='\0';
        strncpy(type2,p_type->BType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type2[j]) != 0 ) type2[j] = '\0';
        }
        char type3[5];
        type3[4]='\0';
        strncpy(type3,p_type->CType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type3[j]) != 0 ) type3[j] = '\0';
        }
        char type4[5];
        type4[4]='\0';
        strncpy(type4,p_type->DType,4);
        for(int j=3; j > 1; j--) {
            if( isspace(type4[j]) != 0 ) type4[j] = '\0';
        }

        fprintf(ParamsFile,"%2s-%2s-%2s-%2s      %14.7f   %7.3f   %4.1f\n", type1,type2,type3,type4,force_k1,phase1,pn1);
        D++;
    }

    fprintf(ParamsFile,"\n");

    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::WriteHBondTypes(void)
{
    fprintf(ParamsFile,"HBON\n");
    fprintf(ParamsFile,"\n");
    return(true);
}

//---------------------------------------------------------------------------

bool CTop2Params::WriteNBTypes(void)
{
    CSimpleList<const char> Types;

    // go through atom list

    fprintf(ParamsFile,"NONB\n");

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
                eps = b*b / (4.0 * a);
                rii = pow(2*a/b,1.0/6.0) * 0.5;
            }

            fprintf(ParamsFile,"%s  %12.7f %12.7f\n",p_atom->GetType(),rii,eps);
            Types.InsertToEnd(p_atom->GetType());
        }
    }

    fprintf(ParamsFile,"\n");

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


