#include "window.cpp"

int main(int argc, char **argv)
{
    auto app = Gtk::Application::create("org.ntri12.nf-search");
    return app->make_window_and_run<MainWindow>(argc, argv);
}
