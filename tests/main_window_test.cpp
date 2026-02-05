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

#include "main_window_test.hpp"
#include "noticator_widget.hpp"
#include <QHBoxLayout>
#include <QWidget>
#include <QDial>

MainWindowTest::MainWindowTest(QWidget *parent)
  : QMainWindow(parent)
  , noticator_widget(this)
{
  QDial *dial_length = new QDial(this);
  QDial *dial_intensity = new QDial(this);
  QWidget *widget = new QWidget(this);
  QHBoxLayout *layout = new QHBoxLayout(widget);

  dial_length->setMaximum(1.0);
  dial_intensity->setMaximum(127);
  setCentralWidget(widget);
  widget->setLayout(layout);
  layout->addWidget(&noticator_widget);
  layout->addWidget(dial_length);
  layout->addWidget(dial_intensity);

  connect(dial_length, &QDial::valueChanged, this, &MainWindowTest::set_length_scale);
  connect(dial_intensity, &QDial::valueChanged, &this->noticator_widget, &NoticatorWidget::set_intensity);
  timerId = startTimer(1000);
}

MainWindowTest::~MainWindowTest() {
  killTimer(timerId);
}

void MainWindowTest::set_length_scale(int value) {
  this->noticator_widget.set_length(value/127.0);
}


void MainWindowTest::timerEvent(QTimerEvent *event) {
  noticator_widget.set_intensity(50);
  noticator_widget.set_length(0.8);
}
