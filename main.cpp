#include "window.cpp"

int main(int argc, char **argv)
{
    auto app = Gtk::Application::create("ntr12.tst");
    return app->make_window_and_run<MainWindow>(argc, argv);
}
