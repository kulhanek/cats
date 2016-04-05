#include "StdoutWatcher.hpp"

CStdoutWatcher::CStdoutWatcher(std::ostream &stream, QTextEdit* text_edit) : m_stream(stream)
{
    log_window = text_edit;
    m_old_buf = stream.rdbuf();
    stream.rdbuf(this);
}

CStdoutWatcher::~CStdoutWatcher()
{
    m_stream.rdbuf(m_old_buf);
}
