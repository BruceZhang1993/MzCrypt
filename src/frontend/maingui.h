// maingui.h

// Copyright (C) 2012 by Werner Lemberg.
//
// This file is part of the ttfautohint library, and may only be used,
// modified, and distributed under the terms given in `COPYING'.  By
// continuing to use, modify, or distribute this file you indicate that you
// have read `COPYING' and understand and accept it fully.
//
// The file `COPYING' mentioned in the previous paragraph is distributed
// with the ttfautohint library.


#ifndef __MAINGUI_H__
#define __MAINGUI_H__

#include <QtGui>
#include "lineedit.h"

#include <stdio.h>

class QAction;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QFile;
class QLabel;
class QLocale;
class QMenu;
class QPushButton;
class QSpinBox;

class Line_Edit;

class Main_GUI
: public QMainWindow
{
  Q_OBJECT

public:
  Main_GUI();

protected:
  void closeEvent(QCloseEvent*);

private slots:
  void about();
  void browse_input();
  void browse_output();
  void absolute_input();
  void absolute_output();
  void check_run();
  void run();
  void slotWorkFinished(int,const QString&);
  void slotTimer();

private:

  void create_layout();

  void create_connections();
  void create_actions();
  void create_menus();
  void create_status_bar();
  void read_settings();
  void write_settings();

  int check_filenames(const QString&, const QString&);
  int handle_error(const unsigned char*, QString);

  QMenu* file_menu;
  QMenu* help_menu;

  Line_Edit* input_line;
  QPushButton* input_button;

  Line_Edit* output_line;
  QPushButton* output_button;

  QComboBox* command_box;

  QPushButton* run_button;

  QAction* exit_act;
  QAction* about_act;
  QAction* about_Qt_act;

  QProgressDialog *progress;
  QTimer timer;
};

#endif // __MAINGUI_H__

// end of maingui.h
