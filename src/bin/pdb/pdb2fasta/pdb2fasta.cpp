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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <SmallString.hpp>
#include <ResidueMaps.hpp>

#include "pdb2fasta-opts.hpp"

//---------------------------------------------------------------------------

CPDB2FASTAOpts  Options;

FILE*           InputFile = NULL;           // input file
bool            OwnInputFile = false;       // do we own input file handle?
FILE*           OutputFile = NULL;          // output file
bool            OwnOutputFile = false;      // do we own output file handle?

int             FirstNameIndex = 0;

//---------------------------------------------------------------------------

bool ConvertFile(void);

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int main(int argc, char* argv[])
{
    int oresult;

    // encode program options, all check procedures are done inside of CPDB2FASTAOpts
    if( (oresult = Options.ParseCmdLine(argc,argv)) != SO_CONTINUE) return(oresult);

    // open files -----------------------------------
    if( Options.GetArgInput() == "-" ) {
        InputFile = stdin;
        OwnInputFile = false;
    } else {
        InputFile = fopen(Options.GetArgInput(),"r");
        if( InputFile == NULL ) {
            fprintf(stderr,"%s: unable to open input file '%s'\n",
                    (char*)Options.GetProgramName(),(const char*)Options.GetArgInput());
            return(SO_USER_ERROR);
        }
        OwnInputFile = true;
    }

    if( Options.GetArgOutput() == "-" ) {
        OutputFile = stdout;
        OwnOutputFile = false;
    } else {
        OutputFile = fopen(Options.GetArgOutput(),"w");
        if( OutputFile == NULL ) {
            fprintf(stderr,"%s: unable to open output file '%s'\n",
                    (char*)Options.GetProgramName(),(const char*)Options.GetArgOutput());
            if( (OwnInputFile == true) && (InputFile != NULL) ) {
                fclose(InputFile);
                InputFile = NULL;
                OwnInputFile = false;
            }
            return(SO_USER_ERROR);
        }
        OwnOutputFile = true;
    }

    // convert all given files
    bool cresult;

    cresult = ConvertFile();

    // close files if they are own by program
    if( (OwnInputFile == true) && (InputFile != NULL) ) {
        fclose(InputFile);
        InputFile = NULL;
        OwnInputFile = false;
    }
    if( (OwnOutputFile == true) && (OutputFile != NULL) ) {
        fclose(OutputFile);
        OutputFile = NULL;
        OwnOutputFile = false;
    }

    if( cresult == false ) return(1);

    return(0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool ConvertFile(void)
{
    char               LineBuffer[82];
    CSmallString       last_resname;
    int                last_resid=-1;
    char               last_chain_id = '.';
    int                counter=0;

    memset(LineBuffer,0,82);

    // read lines until end-of-file is reached
    while( fgets(LineBuffer,82,InputFile) != NULL ) {
        if( strstr(LineBuffer,"ENDMDL") == LineBuffer ) break;
        if( strstr(LineBuffer,"ATOM") == LineBuffer ) {
            // do magic with resname
            CSmallString    resname;
            resname.SetLength(3);
            resname.GetBuffer()[0] = LineBuffer[17];
            resname.GetBuffer()[1] = LineBuffer[18];
            resname.GetBuffer()[2] = LineBuffer[19];

            char chain_id;
            chain_id = LineBuffer[21];

            int res_id;
            sscanf(&LineBuffer[22]," %d",&res_id);

            if( (res_id != last_resid) || (last_chain_id != chain_id) ) {
                if( last_chain_id != chain_id ) {
                    if( last_resid != -1 ) fprintf(OutputFile,"\n");
                    fprintf(OutputFile,">%s:%c\n",(const char*)Options.GetArgInput(),chain_id);
                    last_chain_id = chain_id;
                    last_resid = res_id;
                    counter = 0;
                }
                if( last_resid == -1 ) last_resid = 0;
                for(int i=last_resid+1; i < res_id; i++) {
                    fprintf(OutputFile,"-");
                    counter++;
                    if( counter % 80 == 0 ) printf("\n");
                }
                if( ConvertResnameToFASTA(resname) == false ) {
                    return(false);
                }
                fprintf(OutputFile,"%1s",(const char*)resname);
                counter++;
                if( counter % 80 == 0 ) fprintf(OutputFile,"\n");
                last_resid = res_id;
            }
        }
    }

    if( counter % 80 != 0 ) fprintf(OutputFile,"\n");

    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
