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

#ifndef _NOTICATOR_WIDGET_HPP_
#define _NOTICATOR_WIDGET_HPP_

#include <QtWidgets>

class NoticatorWidget : public QWidget{
  Q_OBJECT
 public:
  NoticatorWidget(QWidget* parent);
  ~NoticatorWidget();

 public slots:
  void set_note(int note_value);
  void set_length(float length);
  void set_intensity(int intensity);
  void set_full_note(int note_value, float length, int intensity);

 private:
  int note_value;
  float length;
  int intensity;

 protected:
  virtual void paintEvent(QPaintEvent*) override;
  virtual void resizeEvent(QResizeEvent *ev) override;
};

#endif // _NOTICATOR_WIDGET_HPP_
