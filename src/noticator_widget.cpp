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

#include "noticator_widget.hpp"

NoticatorWidget::NoticatorWidget(QWidget *parent)
 : note_value(0), length(0.0), intensity(0) {
  //TODO: generate widget
}
NoticatorWidget::~NoticatorWidget(){}

void NoticatorWidget::set_note(int note_value) {
  this->note_value = note_value;
}
void NoticatorWidget::set_length(float length) {
  this->length = length;
}
void NoticatorWidget::set_intensity(int intensity) {
  this->intensity = intensity;
}
void NoticatorWidget::set_note(int note_value, float length, int intensity){
  this->note_value = note_value;
  this->length = length;
  this->intensity = intensity;
}

