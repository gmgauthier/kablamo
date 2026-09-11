/* SPDX-License-Identifier: Unlicense */

#pragma once

#include "board.hpp"
#include "hud.hpp"
#include "mine_field.hpp"

#include <gtkmm.h>

namespace kablamo {

class MainWindow : public Gtk::Window {
 public:
  MainWindow();

 private:
  void load_css();
  void build_menu();
  void on_new();
  void on_quit();
  void on_about();
  void on_pressing(bool down);
  void on_changed();
  void sync_hud();
  void stop_timer();
  bool on_tick();

  Gtk::MenuItem* add_item(Gtk::Menu& menu, const Glib::ustring& label,
                          const sigc::slot<void()>& slot, guint key = 0,
                          Gdk::ModifierType mods = Gdk::ModifierType(0));

 protected:
  bool on_key_press_event(GdkEventKey* event) override;

  Gtk::Box root_{Gtk::ORIENTATION_VERTICAL, 0};
  Gtk::MenuBar menubar_;
  Gtk::Box well_{Gtk::ORIENTATION_VERTICAL, 6};
  Board board_;
  Hud hud_;
  MineField field_{board_};
  Glib::RefPtr<Gtk::AccelGroup> accel_;
  sigc::connection tick_;
  int seconds_ = 0;
};

}  // namespace kablamo
