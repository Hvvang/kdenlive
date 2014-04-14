/***************************************************************************
 *   Copyright (C) 2011 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/


#include "widgets/abstractmonitor.h"
#include "kdenlivesettings.h"
#include "monitormanager.h"

#include <KDebug>

#include <QPaintEvent>
#include <QDesktopWidget>
#include <QVBoxLayout>


AbstractMonitor::AbstractMonitor(Kdenlive::MonitorId id, MonitorManager *manager, QGLWidget *glContext, QWidget *parent): 
    QWidget(parent)
    , m_id(id)
    , m_monitorManager(manager)
    , m_parentGLContext(glContext)
    , m_glWidget(NULL)
{
    videoBox = new QFrame(this);
}


AbstractMonitor::~AbstractMonitor()
{
    delete videoBox;
}

bool AbstractMonitor::isActive() const
{
    return m_monitorManager->isActive(m_id);
}

bool AbstractMonitor::slotActivateMonitor(bool forceRefresh)
{
    return m_monitorManager->activateMonitor(m_id, forceRefresh);
}


#include "abstractmonitor.moc"
