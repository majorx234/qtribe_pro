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

#include <fstream>
#include <QString>
#include <QPushButton>
#include <QSpinBox>
#include <QPalette>
#include <qdir.h>
#include <QButtonGroup>

#include "stepSequencerWidget.hpp"

#ifndef DEFAULT_BANK_DIR
#define DEFAULT_BANK_DIR "/usr/share/qtribe"
#endif

const char* noteNames[]={"C-","C#-","D-","D#-","E-","F-","F#-","G-","G#-","A-","A#-","B-","C0","C#0","D0","D#0","E0","F0","F#0","G0","G#0","A0","A#0","B0","C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1","C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2","C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3","C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4","C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5","C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6","C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7","C8","C#8","D8","D#8","E8","F8","F#8","G8","G#8","A8","A#8","B8","C9","C#9","D9","D#9","E9","F9","F#9","G9","G#9","A9","A#9","B9","ERR"};

void assignIdsToBottons(QButtonGroup *buttonGroup, QString regex) {
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  QRegExp re(regex);
  foreach (QAbstractButton *button, buttons) {
    QString qbuttonname = button->objectName();
    (void)re.indexIn(qbuttonname); // we know it matches, but we need the captured text
    const int id = re.cap(1).toInt() - 1;
    buttonGroup->setId(button, id); // assuming the button is already in the button group
    std::string button_str = qbuttonname.toStdString();
  }
}

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
  pal = ui->synthPart01->palette();

  std::function<void()> stepCallback = std::bind(&stepSequencerWidget::updateGui, this );
  mySequencerCore = new SequencerCore(stepCallback);
  mySequencerCore->initSequencer();

  QString file("qtribe.bank");
  QString homePath = QDir::homePath()+QDir::separator()+".qtribe"+QDir::separator()+file;
  QString defaultPath = QString(DEFAULT_BANK_DIR) + QDir::separator() + file;
  if (QFile::exists(homePath)) {
    fprintf(stdout,"Loading Bankfile from path: %s\n", homePath.toStdString().c_str());
    std::ifstream fin((char*)homePath.toStdString().c_str());
    std::string file_content;
    fin >> file_content;
    mySequencerCore->loadBank(file_content);
  } else if (QFile::exists(defaultPath)) {
    fprintf(stdout,"Loading Bankfile from path: %s\n",defaultPath.toStdString().c_str());
    std::ifstream fin((char*)defaultPath.toStdString().c_str());
    std::string file_content;
    fin >> file_content;
    mySequencerCore->loadBank(file_content);
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

  // set id of Buttons in Buttongroups:
  assignIdsToBottons(ui->sequenceGroup,QString("step(\\d+)"));
  assignIdsToBottons(ui->drumParts,QString("drumPartButton(\\d+)"));
  assignIdsToBottons(ui->synthParts, QString("synthPart(\\d+)"));
  assignIdsToBottons(ui->playPositionGroup,QString("playPositionButton(\\d+)"));
  assignIdsToBottons(ui->editPositionGroup,QString("editPositionButton(\\d+)"));

  // connect signals and slots
  connect(ui->play, &QToolButton::toggled, this, &stepSequencerWidget::play_toggled );
  connect(ui->sequenceGroup, &QButtonGroup::idClicked, this, &stepSequencerWidget::sequence_clicked);
  connect(ui->dataDial,&QDial::valueChanged,this,&stepSequencerWidget::dataDial_valueChanged);
  connect(ui->stepModeGroup, &QButtonGroup::idClicked, this, &stepSequencerWidget::stepModeGroup_clicked);
  connect(ui->muteParts, &QCheckBox::toggled,this, &stepSequencerWidget::muteParts_toggled );
  connect(ui->editPositionGroup,&QButtonGroup::idClicked, this, &stepSequencerWidget::editPositionGroup_clicked);
  connect(ui->patternModeGroup,&QButtonGroup::idClicked, this, &stepSequencerWidget::patternModeGroup_clicked);
  connect(ui->drumParts, &QButtonGroup::idClicked, this, &stepSequencerWidget::drumParts_clicked);
  connect(ui->synthParts, &QButtonGroup::idClicked, this, &stepSequencerWidget::synthParts_clicked);
  connect(ui->writeButton, &QToolButton::clicked, this, &stepSequencerWidget::writeButton_clicked);
  connect(ui->bpm, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &stepSequencerWidget::bpm_valueChanged);
  connect(ui->chainGroup, &QButtonGroup::idClicked, this, &stepSequencerWidget::chainGroup_clicked);
  connect(ui->arpOn,&QCheckBox::toggled, this, &stepSequencerWidget::arpOn_stateChanged);

  stepModeGroup_clicked(stepMode);
  patternModeGroup_clicked(patternMode);

  setStepButtonColors();
  setSynthPartButtonColors();
  setDrumPartButtonColors();

  // run the sequencer
  mySequencerThread = std::thread(&SequencerCore::run, mySequencerCore);
}

stepSequencerWidget::~stepSequencerWidget() {
  mySequencerCore->cleanup();
  mySequencerThread.join();
}

void stepSequencerWidget::start() {
  mySequencerCore->start();
}
void stepSequencerWidget::setBankFile(char *c) {
  bankFile = c;
}

void stepSequencerWidget::setStepButtonColors() {
  step* myStep;
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  stepSequence* mySequence = myPattern->getActiveSequence();

  for (int i=0;i< 16;i++) {
    QAbstractButton* myButton = ui->sequenceGroup->button(i);
    myStep=mySequence->getStep(i+(selectedMeasure*16));
    if (myButton->isChecked()) {
      myButton->setChecked(false);
    }
    if ((selectedStep-1)==i) {
      myButton->setChecked(true);
    }
    if (myStep->isOn) {
       pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
      myButton ->setPalette( pal );
    } else {
      pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
      myButton ->setPalette( pal );
    }
  }
}

void stepSequencerWidget::chainClearStepButtonColors() {
  stepPatternChain* myPatternChain = mySequencerCore->getPatternChain();
  for (int i=0;i< 16;i++) {
    QAbstractButton* myButton = ui->sequenceGroup->button(i);
    myButton->setDown(true);

    if (myPatternChain->getPatternIndex(i) != 0) {
      pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
      myButton ->setPalette( pal );
    } else {
      pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
      myButton ->setPalette( pal );
    }
  }

  //light up the currently playing chain index
  int myButtonIndex=myPatternChain->getCurrentPatternIndex();

  QAbstractButton* chainButton = ui->sequenceGroup->button(myButtonIndex);
  pal.setColor( QPalette::Active, QPalette::Button, buttonPlayColor);
  chainButton ->setPalette( pal );

  //we're in chain mode so we need to light up the currentsly selected chain step
  QToolButton* myButton=(QToolButton*) ui->sequenceGroup->button(selectedChainStep);
  pal.setColor( QPalette::Active, QPalette::Button,selectedChainColor);
  myButton ->setPalette( pal );
}

void stepSequencerWidget::setSynthPartButtonColors() {
  stepSequence* mySequence;
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  stepSequence* activeSequence=myPattern->getActiveSequence();
  for (int i=0;i< 4;i++) {
    QToolButton* myButton=(QToolButton*) ui->synthParts->button(i);
    mySequence=myPattern->getSequence(i+1);

    myButton->setChecked(false);

    if (! mySequence->getMuted()) {
       pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
      myButton ->setPalette( pal );
    } else {
      pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
      myButton ->setPalette( pal );
    }
    if (mySequence==activeSequence) {
      myButton->setChecked( true );
    }
  }
}
void stepSequencerWidget::setDrumPartButtonColors() {
  stepSequence* mySequence;
  stepPattern* myPattern=mySequencerCore->getCurrentPattern();
  stepSequence* activeSequence=myPattern->getActiveSequence();
  for (int i=0;i< 9;i++) {
    QAbstractButton* myButton = ui->drumParts->button(i);
    //offset for 0-base and synthParts
    mySequence=myPattern->getSequence(i+5);

    myButton->setChecked(true);

    if (! mySequence->getMuted()) {
      pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
      myButton ->setPalette( pal );
    } else {
      pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
      myButton ->setPalette( pal );
    }

    if (mySequence==activeSequence) {
      myButton->setChecked( true );
    }
  }
}

void stepSequencerWidget::updatePlaybackPosition() {
  stepPatternChain* myPatternChain = mySequencerCore->getPatternChain();
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  stepSequence* activeSequence = myPattern->getActiveSequence();
  int s = myPattern->getCurrentStepIndex()-1;
  //fprintf(stderr,"DEBUG: patternStepsong %d\n",patternStepSong);
  if (patternStepSong == 2) {
    //chain mode functions
    if ( s == 15) {
      //this is here to ensure we dont update multiple times per step - enable delayedPatternchange
      //on the last step in song/chain mode so we can be sure our chain incrementing call below
      //will not get called more than once.
      delayedPatternChange = 1;
    }

    if (s==0 && delayedPatternChange) {
      //we're in song mode, so we need to switch to the next pattern in the chain,
      //and update the step key to show the current pattern

      delayedPatternChange = 0;
      myPatternChain->getNextPattern();
      setSynthPartButtonColors();
      setDrumPartButtonColors();
      chainClearStepButtonColors();
      mySequencerCore->setPattern(myPatternChain->getCurrentPattern());
      ui->dataDisplay->setText(QString("P%1").arg(myPatternChain->getCurrentPattern()));
      //we're in chain mode so we need to light up the step currently playing
      //int myButtonIndex=myPatternChain->getCurrentPatternIndex();

      //QToolButton* myButton=(QToolButton*) sequenceGroup->find(myButtonIndex);
      //pal.setColor( QPalette::Active, QColorGroup::Button, buttonPlayColor);
      //myButton ->setPalette( pal );
    }
  } else {
    if (s == 0 && delayedPatternChange) {
      mySequencerCore->setPattern(delayedPatternChange);
      ui->dataDisplay->setText(QString("P%1").arg(mySequencerCore->getCurrentPatternIndex()));
      delayedPatternChange = 0;
      fprintf(stderr,"DEBUG: about to call patternModeGroup_clicked %d\n",patternMode);
      patternModeGroup_clicked(patternMode);
      setSynthPartButtonColors();
      setDrumPartButtonColors();
      setStepButtonColors();
    }
    //fprintf(stderr,"stepPos: %d\n",s % 16);

    //setStepButtonColors();
    if (playing) {
      if(s != -1) {
        int myButtonIndex = s % 16;
        int myPrevButtonIndex = myButtonIndex-1;
        if (myPrevButtonIndex < 0) {
          myPrevButtonIndex = 15;
        }
        QAbstractButton* myButton = ui->sequenceGroup->button(myButtonIndex);
        pal.setColor( QPalette::Active, QPalette::Button, buttonPlayColor);
        myButton ->setPalette( pal );
        myButton = ui->sequenceGroup->button(myPrevButtonIndex);
        step* myStep = activeSequence->getStep(myPrevButtonIndex+(selectedMeasure*16));
        if (myStep->isOn) {
          pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
          myButton ->setPalette( pal );
        } else {
          pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
          myButton ->setPalette( pal );
        }
      }
    }
  }
  //doesnt matter what mode we are in, we should always uplate the playback indicator
  if (playing) {
    if (s > -1 and s < 16) {
      ui->playPositionGroup->button(0)->setDown(true);
    } else if (s > 15 and s < 32) {
      ui->playPositionGroup->button(1)->setDown(true);
    } else if (s > 31 and s < 48) {
      ui->playPositionGroup->button(2)->setDown(true);
    } else if (s > 47 and s < 64) {
      ui->playPositionGroup->button(3)->setDown(true);
    } else if (s > 63 and s < 80) {
      ui->playPositionGroup->button(4)->setDown(true);
    } else if (s > 79 and s < 96) {
      ui->playPositionGroup->button(5)->setDown(true);
    } else if (s > 95 and s < 112) {
      ui->playPositionGroup->button(6)->setDown(true);
    } else {
      ui->playPositionGroup->button(7)->setDown(true);
    }
  }
}

void stepSequencerWidget::updateGui() {
  //fprintf(stderr,"UPDATE GUI FROM TIMER\n");
  if (playing) {
    ui->statusline->setText(QString("Status: %1").arg(patternStepSong));
    updatePlaybackPosition();
  }
}

void stepSequencerWidget::muteParts_toggled(bool b) {
  if (b) {
    muteMode=1;
  } else {
    muteMode=0;
  }
}
void stepSequencerWidget::stepModeGroup_clicked(int mode) {
  //we are in step mode (arg is 0 - on./off, 1 - note - 2- length 3 -velocity)
  stepMode = mode;
  patternStepSong = 1;
   if (stepMode==0) {
    ui->sequenceGroup->setExclusive(false);
    ui->dataDisplay->setText("qTribe");
    //now we need to go through our steps and set them on or off depending on our underlying step model.
    QAbstractButton* myButton;
    step* myStep;
    stepPattern* myPattern = mySequencerCore->getCurrentPattern();
    stepSequence* mySequence = myPattern->getActiveSequence();
    for (int i=0;i< 16;i++) {
      //myButton = (QToolButton*) ui->sequenceGroup->button(i);
      myButton = ui->sequenceGroup->button(i);
      //fprintf(stderr,"BUTTON: %s\n",myButton->className());
      myStep = mySequence->getStep(i+(selectedMeasure * 16));
      if (myStep->isOn) {
        myButton->setChecked(true);
        pal.setColor( QPalette::Active, QPalette::Button, buttonOnColor);
        myButton->setPalette( pal ); 
      } else {
        myButton->setChecked(false);
        pal.setColor( QPalette::Active, QPalette::Button, buttonOffColor);
        myButton->setPalette( pal );
      }
    }
  }
  if (patternStepSong == 2) {
    // chain mode
    // if we have a selected step, assign the selected pattern to it.

    stepPatternChain* myPatternChain = mySequencerCore->getPatternChain();
    // TODO: figure out where dataVal usally come from
    // looks like it is used to set value here <- come from dial
    int dataVal = get_dataDial_value();
    int modDataVal=(dataVal + 8)/9;
    ui->dataDisplay->setText(QString("P%1").arg(modDataVal));
    myPatternChain->setPattern(selectedChainStep, modDataVal) ;
  } else if (stepMode==1 || stepMode==2 || stepMode==3 || stepMode==4) {
    ui->sequenceGroup->setExclusive(true);
    ui->dataDisplay->setText("---");
    step* myStep;
    stepPattern* myPattern = mySequencerCore->getCurrentPattern();
    stepSequence* mySequence=myPattern->getActiveSequence();

    for (int i=0;i< 16;i++) {
      QAbstractButton* myButton = ui->sequenceGroup->button(i);
      // old: myButton->setOn(true);
      myButton->setChecked(true);
    }

    if (selectedStep) {
      int step_array_index=selectedStep-1;
      // old: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      myStep=mySequence->getStep(step_array_index+(selectedMeasure*16));

      if (stepMode==1) {
        ui->dataDisplay->setText(QString("%1").arg(noteNames[myStep->noteNumber]));
      }
      if (stepMode==2) {
        ui->dataDisplay->setText(QString("%1").arg(myStep->noteLength));
      }
      if (stepMode==3) {
        ui->dataDisplay->setText(QString("%1").arg(myStep->noteVelocity));
      }
      if (stepMode==4) {
        ui->dataDisplay->setText(QString("%1").arg(myStep->noteTonality));
      }
    }
    if (mySequence->drumSequence==1) {
      //drum part
      if (stepMode==1) {
        ui->dataDisplay->setText(QString("%1").arg(noteNames[mySequence->drumNote]));
      }
    }
  } else {
    ui->sequenceGroup->setExclusive(true);
  }
}
void stepSequencerWidget::synthParts_clicked(int sequencer_part_index) {
  int sequencerPart=sequencer_part_index+1;
  //we need to switch our active sequence and update our display depending on the mode
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  stepSequence* mySequence = myPattern->getSequence(sequencerPart);
  QAbstractButton* myButton = ui->synthParts->button(sequencer_part_index);

  if (muteMode) {
    if (! mySequence->getMuted()) {
      mySequence->setMuted(1);
    } else {
      mySequence->setMuted(0);
    }
  }
  myPattern->setActiveSequence(sequencerPart);
  myButton->setChecked(true);

  fprintf(stderr,"wtf? %d\n",mySequence->isArp());
  if (mySequence->isArp() == true) {
    ui->arpOn->setChecked(true);
  } else {
    ui->arpOn->setChecked(false);
  }

  if (patternStepSong==0) {
    patternModeGroup_clicked(patternMode);
  } else if (patternStepSong == 1) {
    stepModeGroup_clicked(stepMode);
  } else if (patternStepSong == 2) {
    mySequencerCore->setActiveSequence(sequencer_part_index);
  }
  setSynthPartButtonColors();
  setDrumPartButtonColors();
  setStepButtonColors();
}

int stepSequencerWidget::get_dataDial_value() {
  return ui->dataDial->value();
}

void stepSequencerWidget::dataDial_valueChanged(int dataVal) {
  // here we need to update our backed data to reflect our new selection.
  if (patternStepSong == 0) {
    if (patternMode == 0) {
      int modDataVal=(dataVal+9)/9;
      ui->dataDisplay->setText(QString("P%1").arg(modDataVal));
      if (playing) {
        //queue switching of pattern at next step 0.
        delayedPatternChange=modDataVal;
        ui->dataDisplay->setText(QString(">P%1").arg(modDataVal));
      } else {
        mySequencerCore->setPattern(modDataVal);
        stepModeGroup_clicked(stepMode);
        patternModeGroup_clicked(patternMode);
        setSynthPartButtonColors();
        setDrumPartButtonColors();
        setStepButtonColors();
      }
    }

    if (patternMode==1) {
      int modDataVal = (dataVal+9)/9;
      //fprintf(stderr,"Setting MIDI Channel %d orig %d\n",modDataVal,dataVal);
      stepPattern* myPattern = mySequencerCore->getCurrentPattern();
      stepSequence* mySequence=myPattern->getActiveSequence();
      if (mySequence->getMidiChannel() != modDataVal) {
        mySequence->setMidiChannel(modDataVal);
      }
      ui->dataDisplay->setText(QString("%1").arg(modDataVal));
    }

    if (patternMode==2) {
      int modDataVal=(dataVal+18)/17;
      stepPattern* myPattern = mySequencerCore->getCurrentPattern();
      if (myPattern->getPatternLength() != modDataVal) {
        myPattern->setPatternLength(modDataVal*16);
      }
      ui->dataDisplay->setText(QString("%1").arg(modDataVal));
    }
  }

  //do we have a note selected in note mode?
  if (patternStepSong == 1) {
    stepPattern* myPattern = mySequencerCore->getCurrentPattern();
    stepSequence* mySequence=myPattern->getActiveSequence();

    if (mySequence->drumSequence==1 && stepMode==1) {
      mySequence->drumNote=dataVal;
      ui->dataDisplay->setText(QString("%1").arg(noteNames[dataVal]));
      //fprintf(stderr,"We're supposedly a drum sequence\n"  );
    } else {
      if (selectedStep && stepMode==1) {
        int step_array_index=selectedStep-1;
        step* myStep;
        myStep = mySequence->getStep(step_array_index+(selectedMeasure*16));
        myStep->noteNumber=dataVal;
        ui->dataDisplay->setText(QString("%1").arg(noteNames[dataVal]));
        //fprintf(stderr,"STEP INFO: On: %d Note Number %d Length %d Vel %d\n",myStep->isOn,myStep->noteNumber,myStep->noteLength,myStep->noteVelocity);
      }

      if (selectedStep && stepMode==3) {
        int step_array_index=selectedStep-1;
        step* myStep;
        myStep=mySequence->getStep(step_array_index+(selectedMeasure*16));
        myStep->noteVelocity=dataVal;
        ui->dataDisplay->setText(QString("%1").arg(dataVal));
      }
      if (selectedStep && stepMode==4) {
        int step_array_index=selectedStep-1;
        step* myStep;
        myStep=mySequence->getStep(step_array_index+(selectedMeasure*16));
        myStep->noteTonality=dataVal;
        ui->dataDisplay->setText(QString("%1").arg(dataVal));
      }
    }
    if (selectedStep && stepMode==2) {
      int modDataVal=dataVal; //max 128 steps note length
      int step_array_index=selectedStep-1;
      step* myStep;
      myStep=mySequence->getStep(step_array_index+(selectedMeasure*16));
      myStep->noteLength=modDataVal;
      ui->dataDisplay->setText(QString("%1").arg(modDataVal));
    }
  }
  if (patternStepSong==2) {
    //chain mode
    //if we have a selected step, assign the selected pattern to it.

    stepPatternChain* myPatternChain=mySequencerCore->getPatternChain();
    int modDataVal=(dataVal+8)/9;
    ui->dataDisplay->setText(QString("P%1").arg(modDataVal));
    myPatternChain->setPattern(selectedChainStep, modDataVal) ;
  }
}

void stepSequencerWidget::sequence_clicked(int step_array_index) {
  // depending on what mode we are in we want different behaviour here.
  // for now lets just implement the step editing facility.
  if (patternStepSong==2) {
    stepPatternChain* myPatternChain = mySequencerCore->getPatternChain();
    //handle chain-mode functions
    selectedChainStep=step_array_index;
    chainClearStepButtonColors();
    ui->dataDisplay->setText(QString("P%1").arg(myPatternChain->getPatternIndex(step_array_index)));
  } else {
    patternStepSong = 1;
    step* myStep;
    stepPattern* myPattern = mySequencerCore->getCurrentPattern();
    stepSequence* mySequence = myPattern->getActiveSequence();
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
      //Old: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      ui->dataDisplay->setText(QString("%1").arg(noteNames[myStep->noteNumber]));
      //fprintf(stderr,"STEP DISPLAY INFO: %d",myStep->noteNumber);
    }

    if (stepMode==2) { //length
      //ensure this button is on, even if we clicked on it when it was on.
      //Old: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      ui->dataDisplay->setText(QString("%1").arg(myStep->noteLength));
    }

    if (stepMode==3) { //note velocity
      //ensure this button is on, even if we clicked on it when it was on.
      //Old: ui->sequenceGroup->setButton(step_array_index);
      ui->sequenceGroup->button(step_array_index)->setDown(true);
      ui->dataDisplay->setText(QString("%1").arg(myStep->noteVelocity));
    }
  }
}

void stepSequencerWidget::drumParts_clicked(int drum_part_index) {
    //skip  over our synth parts and add once to convert from 0-based index
  int sequencer_part_index = drum_part_index+5;


  //we need to switch our active sequence and update our display depending on the mode
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  stepSequence* mySequence = myPattern->getSequence(sequencer_part_index);
  QAbstractButton* myButton = ui->drumParts->button(drum_part_index);

  if (muteMode) {
    if (! mySequence->getMuted()) {
        mySequence->setMuted(1);
    } else {
      mySequence->setMuted(0);
    }
  }
  //else
  //  {
    myPattern->setActiveSequence(sequencer_part_index);
    myButton->setChecked(true);
  //  }
  if (patternStepSong==0) {
    patternModeGroup_clicked(patternMode);
  } else if (patternStepSong==1) {
    stepModeGroup_clicked(stepMode);
  } else if (patternStepSong==2) {
    mySequencerCore->setActiveSequence(sequencer_part_index);
  }
  setSynthPartButtonColors();
  setDrumPartButtonColors();
  setStepButtonColors();
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

void stepSequencerWidget::patternModeGroup_clicked(int i) {
  fprintf(stderr,"DEBUG:patternModeGroup_clicked\n");
  patternStepSong = 0;
  patternMode = i;
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  stepSequence* activeSequence = myPattern->getActiveSequence();

  if (patternMode==0) {
    ui->dataDisplay->setText(QString("P%1").arg(mySequencerCore->getCurrentPatternIndex()));
  }
  if (patternMode==1) {
    ui->dataDisplay->setText(QString("%1").arg(activeSequence->getMidiChannel()));
  }

  if (patternMode==2) {
    ui->dataDisplay->setText(QString("%1").arg((myPattern->getPatternLength()+9)/16));
  }
  setStepButtonColors();
}

void stepSequencerWidget::editPositionGroup_clicked(int s) {
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  //fprintf(stderr,"DEBUG:%d %d\n",myPattern->getPatternLength(),selectedMeasure*16);
  if (myPattern->getPatternLength() > s*16) {
    //fprintf(stderr,"DEBUG:%d %d\n",selectedMeasure,s);
    selectedMeasure=s;
    stepModeGroup_clicked(stepMode);
    setStepButtonColors();
  } else {
    ui->editPositionGroup->button(selectedMeasure)->setDown(true);
  }
}

void stepSequencerWidget::bpm_valueChanged(int bpm) {
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();
  myPattern->setPatternTempo(bpm);
}

void stepSequencerWidget::writeButton_clicked() {
  FILE* file;
  file = fopen ((char*)bankFile.toStdString().c_str(),"w");

  std::string to_save = mySequencerCore->saveBank();
  fprintf(file, to_save.c_str());
  fclose(file);

}

void stepSequencerWidget::loadButton_clicked() {
  //Currently no GUI widget for this.
  mySequencerCore->stopSequence();
  playing=0;
  mySequencerCore->loadBank((char*)bankFile.toStdString().c_str());
  stepPattern* myPattern = mySequencerCore->getCurrentPattern();

  //set up our UI state to something resembling the default.
  ui->play->setDown(true);
  ui->bpm->setValue(myPattern->getPatternTempo());
  patternStepSong=1;
  stepMode=0;

  stepModeGroup_clicked(stepMode);
  setSynthPartButtonColors();
  setDrumPartButtonColors();
  setStepButtonColors();
}

void stepSequencerWidget::chainGroup_clicked(int i) {
  fprintf(stderr,"DEBUG:chainGroup_clicked %d\n",i);
  stepPatternChain* myPatternChain = mySequencerCore->getPatternChain();
  patternStepSong = 2;
  mySequencerCore->setPattern(myPatternChain->getCurrentPattern());
  ui->dataDisplay->setText(QString("P%1").arg(mySequencerCore->getCurrentPatternIndex()));
  delayedPatternChange = 0;
  setSynthPartButtonColors();
  setDrumPartButtonColors();
  chainClearStepButtonColors();
}

void stepSequencerWidget::arpOn_stateChanged(bool b) {
  fprintf(stderr,"DEBUG:arpOn_stateChanged %d\n",b);
  stepPattern* myPattern=mySequencerCore->getCurrentPattern();
  stepSequence* mySequence=myPattern->getActiveSequence();
  // this is annoying - since Qt fires this signal no matter how the
  // widget is updated (user click or programmatic rest on part change,
  // we need to do extra checking to see if we really need to take
  // action here
  if (b) {
    if (mySequence->isArp() == 0) {mySequence->arpeggiate();}
  } else {
    if (mySequence->isArp() > 0) {mySequence->clearArp();}
  }
}
