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

#ifndef _SEQUENCERCORE_HPP_
#define _SEQUENCERCORE_

#include "stepSequence.hpp"

class SequencerCore {
 public:
  void initSequencer();
  void cleanup();
  void setPattern(int i);
  void run();
  
  stepPatternChain* getPatternChain();
  stepPattern* getCurrentPattern();
  int getCurrentPatternIndex();

  void playSequence();
  void stopSequence();

  void createBank();
  void loadBank(char*);
  void saveBank(char*);

  void setActiveSequence(int seq);

  SequencerCore();
  ~SequencerCore();

 private:
  stepPatternChain* myPatternChain;
  stepPattern* patterns[16];
  stepPattern* myPattern;
  int myPatternNumber;

  stepPattern* createBlankPattern();
  int playing;
  int alive;
  int sequenceId;
  int backendInitialised;
};

#endif //  _SEQUENCERCORE_HPP_
