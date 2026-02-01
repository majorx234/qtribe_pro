/***************************************************************************
 *   Copyright (C) 2008 by Pete Black                                      *
 *   pete@marchingcubes.com                                                *
 *   Copyright (c) 2022 Majorx234                                          *
 *   majorx234 (at) gmail (dot) com                                        *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtWidgets>

#include "qtribe.hpp"

qTribe::qTribe()
    : main_sequencer_widget(new stepSequencerWidget(this)) {
  setCentralWidget(main_sequencer_widget);
  createMenuBar();
}

qTribe::~qTribe() {
}

void qTribe::closeEvent(QCloseEvent *event) {
   QMainWindow::closeEvent(event);
     if (event->isAccepted())
        emit closed();
}


void qTribe::resizeEvent(QResizeEvent* event) {
  QMainWindow::resizeEvent(event);
}

void qTribe::close() {
  QMainWindow::close();
}

void qTribe::open() {
}

void qTribe::newFile() {
}

bool qTribe::save() {
  return false;
}

void qTribe::createMenuBar() {
  QMenu *fileMenu;
  QAction *openAct;
  QAction *saveAct;
  QAction * exitAct;

  openAct = new QAction(tr("&Open"), this);
  openAct->setShortcuts(QKeySequence::New);
  openAct->setStatusTip(tr("Open a bank file"));
  connect(openAct, &QAction::triggered, this, &qTribe::open);
  saveAct = new QAction(tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::New);
  saveAct->setStatusTip(tr("Save a bank file"));
  connect(saveAct, &QAction::triggered, this, &qTribe::save);
  exitAct = new QAction(tr("&Exit"), this);
  exitAct->setShortcuts(QKeySequence::New);
  exitAct->setStatusTip(tr("Exit Program"));
  connect(exitAct, &QAction::triggered, this, &qTribe::close);
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(exitAct);
}

void qTribe::createStatusBar() {
  // TODO: implementation
}

bool qTribe::saveFile(const QString &fileName){
  // TODO: implementation
  return false;
}

