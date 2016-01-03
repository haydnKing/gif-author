#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <stdint.h>

#include <gdk/gdkkeysyms.h>

#include <glibmm/main.h>

#include "navigation.h"
#include "pages.h"

#include <iostream>

class MainWindow : public Wizzard
{

public:
  MainWindow();
  virtual ~MainWindow();

  void play(bool reverse=false);
  void pause();
  bool isPlaying();

protected:
  //Signal handlers:

  //Member widgets:

};

#endif // GTKMM_MAINWINDOW_H
