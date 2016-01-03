#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <stdint.h>

#include <gtkmm/image.h>
#include <gtkmm/window.h>
#include <gtkmm/stack.h>
#include <gtkmm/stacksidebar.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/hvseparator.h>
#include <gdk/gdkkeysyms.h>

#include <glibmm/main.h>

#include "widgets/videocontrol.h"
#include "widgets/videoclipper.h"
#include "navigation.h"

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
