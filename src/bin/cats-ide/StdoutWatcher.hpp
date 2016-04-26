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
 * CATs developed by: RNDr. Petr Kulhánek, PhD.
 * CATs IDE developed by: Mgr. Jaroslav Oľha
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
    void StartOutputRedirection();
    void StopOutputRedirection();
    bool Terminated;

signals:
    void LineRead(QString line);

private:
    QString RedirectFileName;
    virtual void run();
};


#endif // STDOUTWATCHER_HPP
