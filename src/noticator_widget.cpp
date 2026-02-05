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

#include <QPainter>
#include <QColor>
#include <QResizeEvent>

#include "noticator_widget.hpp"

NoticatorWidget::NoticatorWidget(QWidget *parent, size_t min_width, size_t min_height)
    : min_width(min_width), min_height(min_height), note_value(0), length(0.0), intensity(0) {
  setMinimumSize(min_width, min_height);
  setPalette(QPalette(QColor(200, 200, 200), QColor(20, 0, 70)));
  setAutoFillBackground(true);
  //TODO: generate widget
}
NoticatorWidget::~NoticatorWidget(){}

void NoticatorWidget::set_note(int note_value) {
  this->note_value = note_value;
  update();
}
void NoticatorWidget::set_length(int length) {
  this->length = length;
  update();
}
void NoticatorWidget::set_intensity(int intensity) {
  this->intensity = intensity;
  update();
}
void NoticatorWidget::set_full_note(int note_value, int length, int intensity){
  this->note_value = note_value;
  this->length = length;
  this->intensity = intensity;
  update();
}

void NoticatorWidget::paintEvent(QPaintEvent *event) {
  QPainter p;

  int w = width();
  int h = height();
  int norm_inensity = (int)((((float)intensity) / 127.0) * h);
  int norm_length = (int)((((float)length) / 127.0) * w);

  p.begin(this);
  p.setPen(QPen(QBrush(QColor(0, 233, 0)), 1));
  p.drawRect(0, h - norm_inensity, norm_length, norm_inensity);
  p.end();
}

void NoticatorWidget::resizeEvent(QResizeEvent *ev) {

}
