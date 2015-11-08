#ifndef Qx3DNAH
#define Qx3DNAH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <QCATsScriptable.hpp>

#include <QTopology.hpp>
#include <QSnapshot.hpp>
#include <QSelection.hpp>

//------------------------------------------------------------------------------

/// 3D x3DNA

class Qx3DNA : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    Qx3DNA(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:

    /// perfomer core analysis
    QScriptValue analyze(void);

// access methods --------------------------------------------------------------
private:

    /// create PDB file from QSnapshot
    bool WritePDB(QTopology* p_qtop,QSnapshot* p_qsnap,QSelection* p_qsel,FILE* p_fout);

    /// write PDB remark
    bool WritePDBRemark(FILE* p_file,const CSmallString& string);

    /// get PDB res name
    const char* GetPDBResName(CAmberAtom* p_atom,CAmberResidue* p_res);

    /// get PDB atom name
    const char* GetPDBAtomName(CAmberAtom* p_atom,CAmberResidue* p_res);
};

//------------------------------------------------------------------------------

#endif
