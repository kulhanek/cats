// =============================================================================
// CATs - Conversion and Analysis Tools
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
#include <string.h>

#include <SmallString.hpp>
#include <ResidueMaps.hpp>

//---------------------------------------------------------------------------

const char* ResidueNameConversion[MAXRESLIST][2]= {
    {"ALA", "A"},
    {"ARG", "R"},
    {"ASN", "N"},
    {"ASP", "D"},
    {"CYS", "C"},
    {"CYX", "4"},
    {"GLU", "E"},
    {"GLN", "Q"},
    {"GLY", "G"},
    {"HIS", "H"},
    {"HID", "1"},
    {"HIE", "2"},
    {"HIP", "3"},
    {"ILE", "I"},
    {"LEU", "L"},
    {"LYS", "K"},
    {"MET", "M"},
    {"PHE", "F"},
    {"PRO", "P"},
    {"SER", "S"},
    {"THR", "T"},
    {"TRP", "W"},
    {"TYR", "Y"},
    {"VAL", "V"},
};

//---------------------------------------------------------------------------

const char* ResidueToFASTA[MAXRESLISTFASTA][2]= {
    {"---", "?"},
    {"ALA", "A"},
    {"CYS", "C"},
    {"CYX", "C"},
    {"ASP", "D"},
    {"GLU", "E"},
    {"PHE", "F"},
    {"GLY", "G"},
    {"HIS", "H"},
    {"HSD", "H"},
    {"HSE", "H"},
    {"HSP", "H"},
    {"HIE", "H"},
    {"HID", "H"},
    {"HIP", "H"},
    {"ILE", "I"},
    {"LYS", "K"},
    {"LEU", "L"},
    {"MET", "M"},
    {"ASN", "N"},
    {"PRO", "P"},
    {"GLN", "Q"},
    {"ARG", "R"},
    {"SER", "S"},
    {"THR", "T"},
    {"VAL", "V"},
    {"TRP", "W"},
    {"TYR", "Y"},
    {"NME", "-"},
    {"ACE", "-"},

    {"  A", "A"},
    {"  C", "C"},
    {"  G", "G"},
    {"  T", "T"},
    {"  U", "U"},

    {"DA ", "A"},
    {"DC ", "C"},
    {"DG ", "G"},
    {"DT ", "T"},

    {"DA5", "A"},
    {"DC5", "C"},
    {"DG5", "G"},
    {"DT5", "T"},

    {"DA3", "A"},
    {"DC3", "C"},
    {"DG3", "G"},
    {"DT3", "T"}
};

//---------------------------------------------------------------------------

bool ConvertResnameToNTerminal(CSmallString& resname,bool silent)
{
    // find residue
    bool found = false;
    int  index = 0;
    for(index=0; index < MAXRESLIST; index++) {
        if( strcmp(ResidueNameConversion[index][0],resname) == 0 ) {
            found = true;
            break;
        }
    }

    if( found == false ) {
        if( silent == true ) return(true);
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to translate name of N-terminal residue '%s'!\n",
                (const char*)resname);
        fprintf(stderr,"\n");
        return(false);
    }

    resname = "NT";
    resname += ResidueNameConversion[index][1];

    return(true);
}

//---------------------------------------------------------------------------

bool ConvertResnameToCTerminal(CSmallString& resname,bool silent)
{
    // find residue
    bool found = false;
    int  index = 0;
    for(index=0; index < MAXRESLIST; index++) {
        if( strcmp(ResidueNameConversion[index][0],resname) == 0 ) {
            found = true;
            break;
        }
    }

    if( found == false ) {
        if( silent == true ) return(true);
        fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to translate name of C-terminal residue '%s'!\n",
                (const char*)resname);
        fprintf(stderr,"\n");
        return(false);
    }

    resname = "CT";
    resname += ResidueNameConversion[index][1];

    return(true);
}

//---------------------------------------------------------------------------

bool ConvertNCResnameToResname(CSmallString& resname)
{
    if( resname.GetLength() != 3 ) return(true);

    if( (resname.GetBuffer()[0] == 'N') && (resname.GetBuffer()[1] == 'T') ) {
        // try as N terminal
        for(int index=0; index < MAXRESLIST; index++) {
            if( ResidueNameConversion[index][1][0] == resname.GetBuffer()[2] ) {
                resname = ResidueNameConversion[index][0];
                return(true);
            }
        }
    }

    // try as C terminal
    if( (resname.GetBuffer()[0] == 'C') && (resname.GetBuffer()[1] == 'T') ) {
        // try as C terminal
        for(int index=0; index < MAXRESLIST; index++) {
            if( ResidueNameConversion[index][1][0] == resname.GetBuffer()[2] ) {
                resname = ResidueNameConversion[index][0];
                return(true);
            }
        }
    }

    return(true);
}

//---------------------------------------------------------------------------

bool ConvertResnameToFASTA(CSmallString& resname)
{
    // find residue
    bool found = false;
    int  index = 0;
    for(index=0; index < MAXRESLISTFASTA; index++) {
        if( strcmp(ResidueToFASTA[index][0],resname) == 0 ) {
            found = true;
            break;
        }
    }

    if( found ) {
        resname = ResidueToFASTA[index][1];
    } else {
        resname = ResidueToFASTA[0][1];
    }
    return(true);
}

//---------------------------------------------------------------------------




