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

#include <QCATsScriptable.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include <cmath>

//------------------------------------------------------------------------------

using namespace std;
using namespace boost;
using namespace boost::algorithm;

//------------------------------------------------------------------------------

QScriptEngine* QCATsScriptable::CATsEngine = NULL;

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QCATsScriptable::QCATsScriptable(const QString& classname)
{
    ClassName = classname;
}

//------------------------------------------------------------------------------

bool QCATsScriptable::IsHelpRequested(void)
{
    if( Context() ) {
        if( Context()->isCalledAsConstructor() == true ) return(false);
    }
    if( GetArgumentCount() != 1 ) return(false);
    QScriptValue value = GetArgument(1);
    return( value.toString() == "help" );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::IsCalledAsConstructor(void)
{
    if( ! Context() ) return(false);
    if( Context()->isCalledAsConstructor()  == false ){
        QString error;
        error = ClassName + "() - it must be called as a constructor";
        return( Context()->throwError(error) );
    }
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::GetUndefinedValue(void)
{
    return( CATsEngine->undefinedValue() );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckNumberOfArguments(int low)
{
    return( CheckNumberOfArguments(QString(),low,low) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckNumberOfArguments(const QString& args,int low)
{
    return( CheckNumberOfArguments(args,low,low) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckNumberOfArguments(int low,int high)
{
    return( CheckNumberOfArguments(QString(),low,high) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckNumberOfArguments(const QString& args,int low,int high)
{
    int count = GetArgumentCount();
    if( (count >= low) && (count <= high) ){
        // everything is OK
        return(QScriptValue(true));
    }
    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString error;
    error = GetMethodName(args) + " - ";
    if( low == high ){
        if( low == 0 ){
            error += "no argument is required";
        } else if (low == 1) {
            error += "only one argument is required";
        } else {
            error += QString().setNum(low) + " arguments are required";
        }
    } else {
        error += " arguments between " + QString().setNum(low) + " to " + QString().setNum(high) + " are required";
    }
    error += " but (" + QString().setNum(count) + ") arguments are provided";

    return( Context()->throwError(error) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckMinimumNumberOfArguments(int nminargs)
{
    return( CheckMinimumNumberOfArguments(QString(),nminargs) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckMinimumNumberOfArguments(const QString& args,int nminargs)
{
    int count = GetArgumentCount();
    if( count >= nminargs ){
        // everything is OK
        return(QScriptValue(true));
    }
    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString error;
    error = GetMethodName(args) + " - ";
    if( nminargs == 1 ) {
        error += " at least one argument is required";
    } else {
        error += " at least " + QString().setNum(nminargs) + " arguments are required";
    }
    error += " but only (" + QString().setNum(count) + ") arguments are provided";

    return( Context()->throwError(error) );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

int QCATsScriptable::GetArgumentCount(void)
{
    if( Context() == NULL ) return(0);
    return( Context()->argumentCount() );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::GetArgument(int idx)
{
    idx--;
    if( Context() == NULL ) return(QScriptValue());
    return( Context()->argument(idx) );
}

//------------------------------------------------------------------------------

bool QCATsScriptable::IsArgumentString(int idx)
{
    QScriptValue svalue = GetArgument(idx);
    return( svalue.isString() );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::GetArgAsString(const QString& args,const QString& arg,int idx,QString& value)
{
    int count = GetArgumentCount();
    if( (idx > count) || (idx < 1) ){
        return( ThrowError(args,"argument is out-of-legal range - internal bug") );
    }

    value.clear();
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isString() ){
        ArgUsageFlags[idx] = true;
        value = svalue.toString();
        return(svalue);
    }

    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString error;
    error = GetMethodName(args) + " - ";
    error += " argument '" + arg + "' must be of string type";

    return( Context()->throwError(error) );
}

//------------------------------------------------------------------------------

bool QCATsScriptable::IsArgumentInt(int idx)
{
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isNumber() == false ) return(false);

    // FIXME - is integer number?
    qsreal number = svalue.toNumber();
    double intnum;
    double fraction = modf(number,&intnum);
    return(fraction == 0.0);
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::GetArgAsInt(const QString& args,const QString& arg,int idx,int& value)
{
    int count = GetArgumentCount();
    if( (idx > count) || (idx < 1) ){
        return( ThrowError(args,"argument is out-of-legal range - internal bug") );
    }

    value = 0;
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isNumber() ){

        // FIXME - is integer number?
        qsreal number = svalue.toNumber();
        double intnum;
        double fraction = modf(number,&intnum);
        if( fraction == 0.0 ){
            ArgUsageFlags[idx] = true;
            value = intnum;
            return(svalue);
        }
    }

    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString error;
    error = GetMethodName(args) + " - ";
    error += " argument '" + arg + "' must be of integer type";

    return( Context()->throwError(error) );
}

//------------------------------------------------------------------------------

bool QCATsScriptable::IsArgumentRNumber(int idx)
{
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isNumber() == false ) return(false);
    return(true);
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::GetArgAsRNumber(const QString& args,const QString& arg,int idx,double& value)
{
    int count = GetArgumentCount();
    if( (idx > count) || (idx < 1) ){
        return( ThrowError(args,"argument is out-of-legal range - internal bug") );
    }

    value = 0;
    QScriptValue svalue = GetArgument(idx);
    if( svalue.isNumber() ){
        ArgUsageFlags[idx] = true;
        value = svalue.toNumber();
        return(svalue);
    }

    // construct error string
    if( Context() == NULL ){
        return(QScriptValue(false));
    }
    QString error;
    error = GetMethodName(args) + " - ";
    error += " argument '" + arg + "' must be of real number type";

    return( Context()->throwError(error) );
}

//------------------------------------------------------------------------------

bool QCATsScriptable::IsArgumentKeySelected(const QString& key)
{
    for(int i=1; i <= GetArgumentCount(); i++){
        QScriptValue value = GetArgument(i);
        if( value.isString() ){
            if( value.toString() == key ){
                ArgUsageFlags[i] = true;
                return(true);
            }
        }
    }
    return(false);
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::CheckArgumentsUsage(const QString& args)
{
    bool error = false;
    QString wargs;
    for(int i=1; i <= GetArgumentCount(); i++){
        if( ArgUsageFlags[i] == false ){
            if( error ) wargs += ",";
            wargs += QString().setNum(i);
            error = true;
        }
    }
    if( error ){
        QString error = "arguments " + wargs + " were not used";
        return(ThrowError(args,error));
    }
    return(true);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QString QCATsScriptable::GetMethodName(const QString& args)
{
    if( ! Context() )  return(QString());   // no context

    QString text;
    if( Context()->isCalledAsConstructor() == false ){
        // get function name
        string sfce = Context()->callee().toString().toStdString();
        vector<string> list;
        split(list,sfce,is_any_of(" ("));
        QString fce;
        if( list.size() > 2 ){
            fce = QString::fromStdString(list[1]);
        }
        text = ClassName + "::" + fce + "(" + args + ")";
    } else {
        text = ClassName + "(" + args + ")";
    }
    return(text);
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::ThrowError(const QString& args,const char* p_error)
{
    return( ThrowError(args,QString(p_error)) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::ThrowError(const QString& args,const CSmallString& error)
{
    return( ThrowError(args,QString(error)) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::ThrowError(const QString& args,const QString& error)
{
    if( ! Context() ) return( QScriptValue() );

    QString str;
    str = GetMethodName(args) + " - ";
    str += error;
    return( Context()->throwError(str) );
}

//------------------------------------------------------------------------------

QScriptValue QCATsScriptable::setIsNotAllowed(const QScriptValue& value)
{
    return(ThrowError("","this property is read only"));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QScriptContext* QCATsScriptable::Context(void)
{
    if( CATsEngine == NULL ) return(NULL);
    return( CATsEngine->currentContext() );
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================


