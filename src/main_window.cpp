/* SPDX-License-Identifier: Unlicense */

#include "main_window.hpp"
#include "about_dialog.hpp"
#include "paths.hpp"

#include <iostream>

namespace kablamo {

MainWindow::MainWindow()
{
  set_title("Kablamo!");
  set_resizable(false);
  set_border_width(0);
  get_style_context()->add_class("kablamo-window");

  accel_ = Gtk::AccelGroup::create();
  add_accel_group(accel_);

  load_css();
  build_menu();

  well_.set_border_width(8);
  well_.pack_start(hud_, Gtk::PACK_SHRINK);
  well_.pack_start(field_, Gtk::PACK_SHRINK);

  field_.signal_pressing().connect(sigc::mem_fun(*this, &MainWindow::on_pressing));
  field_.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_changed));
  hud_.signal_new_game().connect(sigc::mem_fun(*this, &MainWindow::on_new));

  root_.pack_start(menubar_, Gtk::PACK_SHRINK);
  root_.pack_start(well_, Gtk::PACK_SHRINK);
  add(root_);
  show_all();
}

void MainWindow::load_css()
{
  const std::string css_path = find_data_file("skin/lcos/lcos.css");
  if (css_path.empty()) {
    std::cerr << "kablamo: lcos.css not found\n";
    return;
  }
  try {
    auto css = Gtk::CssProvider::create();
    css->load_from_path(css_path);
    Gtk::StyleContext::add_provider_for_screen(
        Gdk::Screen::get_default(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  } catch (const Glib::Error& e) {
    std::cerr << "kablamo: CSS: " << e.what() << "\n";
  }
}

Gtk::MenuItem* MainWindow::add_item(Gtk::Menu& menu, const Glib::ustring& label,
                                    const sigc::slot<void()>& slot, guint key,
                                    Gdk::ModifierType mods)
{
  auto* item = Gtk::manage(new Gtk::MenuItem(label, true));
  item->signal_activate().connect(slot);
  if (key != 0)
    item->add_accelerator("activate", accel_, key, mods, Gtk::ACCEL_VISIBLE);
  menu.append(*item);
  return item;
}

void MainWindow::build_menu()
{
  auto add_menu = [this](const Glib::ustring& label, Gtk::Menu& menu) {
    auto* top = Gtk::manage(new Gtk::MenuItem(label, true));
    top->set_submenu(menu);
    menubar_.append(*top);
  };

  auto* game = Gtk::manage(new Gtk::Menu());
  add_item(*game, "_New", sigc::mem_fun(*this, &MainWindow::on_new), GDK_KEY_F2,
           Gdk::ModifierType(0));
  game->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
  add_item(*game, "E_xit", sigc::mem_fun(*this, &MainWindow::on_quit));
  add_menu("_Game", *game);

  auto* help = Gtk::manage(new Gtk::Menu());
  add_item(*help, "_About Kablamo!", sigc::mem_fun(*this, &MainWindow::on_about));
  add_menu("_Help", *help);
}

void MainWindow::stop_timer()
{
  if (tick_.connected())
    tick_.disconnect();
}

bool MainWindow::on_tick()
{
  if (board_.phase() != Phase::playing)
    return false;
  if (seconds_ < 999)
    ++seconds_;
  hud_.set_seconds(seconds_);
  return true;
}

void MainWindow::sync_hud()
{
  hud_.set_remaining(board_.remaining());
  switch (board_.phase()) {
    case Phase::won:
      stop_timer();
      hud_.set_face(Face::won);
      break;
    case Phase::lost:
      stop_timer();
      hud_.set_face(Face::dead);
      break;
    case Phase::playing:
      if (!tick_.connected()) {
        seconds_ = 1;
        hud_.set_seconds(seconds_);
        tick_ = Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::on_tick),
                                               1000);
      }
      break;
    case Phase::ready:
    default:
      stop_timer();
      hud_.set_face(Face::smile);
      break;
  }
}

void MainWindow::on_changed()
{
  sync_hud();
}

void MainWindow::on_new()
{
  stop_timer();
  seconds_ = 0;
  board_.reset();
  field_.reset();
  hud_.reset();
}

void MainWindow::on_quit()
{
  hide();
}

void MainWindow::on_about()
{
  AboutDialog dlg(*this);
  dlg.run();
}

void MainWindow::on_pressing(bool down)
{
  if (board_.phase() == Phase::won || board_.phase() == Phase::lost)
    return;
  hud_.set_face(down ? Face::wow : Face::smile);
}

bool MainWindow::on_key_press_event(GdkEventKey* event)
{
  if (event && event->keyval == GDK_KEY_F2) {
    on_new();
    return true;
  }
  return Gtk::Window::on_key_press_event(event);
}

}  // namespace kablamo
