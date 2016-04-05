#ifndef STDOUTWATCHER_HPP
#define STDOUTWATCHER_HPP

#include <iostream>
#include <streambuf>
#include <string>

#include <QTextEdit>

class CStdoutWatcher : public std::basic_streambuf<char>
{
public:
    CStdoutWatcher(std::ostream &stream, QTextEdit* text_edit);
    ~CStdoutWatcher();
    static void registerQDebugMessageHandler();

private:
    static void myQDebugMessageHandler(QtMsgType, const QMessageLogContext &, const QString &msg);

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    QTextEdit* log_window;
};


#endif // STDOUTWATCHER_HPP
