#include <iostream>
#include <gtkmm.h>
#include "database.cpp"

using namespace Gtk;
using namespace std;
class MainWindow : public Window
{
public:
    MainWindow()
    {
        set_title("Nerd font search");
        set_default_size(360, 400);
        set_resizable(false);

        set_child(mainBox);
        mainBox.set_orientation(Orientation::VERTICAL);
        contentBox.set_margin(5);

        mainBox.append(search);
        mainBox.append(scWindow);
        scWindow.set_vexpand(true);

        scWindow.set_child(contentBox);

        search.set_margin(5);
        search.signal_search_changed().connect(sigc::mem_fun(*this, &MainWindow::onSearchChanged));

        contentBox.set_orientation(Orientation::VERTICAL);
    }

protected:
    void onSearchChanged()
    {
        string searchText = search.get_text();

        int findCount = 0;

        if (searchText.length() > 0)
        {
            clearSearch();
            for (int i = 0; i < db::length; i++)
            {
                if (findCount > 55)
                    break;

                if (db::data[i].find(searchText) != string::npos)
                {
                    makeButton(db::data[i]);
                    findCount++;
                }
            }
        }
    }

    void makeButton(string s)
    {
        auto dbut = new Button();
        dbut->set_margin_top(3);
        dbut->set_margin_bottom(3);

        auto pos = s.find(':');
        string stringLeft = s.substr(0, pos);
        string stringRight = s.substr(pos + 1);

        initButton(dbut, stringLeft, stringRight);

        dbut->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::onClicked), dbut, stringLeft, stringRight));
        contentBox.append(*dbut);
    }

    void clearSearch()
    {
        while (auto child = contentBox.get_first_child())
        {
            contentBox.remove(*child);
            delete child;
            replacedButton = nullptr;
        }
    }

    void initButton(Button *but, string sL, string sR)
    {
        Box boxt;
        but->set_child(boxt);
        Label text1(sL);
        Label text2(sR);
        boxt.prepend(text1);
        boxt.append(text2);
        text2.set_halign(Align::END);
        text2.set_hexpand(true);
    }

    void onClicked(Button *but, string sL, string sR)
    {
        if (but == replacedButton)
        {
            get_clipboard()->set_text(sR);
            return;
        }

        if (replacedButton != nullptr)
        {
            initButton(replacedButton, prevL, prevR);
        }

        prevL = sL;
        prevR = sR;
        replacedButton = but;
        but->set_label("Copied✅");

        get_clipboard()->set_text(sR);

        replacedButton = but;
    }

    SearchEntry search;
    Box contentBox;
    Box mainBox;
    ScrolledWindow scWindow;

    Button *replacedButton = nullptr;

    string prevL, prevR;
};
