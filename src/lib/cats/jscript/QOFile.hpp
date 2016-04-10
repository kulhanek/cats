#ifndef QOFileH
#define QOFileH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2012 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <CATsMainHeader.hpp>
#include <QObject>
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <fstream>
#include <QCATsScriptable.hpp>

//------------------------------------------------------------------------------

/// support for formatted output

class CATS_PACKAGE QOFile : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QOFile(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// methods ---------------------------------------------------------------------
public slots:
    /// open file
    /// open(name)
    QScriptValue open(void);

    /// close file
    /// close()
    QScriptValue close(void);

    /// print to file
    /// printf(format,...)
    QScriptValue printf(void);

// section of private data -----------------------------------------------------
private:
    std::ofstream   vout;

    /// open file
    bool openInternal(const QString& name);

    friend class QTinySpline;
};

//------------------------------------------------------------------------------

#endif

