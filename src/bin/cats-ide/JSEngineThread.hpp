#ifndef JSENGINETHREAD_HPP
#define JSENGINETHREAD_HPP
#include <QScriptEngineDebugger>
#include <fstream>
#include <iostream>
#include <QCATs.hpp>
#include <qthread.h>
#include <QPlainTextEdit>


class CJSEngineThread : public QThread
{
private:
    QString         JSCode;
    QScriptEngine   JSEngine;
    QScriptValue    Result;
    virtual void run();
public:
    CJSEngineThread(QObject *parent);
    void RunCode(const QString &code);
    QScriptValue GetResult();
};

#endif // JSENGINETHREAD_HPP
