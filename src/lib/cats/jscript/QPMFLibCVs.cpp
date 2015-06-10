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

#include <iostream>
#include <QScriptEngine>
#include <QPMFLibCVs.hpp>
#include <QPMFLibCVs.moc>
#include <QTopology.hpp>
#include <QSnapshot.hpp>

using namespace std;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void QPMFLibCVs::Register(QScriptEngine& engine)
{
    QScriptValue ctor = engine.newFunction(QPMFLibCVs::New);
    QScriptValue metaObject = engine.newQMetaObject(&QPMFLibCVs::staticMetaObject, ctor);
    engine.globalObject().setProperty("PMFLibCVs", metaObject);
}

//------------------------------------------------------------------------------

QScriptValue QPMFLibCVs::New(QScriptContext *context,
                             QScriptEngine *engine)
{
    if( context->isCalledAsConstructor() == false ) {
        context->throwError("PMFLibCVs() can be called only as a constructor\nusage: new PMFLibCVs(topology)\n       new PMFLibCVs(topology,name)");
        return(engine->undefinedValue());
    }
    if( (context->argumentCount() > 2) || (context->argumentCount() < 1) ) {
        context->throwError("Illegal number of arguments\n"
                            "usage: new PMFLibCVs(topology)\n"
                            "       new PMFLibCVs(topology,snapshot)");
        return(engine->undefinedValue());
    }

    QTopology* p_qtop =  dynamic_cast<QTopology*>(context->argument(0).toQObject());
    if( p_qtop == NULL ) {
        context->throwError("First parameter is not Topology");
        return(engine->undefinedValue());
    }

    QPMFLibCVs* p_obj = new QPMFLibCVs(p_qtop);

    return(engine->newQObject(p_obj, QScriptEngine::ScriptOwnership));

}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QPMFLibCVs::QPMFLibCVs(QTopology* p_qtop)
    : QCATsScriptable("PMFLibCVs")
{
    PMFLibCVs.SetTopology(&p_qtop->Topology);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool QPMFLibCVs::load(const QString& name)
{
    return( PMFLibCVs.Read(name) );
}

//------------------------------------------------------------------------------

double QPMFLibCVs::getValue(QObject* p_snapshot,const QString& name)
{
    if( argumentCount() != 2 ) {
        context()->throwError("illegal number of arguments\nusage: PMFLibCVs::getValue(snapshot,name)");
        return(0.0);
    }
    QSnapshot* p_qsnap = dynamic_cast<QSnapshot*>(p_snapshot);
    if( p_qsnap == NULL ) {
        context()->throwError("illegal argument\nusage: PMFLibCVs::getValue(snapshot,name)");
        return(0.0);
    }
    double value = 0;
    bool result = PMFLibCVs.GetValue(name,&p_qsnap->Restart,value);
    if( result == false ){
        context()->throwError("unable to find specified CV");
        return(0.0);
    }

    return(value);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


