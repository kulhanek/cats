#ifndef QTinySplineH
#define QTinySplineH
// =============================================================================
// CATS - Conversion and Analysis Tools
// -----------------------------------------------------------------------------
//    Copyright (C) 2016 Petr Kulhanek, kulhanek@chemi.muni.cz
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
#include <QScriptValue>
#include <QScriptContext>
#include <QScriptable>
#include <QCATsScriptable.hpp>
#include <tinyspline/tinyspline.h>

//------------------------------------------------------------------------------

/// TinySpline
class CATS_PACKAGE QTinySpline : public QObject, protected QScriptable, protected QCATsScriptable {
Q_OBJECT
public:
// constructor -----------------------------------------------------------------
    QTinySpline(void);
    static QScriptValue New(QScriptContext *context,QScriptEngine *engine);
    static void Register(QScriptEngine& engine);

// properties ------------------------------------------------------------------
    Q_PROPERTY(QScriptValue degree          READ getDegree WRITE setDegree)
    Q_PROPERTY(QScriptValue dimension       READ getDimension WRITE setDimension)
    Q_PROPERTY(QScriptValue type            READ getType WRITE setType)
    Q_PROPERTY(QScriptValue numOfCtrlPoints READ getNumOfCtrlPoints WRITE setIsNotAllowed)    
 
// methods ---------------------------------------------------------------------
public slots:
    /// set spline degree
    /// void setDegree(degree)    
    QScriptValue setDegree(const QScriptValue& dummy);
    
    /// get spline degree
    /// int getDegree()    
    QScriptValue getDegree(void);
    
    /// set ctrl point dimension
    /// void setDimension(dimension)    
    QScriptValue setDimension(const QScriptValue& dummy);
    
    /// get ctrl point dimension
    /// int getDimension()    
    QScriptValue getDimension(void);    

    /// set spline type
    /// void setType(type)    
    QScriptValue setType(const QScriptValue& dummy);
    
    /// get spline type
    /// string getType()    
    QScriptValue getType(void);      
    
    /// init spline engine
    /// void initSpline(nctrlpts)
    QScriptValue initSpline(void);
    
    /// init spline from atom coordinates
    /// void initSplineFromAtoms(snapshot[,selection])
    QScriptValue initSplineFromAtoms(void);

    /// init spline from 3DNA BP origins
    /// void initSplineFrom3DNABPOrigins(x3dna)
    QScriptValue initSplineFrom3DNABPOrigins(void);

    /// init spline from 3DNA helical axis
    /// void initSplineFrom3DNAHelAxis(x3dna)
    QScriptValue initSplineFrom3DNAHelAxis(void);

    /// init spline from curves+ helical axis
    /// void initSplineFromCurvesPHelAxis(x3dna)
    QScriptValue initSplineFromCurvesPHelAxis(void);
    
    /// set control point
    /// bool setCtrlPointValue(index,value,dim)
    QScriptValue setCtrlPointValue(void);
    
    /// get number of control points
    /// int getNumOfCtrlPoints()    
    QScriptValue getNumOfCtrlPoints(void);

    /// get curvature
    /// double getCurvature(u)
    QScriptValue getCurvature(void);
        
    /// write geometry of curve
    /// writeXYZ(name/ofile[,density[,symbol]])
    QScriptValue writeXYZ(void);

    /// write curvature of curve
    /// writeCurvature(name/ofile[,density])
    QScriptValue writeCurvature(void);
    
// access methods --------------------------------------------------------------
public:   
    int             Degree;
    int             Dimension;
    tsBSplineType   Type;
    tsBSpline       SSpline;  // curve
    tsBSpline       DSpline;  // first derivative
    tsBSpline       TSpline;  // second derivative
};

//------------------------------------------------------------------------------

#endif
