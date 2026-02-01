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

#ifndef QTRIBE_HPP_
#define QTRIBE_HPP_

#include <QtWidgets/QMainWindow>
#include "stepSequencerWidget.hpp"

class qTribe: public QMainWindow
{
  Q_OBJECT

 public:
  qTribe();
  ~qTribe();

  void loadFile(const QString &fileName);
 signals:
  void closed();


 protected:
  void closeEvent(QCloseEvent *event) override;
  void resizeEvent(QResizeEvent* event) override;
 private slots:
  void newFile();
  void open();
  bool save();
  void close();

 private:
  void createMenuBar();
  void createStatusBar();
  bool saveFile(const QString &fileName);
  stepSequencerWidget* main_sequencer_widget;
};


#endif // QTRIBE_HPP_
