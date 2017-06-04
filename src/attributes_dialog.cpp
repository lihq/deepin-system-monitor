/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2017 Deepin, Inc.
 *               2011 ~ 2017 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#include "attributes_dialog.h"
#include <QPainter>
#include <dwindowclosebutton.h>
#include <proc/sysinfo.h>
#include <proc/readproc.h>
#include <QDebug>
#include "utils.h"

DWIDGET_USE_NAMESPACE
using namespace Utils;

AttributesDialog::AttributesDialog(int pid)
{
    setFixedSize(320, 300);
    
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    auto nameLayout = new QHBoxLayout();
    auto cmdlineLayout = new QHBoxLayout();
    
    auto closeButton = new DWindowCloseButton;
    closeButton->setFixedSize(27, 23);
    connect(closeButton, &DWindowCloseButton::clicked, this, &DAbstractDialog::hide);

    auto iconLabel = new QLabel();
    
    auto nameTitleLabel = new QLabel("程序:");
    nameTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    nameTitleLabel->setFixedWidth(100);
    nameTitleLabel->setAlignment(Qt::AlignRight);
    
    auto nameLabel = new QLabel();
    nameLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");
    
    auto cmdlineTitleLabel = new QLabel("命令行:");
    cmdlineTitleLabel->setStyleSheet("QLabel { background-color : transparent; color : #666666; }");
    cmdlineTitleLabel->setFixedWidth(100);
    cmdlineTitleLabel->setAlignment(Qt::AlignRight);
    
    auto cmdlineLabel = new QLabel();
    cmdlineLabel->setStyleSheet("QLabel { background-color : transparent; color : #000000; }");
    cmdlineLabel->setWordWrap(true);
    
    nameLayout->addWidget(nameTitleLabel);
    nameLayout->addWidget(nameLabel);

    cmdlineLayout->addWidget(cmdlineTitleLabel);
    cmdlineLayout->addWidget(cmdlineLabel);
    
    layout->addWidget(closeButton, 0, Qt::AlignTop | Qt::AlignRight);
    layout->addSpacing(20);
    layout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    layout->addStretch();
    layout->addLayout(nameLayout);
    layout->addLayout(cmdlineLayout);
    layout->addSpacing(20);
    
    // Read the list of open processes information.
    PROCTAB* proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS | PROC_FILLUSR | PROC_FILLCOM);
    static proc_t proc_info;
    memset(&proc_info, 0, sizeof(proc_t));

    std::map<int, proc_t> processes;
    while (readproc(proc, &proc_info) != NULL) {
        processes[proc_info.tid]=proc_info;
    }
    closeproc(proc);

    QMap<QString, QPixmap> *processIconCache = new QMap<QString, QPixmap>();
    for(auto &i:processes) {
        int processId = (&i.second)->tid;
        
        if (pid == processId) {
            QString name = getProcessName(&i.second);
            QString cmdline = Utils::getProcessCmdline(processId);
            QPixmap icon = getProcessIconFromName(name, processIconCache, 96);
            
            iconLabel->setPixmap(icon);
            nameLabel->setText(name);
            cmdlineLabel->setText(cmdline);
            
            // qDebug() << name << cmdline;
            
            break;
        }
    }
}

void AttributesDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPainterPath path;
    path.addRect(QRectF(rect()));
    painter.setOpacity(1);
    painter.fillPath(path, QColor("#ffffff"));
}