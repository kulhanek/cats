#ifndef QCATsScriptableH
#define QCATsScriptableH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2013 Petr Kulhanek, kulhanek@chemi.muni.cz
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

#include <QScriptEngine>
#include <QScriptValue>
#include <SmallString.hpp>
#include <vector>

//------------------------------------------------------------------------------

/// helper class
class QCATsScriptable {
public:
    /// init object
    QCATsScriptable(const QString& classname);

// number of arguments ---------------------------------------------------------
    /// check if the correct number of arguments was provided
    QScriptValue CheckNumberOfArguments(int low);

    /// check if the correct number of arguments was provided
    QScriptValue CheckNumberOfArguments(const QString& args,int low);

    /// check if the correct number of arguments was provided
    QScriptValue CheckNumberOfArguments(int low,int high);

    /// check if the correct number of arguments was provided
    QScriptValue CheckNumberOfArguments(const QString& args,int low,int high);

    /// check if the correct number of arguments was provided
    QScriptValue CheckMinimumNumberOfArguments(int nminargs);

    /// check if the correct number of arguments was provided
    QScriptValue CheckMinimumNumberOfArguments(const QString& args,int nminargs);

// argument retrieval ----------------------------------------------------------
    /// return number of argument associated with current context
    int GetArgumentCount(void);

    /// return give argument - idx counted from 1
    QScriptValue GetArgument(int idx);

    /// is argument string - idx counted from 1
    bool IsArgumentString(int idx);

    /// get argument as string - idx counted from 1
    QScriptValue GetArgAsString(const QString& args,const QString& arg,int idx,QString& value);

    /// is argument int - idx counted from 1
    bool IsArgumentInt(int idx);

    /// get argument as int - idx counted from 1
    QScriptValue GetArgAsInt(const QString& args,const QString& arg,int idx,int& value);

    /// is argument real number - idx counted from 1
    bool IsArgumentRNumber(int idx);

    /// get argument as real number - idx counted from 1
    QScriptValue GetArgAsRNumber(const QString& args,const QString& arg,int idx,double& value);

    /// is argument object - idx counted from 1
    template <class object>
    bool IsArgumentObject(int idx);

    /// get argument as object - idx counted from 1
    template <class object>
    QScriptValue GetArgAsObject(const QString& args,const QString& arg,const QString& type,int idx,object& value);

    /// find argument as object - find first occurence
    template <class object>
    QScriptValue FindArgAsObject(const QString& args,const QString& type,object& value,bool error=true);

    /// is key present
    bool IsArgumentKeySelected(const QString& key);

    /// check if all arguments were analyzed
    QScriptValue CheckArgumentsUsage(const QString& args);

    /// wrapper for read only properties
    QScriptValue  setIsNotAllowed(const QScriptValue& value);

// -----------------------------------------------------------------------------
    /// return name of the method
    QString GetMethodName(const QString& args);

    /// is help requested
    bool IsHelpRequested(void);

    /// is called as constructor
    QScriptValue IsCalledAsConstructor(void);

    /// get undefined value
    QScriptValue GetUndefinedValue(void);

    /// throw error
    QScriptValue ThrowError(const QString& args,const char* p_error);

    /// throw error
    QScriptValue ThrowError(const QString& args,const CSmallString& error);

    /// throw error
    QScriptValue ThrowError(const QString& args,const QString& error);

// -----------------------------------------------------------------------------
private:
    QString ClassName;

    /// return actual script context
    QScriptContext* Context(void);

    std::map<int,bool>   ArgUsageFlags;

public:
    static QScriptEngine* CATsEngine;
};

//------------------------------------------------------------------------------

template <class object>
bool QCATsScriptable::IsArgumentObject(int idx)
{
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isObject() == false ) return(false);
    QObject* p_qobj = svalue.toQObject();
    object p_obj = dynamic_cast<object>(p_qobj);
    return( p_obj != NULL );
}

//------------------------------------------------------------------------------

template <class object>
QScriptValue QCATsScriptable::GetArgAsObject(const QString& args,const QString& arg,const QString& type,int idx,object& value)
{
    int count = GetArgumentCount();
    if( (idx > count) || (idx < 1) ){
        return( ThrowError(args,"argument is out-of-legal range - internal bug") );
    }

    value = NULL;
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isObject() == true ){
        QObject* p_qobj = svalue.toQObject();
        object p_obj = dynamic_cast<object>(p_qobj);
        if( p_obj != NULL ){
            ArgUsageFlags[idx] = true;
            value = p_obj;
            return(true);
        }
    }

    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString error;
    error = GetMethodName(args) + " - ";
    error += " argument '" + arg + "' must be of " + type + " type";

    return( Context()->throwError(error) );
}

//------------------------------------------------------------------------------

template <class object>
QScriptValue QCATsScriptable::FindArgAsObject(const QString& args,const QString& type,object& value,bool error)
{
    for(int idx = 1; idx <= GetArgumentCount(); idx++){
        value = NULL;
        QScriptValue svalue = GetArgument(idx);
        if( svalue.isObject() == true ){
            QObject* p_qobj = svalue.toQObject();
            object p_obj = dynamic_cast<object>(p_qobj);
            if( p_obj != NULL ){
                ArgUsageFlags[idx] = true;
                value = p_obj;
                return(true);
            }
        }
    }

    if( ! error ) return(true); // do not report error

    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString serror;
    serror = GetMethodName(args) + " - ";
    serror += " one argument must be of " + type + " type";

    return( Context()->throwError(serror) );
}

//------------------------------------------------------------------------------

#endif
