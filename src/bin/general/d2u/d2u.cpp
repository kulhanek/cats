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
#include "d2u-opts.hpp"

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool ProcessFile(const char* p_name);
bool ProcessStream(void);

bool Error = false;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int main(int argc, char* argv[])
{
    Cd2uOpts options;

    int oresult;

    // encode program options, all check procedures are done inside of Cd2uOpts
    if( (oresult = options.ParseCmdLine(argc,argv)) != SO_CONTINUE) return(oresult);

    // convert all given files
    bool cresult = true;

    for(int i=0; i < options.GetNumberOfProgArgs(); i++) {
        if( options.GetProgArg(i) != "-" ) {
            cresult &= ProcessFile(options.GetProgArg(i));
        } else {
            cresult &= ProcessStream();
        }
    }

    if( Error == true ) fprintf(stderr,"\n");

    if( cresult == false ) return(1);

    return(0);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool ProcessFile(const char* p_name)
{
    FILE* file;

    if( (file = fopen(p_name,"rb")) == NULL ) {
        if( Error == false) fprintf(stderr,"\n");
        fprintf(stderr,"ERROR: Can't open the file '%s' for reading!\n",p_name);
        Error = true;
        return(false);
    }

    fseek(file,0,SEEK_END);
    unsigned int  size = ftell(file);

    if( size == 0 ) return(true);  // file is empty

    char* buffer;

    try {
        buffer = new char[size];
    } catch(...) {
        if( Error == false) fprintf(stderr,"\n");
        fprintf(stderr,"ERROR: Unable to allocate memory for the file : '%s'!\n",p_name);
        Error = true;
        fclose(file);
        return(false);
    }

    fseek(file,0,SEEK_SET    );
    if( fread(buffer,sizeof(char),size,file) != size) {
        if( Error == false) fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Unable to load the file into memory : '%s'!\n",p_name);
        Error = true;
        delete[] buffer;
        fclose(file);
        return(false);
    }

    fclose(file);

    if( (file = fopen(p_name,"wb")) == NULL ) {
        if( Error == false) fprintf(stderr,"\n");
        fprintf(stderr," ERROR: Can't open the file '%s' for writing!\n",p_name);
        Error = true;
        return(false);
    }

    for(unsigned int i=0; i<size; i++) {
        if( buffer[i] != 0x0d ) {
            if( fputc(buffer[i],file) == EOF ) {
                if( Error == false) fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Can't write into the file '%s'\n",p_name);
                Error = true;
                return(false);
            }
        }
    }

    fclose(file);

    delete[] buffer;

    return(true);
}

//------------------------------------------------------------------------------

bool ProcessStream(void)
{
    char buffer;
    while( (buffer = fgetc(stdin)) != EOF ) {
        if( buffer != 0x0d ) {
            if( fputc(buffer,stdout) == EOF ) {
                if( Error == false) fprintf(stderr,"\n");
                fprintf(stderr," ERROR: Can't write into the output stream\n");
                Error = true;
                return(false);
            }
        }
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================
