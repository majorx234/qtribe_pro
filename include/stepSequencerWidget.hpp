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
 **************************************************************************/

#ifndef _STEPSEQUENCERWIDGET_HPP_
#define _STEPSEQUENCERWIDGET_HPP_

#include <QtWidgets>

#include "ui_stepsequencerwidgetbase.h"

#include "sequencerCore.hpp"
namespace Ui {
class stepSequencerWidget;
}

class stepSequencerWidget : public QWidget{
  Q_OBJECT
 private:
  Ui::stepsequencerwidgetbase *ui;
 public:
  explicit stepSequencerWidget(QWidget* parent = 0,
                               const char* name = 0);
      //      ,Qt::WFlags fl = 0 );
  ~stepSequencerWidget();
  void setBankFile(char* c);
 private:
  //qTribe* mySequencer;

  SequencerCore* mySequencerCore;
  std::thread mySequencerThread;

  int selectedStep;
  int selectedChainStep;

  int patternStepSong;
  int stepMode;
  int patternMode;

  int playing;

  int delayedPatternChange;

  int muteMode;
  int selectedMeasure;
  QPalette pal;
  QColor buttonOffColor;
  QColor buttonOnColor;
  QColor buttonPlayColor;
  QColor selectedChainColor;
  QString bankFile;
  void setStepButtonColors();
  void chainClearStepButtonColors();
  void setSynthPartButtonColors();
  void setDrumPartButtonColors();
  void updatePlaybackPosition();
  int get_dataDial_value();

 public slots:
  void start();
  void muteParts_toggled(bool);
  void stepModeGroup_clicked(int);
  void synthParts_clicked(int);
  void dataDial_valueChanged(int);
  void sequence_clicked(int);
  void drumParts_clicked(int);
  void play_toggled(bool);
  void patternModeGroup_clicked(int);
  void editPositionGroup_clicked(int);
  void bpm_valueChanged(int);
  void updateGui();
  void writeButton_clicked();
  void loadButton_clicked();
  void chainGroup_clicked(int);
  void arpOn_stateChanged(bool);
};

#endif // _STEPSEQUENCERWIDGET_HPP_
