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

        Box topBox;
        topBox.append(menuButton);
        topBox.append(search);
        search.set_hexpand(true);
        menuButton.set_icon_name("view-more-symbolic");
        menuButton.set_margin(5);

        Popover popover;
        menuButton.set_popover(popover);

        Box popupBox(Orientation::VERTICAL);

        SpinButton *maxResultsField = new SpinButton();
        maxResultsField->set_range(1, 100);
        maxResultsField->set_increments(1, -1);
        maxResultsField->set_value(55);
        maxResultsField->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnMaxResultsChanged), maxResultsField));
        //  butPop->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::Close));
        Label lbl("Max results");
        popupBox.append(lbl);
        popupBox.append(*maxResultsField);

        CheckButton *exitAfterCopy = new CheckButton("exit after copy");
        exitAfterCopy->set_active(false);
        exitAfterCopy->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::ChangeExit));
        popupBox.append(*exitAfterCopy);

        CheckButton *compactMode = new CheckButton("compact");
        compactMode->set_active(false);
        compactMode->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::ChangeCompact));
        popupBox.append(*compactMode);

        popover.set_child(popupBox);
        mainBox.append(topBox);
        mainBox.append(scWindow);
        scWindow.set_vexpand(true);

        scWindow.set_child(contentBox);
        contentGrid.set_row_spacing(3);
        contentGrid.set_column_spacing(5);
        contentGrid.set_margin(17);
        search.set_margin(5);
        search.set_margin_start(0);
        search.signal_search_changed().connect(sigc::mem_fun(*this, &MainWindow::onSearchChanged));

        contentBox.set_orientation(Orientation::VERTICAL);
    }

protected:
    /*     void ChangeState(bool *state)
        {
            *state = !*state;
        } */

    void ChangeExit()
    {
        exitAfterCopy = !exitAfterCopy;
    }

    void ChangeCompact()
    {
        if (isCompact)
        {
            //      clearGrid();
            scWindow.set_child(contentBox);
            isCompact = false;
            onSearchChanged();
        }
        else
        {
            // clearBox();
            scWindow.set_child(contentGrid);
            isCompact = true;
            onSearchChanged();
        }
    }

    void OnMaxResultsChanged(SpinButton *sb)
    {
        maxResults = sb->get_value();
        onSearchChanged();
    }

    void onSearchChanged()
    {

        string searchText = search.get_text();

        int findCount = 0;

        if (searchText.length() > 0)
        {
            clearBox();
            clearGrid();
            for (int i = 0; i < db::length; i++)
            {
                if (findCount >= maxResults)
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
        if (!isCompact)
        {
            initButton(dbut, stringLeft, stringRight);
            contentBox.append(*dbut);
        }
        else
        {
            dbut->set_label(stringRight);
            contentGrid.attach(*dbut, collum++, row);
            if (collum >= 6)
            {
                collum = 0;
                row++;
            }
        }
        dbut->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::onClicked), dbut, stringLeft, stringRight));
    }

    void clearBox()
    {
        while (auto child = contentBox.get_first_child())
        {
            contentBox.remove(*child);
            delete child;
            replacedButton = nullptr;
        }
    }

    void clearGrid()
    {
        collum = 0;
        row = 0;
        while (auto child = contentGrid.get_first_child())
        {
            contentGrid.remove(*child);
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

        if (replacedButton != nullptr){
            if (!isCompact)
            initButton(replacedButton, prevL, prevR);
            else 
            replacedButton->set_label(prevR);
        }


        prevL = sL;
        prevR = sR;
        replacedButton = but;

        if (!isCompact)
            but->set_label("Copied✅");
        else
            but->set_label("✅");
        get_clipboard()->set_text(sR);
        replacedButton = but;
        if (exitAfterCopy)
        {

            Glib::signal_timeout().connect_once([this]()
                                                { close(); }, 50);
        }
    }

    SearchEntry search;
    MenuButton menuButton;
    Box contentBox;
    Grid contentGrid;
    Box mainBox;
    ScrolledWindow scWindow;

    Button *replacedButton = nullptr;

    string prevL, prevR;
    int maxResults = 55;

    bool exitAfterCopy = false;
    bool isCompact = false;

    int collum = 0;
    int row = 0;
};
