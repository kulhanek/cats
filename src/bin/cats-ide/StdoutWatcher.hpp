#ifndef STDOUTWATCHER_HPP
#define STDOUTWATCHER_HPP
/* =====================================================================
 * This file is part of CATs - Conversion and Analysis Tools.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * CATs developed by: Petr Kulhánek, kulhanek@chemi.muni.cz
 * CATs IDE developed by: Jaroslav Oľha, jaroslav.olha@gmail.com
 * =====================================================================
 */

#include <iostream>
#include <streambuf>
#include <string>
#include <QTextStream>
#include <QFile>
#include <QThread>

class CStdoutWatcher : public QThread
{
    Q_OBJECT

public:
    CStdoutWatcher(QObject *parent, QString fileName);

    //Redirects stdout to a temporary file and launches the thread.
    void StartOutputRedirection();

    //Restores stdout.
    void StopOutputRedirection();

    //The flag that forces the thread to break its endless cycle and exit.
    bool Terminated;

signals:
    //The signal used to send every new line of output to the main window.
    void LineRead(QString line);

private:
    //The path to the temporary file used for output redirection.
    QString RedirectFileName;

    //The thread that continuously reads the temporary output file and sends each line to the main window.
    virtual void run();
};


#endif // STDOUTWATCHER_HPP
