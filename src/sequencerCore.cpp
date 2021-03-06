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

#include "sequencerCore.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

#include "jackIO.hpp"
 
void split(std::string const &str, const char delim,
            std::vector<std::string> &out) {
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}

SequencerCore::SequencerCore(std::function<void ()> step_callback)
    : backendInitialised(0)
    , playing(0)
    , alive(1)
    , sequenceId(0)
    , myPatternNumber(1)
    , stepCallback(step_callback) {
  //constructor
  backendInitialised = initJACK();
  
  if (backendInitialised) {
    fprintf(stderr,"Backend Init. successful.\n");
  } else {
    fprintf(stderr,"Backend Failed to Init.\n");
  }
}

SequencerCore::~SequencerCore() {
  //destructor
 }

void SequencerCore::cleanup() {  
  playing=0;
  alive=0;
  // TODO: what for is this wait?
  //  wait();  
}

void SequencerCore::start() {
  // TODO:need some syncing mechanism here 
}

void SequencerCore::playSequence() {
  playing = 1;
}

void SequencerCore::stopSequence() {
  playing = 0;
  myPattern->setCurrentStepIndex(0);
}

void SequencerCore::setPattern(int i) {
  myPatternNumber = i;
  myPattern = patterns[i-1];
}

int SequencerCore::getCurrentPatternIndex() {
  return myPatternNumber;
}

stepPattern* SequencerCore::getCurrentPattern() {
  //catch oddball condition - switching from chain to pattern mode after setting all steps to P0 in chain mode
  if (myPatternNumber == 0) {
    myPatternNumber = 1;
    myPattern = patterns[0];
  }
  return myPattern;
}

stepPatternChain* SequencerCore::getPatternChain() {
  return myPatternChain;
}

void SequencerCore::run() {
  double last_time = 0;
  while(alive) {
    if(playing) {
      stepCallback();
      double thisStepTime = getJackTime();
      
      //was going to offset multiple notes played at same time by 1ms.. but not
      //sure if necessary.. JACK does odd things from time to time
      int seq_note_inc = 0;
      
      double stepDelta = ((double(60)/(double)myPattern->getPatternTempo())/(double)4 )*1000; //ms
      if (thisStepTime >= (last_time + stepDelta)) {
        for (int i=0;i<12;i++) {
          stepSequence* mySequence = myPattern->getSequence(i+1);
          if (! mySequence->getMuted()) {
            int patternStep = myPattern->getCurrentStepIndex()-1;
            //play our step
            if(patternStep != -1){
              step* myStep = mySequence->getStep(patternStep);
              step* arpStep = mySequence->getArpStep(patternStep);
              if (myStep->isOn) {
                if (mySequence->drumSequence == 1) {
                  stepSequence* myAccents = myPattern->getDrumAccentSequence();
                  step* accentStep = myAccents->getStep(patternStep);
                  int modVelocity = myStep->noteVelocity;
                
                  if (accentStep->isOn) {
                    //implement simple accenting.
                    modVelocity = modVelocity*2;
                    if (modVelocity > 127) {
                      modVelocity=127;
                    }
                  }
                  queue_message(thisStepTime+seq_note_inc, mySequence->getMidiChannel()-1, 144, mySequence->drumNote, modVelocity); //note on
                  queue_message(thisStepTime+(myStep->noteLength*stepDelta), mySequence->getMidiChannel()-1, 128, mySequence->drumNote, 0); //note off
                } else {
                  queue_message(thisStepTime+seq_note_inc, mySequence->getMidiChannel()-1, 144, myStep->noteNumber, myStep->noteVelocity); //note on
                  queue_message(thisStepTime+(myStep->noteLength*stepDelta), mySequence->getMidiChannel()-1, 128, myStep->noteNumber, 0); //note off}
                  //fprintf(stderr,"Note length %d: on / off queued: %d %d\n",myStep->noteLength,thisStepTime,thisStepTime+(myStep->noteLength*stepDelta));
                }
                seq_note_inc+=1;
              }
              if (arpStep->isOn) { 
                queue_message(thisStepTime+seq_note_inc, mySequence->getMidiChannel()-1, 144, arpStep->noteNumber, arpStep->noteVelocity); //note on      
                queue_message(thisStepTime+(myStep->noteLength*stepDelta), mySequence->getMidiChannel()-1, 128, arpStep->noteNumber, 0); //note off}
              }
            }
          }  
        }
        //stepCallback();
        //move our pattern pointer along
        myPattern->nextStep();
        last_time = thisStepTime;    
      } else {
        // usleep(5000);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
    } else {
      // usleep(10000);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  disconnectJACK();
}

stepPattern* SequencerCore::createBlankPattern() {
  int ret;  
  //lets try calling some of our API
  stepPattern* newPattern = new stepPattern();  
  //add our sequences
  ret = newPattern->addSequence("synth","synthPart_1",-1);
  newPattern->setActiveSequence(ret);
  ret = newPattern->addSequence("synth","synthPart_2",-1);
  ret = newPattern->addSequence("synth","synthPart_3",-1);
  ret = newPattern->addSequence("synth","synthPart_4",-1);
  ret = newPattern->addSequence("drum","drumPart_1",32);
  ret = newPattern->addSequence("drum","drumPart_2",33);
  ret = newPattern->addSequence("drum","drumPart_3",34);
  ret = newPattern->addSequence("drum","drumPart_4",35);
  ret = newPattern->addSequence("drum","drumPart_5",36);
  ret = newPattern->addSequence("drum","drumPart_6",37);
  ret = newPattern->addSequence("drum","drumPart_7",38);
  ret = newPattern->addSequence("drum","drumPart_8",39);
  ret = newPattern->addSequence("accent","drumAccent_1",-1);
  newPattern->setDrumAccentSequence(ret);
  //set our pattern length to 16 steps
  newPattern->setPatternLength(16);

  return newPattern;
}

void SequencerCore::initSequencer() {
  //if we can't open our default file, construct a minimal empty bank
  for (int i=0;i<16;i++) {
    patterns[i] = createBlankPattern();
  }
  myPattern = patterns[0];
  myPatternNumber = 1;

  myPatternChain = new stepPatternChain();
}

void SequencerCore::saveBank(char* fileName) {
  FILE* file;
  file = fopen (fileName,"w");
  myPatternChain->serialise(file);
  for (int i=0;i<16;i++) {
    patterns[i]->serialise(file);
  }
  fclose(file);
}

void SequencerCore::createBank() {
  for (int i=0;i<16;i++) {
    patterns[i] = createBlankPattern();
  }
}

void SequencerCore::setActiveSequence(int seq) {
  for (int i=0;i < 16;i++) {
    stepPattern* p = patterns[i];
    if (p != NULL) {
      fprintf(stderr,"Setting pattern %d active seq to %d\n",i,seq);
      p->setActiveSequence(seq);
    }
  }
}

void SequencerCore::loadBank(char* fileName) {
  std::ifstream fin(fileName);
    const int LINE_LENGTH = 100;
    char str[LINE_LENGTH];
  
  //trash all our patterns in memory
  for (int i=0;i<16;i++) {
    delete patterns[i];  
    patterns[i] = nullptr;
  }
  myPattern = nullptr;
  stepPatternChain* currentPatternChain = nullptr;
  stepPattern* currentPattern = nullptr;
  stepSequence* currentSequence = nullptr;  
  step* currentStep;
  int currentStepIndex = 0;
  int patternLength = 0;
  int currentPatternIndex = 0;

  while( fin.getline(str,LINE_LENGTH) ) {
      //use std:strings to make this easy
    std::string line(str);
    std::vector<std::string> parts;
    const char delim = ':';

    split(line, delim, parts);
    if (parts[0]=="patternchain") {
      fprintf(stderr,"DEBUG: loading patternchain\n");
      currentPatternChain=new stepPatternChain();
      std::vector<std::string> data;
      const char delim = '|';
      split(parts[1],delim,data);
      for (int i=0;i<16;i++) {
        currentPatternChain->setPattern(i,stoi(data[i]));
      }
    }

    if (parts[0]=="mutes") {
      fprintf(stderr,"DEBUG: loading mutes\n");
      std::vector<std::string> data;
      const char delim = '|';
      split(parts[1],delim,data);
      for (int i=0;i<16;i++) {
        if (currentPatternChain != NULL) {
          currentPatternChain->setPartMuted(i,stoi(data[i]));
        }
      }
    }
    
    if (parts[0]=="pattern") {
      //fprintf(stderr,"FOUND PATTERN\n");
      
      //we need to set the patern length of the previous pattern when we encounter a pattern tag.
      //see also the block at the ned of the loop that sets the length of the last pattern
      if (patternLength && currentPattern) {
        currentPattern->setPatternLength(patternLength);
      }
      currentPattern=new stepPattern();
      patterns[currentPatternIndex]=currentPattern;
      currentPatternIndex++;

      std::vector<std::string> data;
      const char delim = '|';
      split(parts[1],delim,data);
      //currentStepIndex,patternSteps,patternTempo,drumAccentSequence
      patternLength = stoi(data[1]);
      currentPattern->setPatternTempo(stoi(data[2]));
      currentPattern->setDrumAccentSequence(stoi(data[3]));
    }
    
    if (parts[0]=="sequence") {
      //fprintf(stderr,"FOUND SEQUENCE\n");
      std::vector<std::string> data;
      const char delim = '|';
      split(parts[1],delim,data);
      //sequenceName.ascii(),sequenceType.ascii(),muted,selectedStep,midiChannel,drumSequence,drumNote

      //type, name, drumnote
      int seqindex = currentPattern->addSequence(data[0],data[1],stoi(data[6]));
      currentSequence = currentPattern->getSequence(seqindex);
      currentSequence->setMuted(stoi(data[2]));
      
      currentSequence->setMidiChannel(stoi(data[4]));
      if (seqindex == 1) {
        currentPattern->setActiveSequence(seqindex);
      }
      currentStepIndex = 0;
    }
    
    if (parts[0]=="step") {  
      std::vector<std::string> data;
      const char delim = '|';
      split(parts[1],delim,data);

      currentStep = currentSequence->getStep(currentStepIndex);
      currentStep->isOn = stoi(data[0]);
      currentStep->noteNumber   = stoi(data[1]);
      currentStep->noteLength   = stoi(data[2]);
      currentStep->noteVelocity = stoi(data[3]);
      currentStep->noteTonality = stoi(data[3]);
      currentStepIndex++;
      //fprintf(stderr,"STEP: %d %d\n",currentStep->isOn,currentStep->noteNumber);
    }
  }
  //set patternlength of last pattern.
  if (patternLength && currentPattern) {
    currentPattern->setPatternLength(patternLength);
  }  
  myPattern=patterns[0];
  myPatternNumber=1;
  if (currentPatternChain != NULL) {
    myPatternChain=currentPatternChain;
  }
}
