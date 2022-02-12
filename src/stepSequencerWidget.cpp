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

#include <QString>
#include <QPushButton>
#include <QPalette>
#include <qdir.h>

#include "stepSequencerWidget.hpp"

#ifndef DEFAULT_BANK_DIR
#define DEFAULT_BANK_DIR "/usr/share/qtribe"
#endif

const char* noteNames[]={"C-","C#-","D-","D#-","E-","F-","F#-","G-","G#-","A-","A#-","B-","C0","C#0","D0","D#0","E0","F0","F#0","G0","G#0","A0","A#0","B0","C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1","C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2","C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3","C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4","C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5","C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6","C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7","C8","C#8","D8","D#8","E8","F8","F#8","G8","G#8","A8","A#8","B8","C9","C#9","D9","D#9","E9","F9","F#9","G9","G#9","A9","A#9","B9","ERR"};

stepSequencerWidget::stepSequencerWidget(QWidget *parent,
                                         const char *name)
    : QWidget(parent)
    , ui(new Ui::stepsequencerwidgetbase)
    , bankFile(QString())
    , playing(0)
    , selectedStep(0)
    , selectedChainStep(0)
    , selectedMeasure(0)
    , patternStepSong(0)
    , delayedPatternChange(0)
    , patternMode(2)
    , stepMode(0)
    , buttonOnColor(QColor(200,255,128))
    , buttonOffColor(QColor(240,240,240))
    , buttonPlayColor(QColor(255,128,128))
    , selectedChainColor(QColor(255,192,128))
    , muteMode(0) {
  ui->setupUi(this);
  pal = ui->synthPart1->palette();
  
  mySequencerCore = new SequencerCore();
  mySequencerCore->initSequencer();

  QString file("qtribe.bank");
  QString homePath = QDir::homePath()+QDir::separator()+".qtribe"+QDir::separator()+file;
  QString defaultPath = QString(DEFAULT_BANK_DIR) + QDir::separator() + file;
  if (QFile::exists(homePath)) {
    fprintf(stdout,"Loading Bankfile from path: %s\n", homePath.toStdString().c_str());
    mySequencerCore->loadBank((char*)homePath.toStdString().c_str());
  } else if (QFile::exists(defaultPath)) {
    fprintf(stdout,"Loading Bankfile from path: %s\n",defaultPath.toStdString().c_str());
    mySequencerCore->loadBank((char*)defaultPath.toStdString().c_str());
  } else {
    fprintf(stdout,"Creating new Bank\n");
    mySequencerCore->createBank();
  }
  if (!QFile::exists(homePath)) {
    //if our dotfile doesnt exist, we should create it.
    QDir d(QDir::homePath());
    if (d.exists(".qtribe")) {
      //dotdir exists, just no file saved in it.
    } else {
      if (d.mkdir(".qtribe")) {
        fprintf(stdout,"Created ~/.qtribe\n");
      } else {
        fprintf(stderr,"ERROR: Couldn't create ~/.qtdir - check permissions!\n");
        mySequencerCore->stopSequence();
        mySequencerCore->cleanup();
        // should close the window here
      }
    }
  }
  setBankFile((char*)homePath.toStdString().c_str());

  // stepModeGroup_clicked(stepMode);
  // patternModeGroup_clicked(patternMode);

  // setStepButtonColors();
  // setSynthPartButtonColors();
  // setDrumPartButtonColors();

  // run the sequencer
  mySequencerThread = std::thread(&SequencerCore::run,mySequencerCore);
}

stepSequencerWidget::~stepSequencerWidget() {
  
}

void stepSequencerWidget::start() {
  mySequencerCore->start();
}
void stepSequencerWidget::setBankFile(char *c) {
  bankFile = c;
}


void stepSequencerWidget::setStepButtonColors() {
  
}
void stepSequencerWidget::chainClearStepButtonColors() {
  
}
void stepSequencerWidget::setSynthPartButtonColors() {
  
}
void stepSequencerWidget::setDrumPartButtonColors() {
  
}
void stepSequencerWidget::updatePlaybackPosition() {
  
}

void stepSequencerWidget::muteParts_toggled(bool b) {

}
void stepSequencerWidget::stepModeGroup_clicked(int) {
  
}
void stepSequencerWidget::synthParts_clicked(int) {
  
}
void stepSequencerWidget::dataDial_valueChanged(int) {
  
}
void stepSequencerWidget::sequence_clicked(int step_array_index) {
  //depending on what mode we are in we want different behaviour here.
  //for now lets just implement the step editing facility.
  if (patternStepSong==2)
    {
    stepPatternChain* myPatternChain=mySequencerCore->getPatternChain();
    //handle chain-mode functions
    selectedChainStep=step_array_index;
    chainClearStepButtonColors();
    ui->dataDisplay->setText(QString("P%1").arg(myPatternChain->getPatternIndex(step_array_index)));

    }
  else
    {
    patternStepSong=1;
    step* myStep;
    stepPattern* myPattern=mySequencerCore->getCurrentPattern();
    stepSequence* mySequence=myPattern->getActiveSequence();
    myStep=mySequence->getStep(step_array_index+(selectedMeasure*16));  

    ui->dataDisplay->setText(QString("%1").arg("qTribe"));
    //fprintf(stderr,"STEP INFO: stepMode: %d On: %d Note Number %d Length %d Vel %d\n",stepMode,myStep->isOn,myStep->noteNumber,myStep->noteLength,myStep->noteVelocity);
    selectedStep=step_array_index+1;
    if (stepMode==0) { //note on/off
        //QPushButton* myButton
         QAbstractButton* myButton  = ui->sequenceGroup->button(step_array_index);
      if (! myStep->isOn) {
        myStep->isOn=1;  
        pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
        myButton ->setPalette( pal );
      } else {
        fprintf(stderr,"Turning step off\n");
        myStep->isOn = 0;
        pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
        myButton ->setPalette( pal );
      }
    } 

    if (stepMode==1) { //note number
      //ensure this button is on, even if we clicked on it when it was on.
      //TODO: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      ui->dataDisplay->setText(QString("%1").arg(noteNames[myStep->noteNumber]));
      //fprintf(stderr,"STEP DISPLAY INFO: %d",myStep->noteNumber);
    }
  
    if (stepMode==2) { //length
      //ensure this button is on, even if we clicked on it when it was on.
      //TODO: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      ui->dataDisplay->setText(QString("%1").arg(myStep->noteLength));
    }
  
    if (stepMode==3) { //note velocity 
      //ensure this button is on, even if we clicked on it when it was on.
      //TODO: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      ui->dataDisplay->setText(QString("%1").arg(myStep->noteVelocity));
    }
  }  
}
void stepSequencerWidget::drumParts_clicked(int) {
  
}
void stepSequencerWidget::play_toggled(bool b) {
  if (b) {
    //fprintf(stderr,"Playing Sequence in thread %d\n",mySequencerThread);
    mySequencerCore->playSequence();
    stepPatternChain* myPatternChain=mySequencerCore->getPatternChain();
    playing = 1;
    if (patternStepSong == 2) {
      chainClearStepButtonColors();
      mySequencerCore->setPattern(myPatternChain->getCurrentPattern());
      ui->dataDisplay->setText(QString("P%1").arg(myPatternChain->getCurrentPattern()));
      delayedPatternChange=myPatternChain->getCurrentPattern();
    } else {
      setStepButtonColors();
    }
  } else {
    //fprintf(stderr,"Stopping Sequencein thread %d\n",mySequencerThread);
    mySequencerCore->stopSequence();
    playing = 0;
    if (patternStepSong == 2) {
      chainClearStepButtonColors();
    } else {
      setStepButtonColors();
    }
  }  
}
void stepSequencerWidget::patternModeGroup_clicked(int) {
  
}
void stepSequencerWidget::editPositionGroup_clicked(int) {
  
}
void stepSequencerWidget::bpm_valueChanged(int bpm) {
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  myPattern->setPatternTempo(bpm);
}
void stepSequencerWidget::updateGui() {
  
}
void stepSequencerWidget::writeButton_clicked() {
  
}
void stepSequencerWidget::loadButton_clicked() {
  
}
void stepSequencerWidget::chainGroup_clicked(int) {
  
}
void stepSequencerWidget::arpOn_stateChanged(bool) {
  
}
