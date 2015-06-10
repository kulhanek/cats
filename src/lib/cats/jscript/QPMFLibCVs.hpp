#ifndef QPMFLibCVsH
#define QPMFLibCVsH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
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
#include <AmberRestart.hpp>
#include <PMFLibCVs.hpp>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

class QTopology;
class QSnapshot;

//------------------------------------------------------------------------------

class QPMFLibCVs : public QObject, protected QScriptable, protected QCATsScriptable {
    Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QPMFLibCVs(QTopology* p_qtop);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------

// methods ---------------------------------------------------------------------
public slots:
    /// load PMFLib CV file
    bool load(const QString& name);

    /// get CV value
    double getValue(QObject* p_snapshot,const QString& name);

// access methods --------------------------------------------------------------
public:

// section of private data -----------------------------------------------------
private:
    CPMFLibCVs  PMFLibCVs;
};

//------------------------------------------------------------------------------

#endif
