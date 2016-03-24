#ifndef SCRIPTTHREAD_HPP
#define SCRIPTTHREAD_HPP
#include <QScriptEngineDebugger>

#include <QCATs.hpp>


class ScriptThread : public QThread
{
private:
    QScriptEngine JSEngine;
    QScriptValue result;
    QString JSCode;
    QTextEdit textBrowser;
    void run()
    {
        new Q_DebugStream(std::cout, &textBrowser); //Redirect Console output to QTextEdit
        Q_DebugStream::registerQDebugMessageHandler(); //Redirect qDebug() output to QTextEdit

        result = JSEngine.evaluate(JSCode);
        //sleep(5);
    }
public:
    void registerCATs()
    {
        // metoda registerCATs se stava zbytecnou, pouzijte rovnou RegisterAllCATsClasses()
        RegisterAllCATsClasses(JSEngine);
    }
    QString getOutput()
    {
        QString output = textBrowser.document()->toPlainText();
        textBrowser.clear();
        return output;
    }
    QScriptEngine* getEngine()
    {
        return &JSEngine;
    }
    void setCode(QString code)
    {
        JSCode = code;
    }
    QScriptValue getResult()
    {
        return result;
    }
};

#endif // SCRIPTTHREAD_HPP
