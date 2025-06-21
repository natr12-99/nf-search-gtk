#include <gtkmm.h>
#include <giomm/settings.h>
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
        set_decorated(false);

        set_child(mainBox);
        mainBox.set_orientation(Orientation::VERTICAL);
        contentBox.set_margin(5);

        Box topBox;
        topBox.append(menuButton);
        topBox.append(search);
        search.set_hexpand(true);
        menuButton.set_icon_name("view-more-symbolic");
        menuButton.set_margin(5);
        Button* closeButton = new Button();
        topBox.append(*closeButton);
        closeButton->set_icon_name("window-close-symbolic");
        closeButton->set_margin(5);
        closeButton->signal_clicked().connect([this]() {
            close();
        });

        Popover popover;
        menuButton.set_popover(popover);

        Box popupBox(Orientation::VERTICAL);

        SpinButton *maxResultsField = new SpinButton();
        maxResultsField->set_range(1, 100);
        maxResultsField->set_increments(1, -1);
        maxResultsField->signal_changed().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::OnMaxResultsChanged), maxResultsField));
        Label lbl("Max results");
        popupBox.append(lbl);
        popupBox.append(*maxResultsField);

        CheckButton *exitAfterCopyb = new CheckButton("exit after copy");
        exitAfterCopyb->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::ChangeExit));
        popupBox.append(*exitAfterCopyb);

        CheckButton *compactMode = new CheckButton("compact");
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

        settings = Gio::Settings::create("org.ntri12.nf-search");
        maxResultsField->set_value(settings->get_int("max-results"));
        exitAfterCopyb->set_active(settings->get_boolean("exit-after-copy"));
        compactMode->set_active(settings->get_boolean("compact"));
    }

protected:
    void ChangeExit()
    {
        exitAfterCopy = !exitAfterCopy;
        settings->set_boolean("exit-after-copy", exitAfterCopy);
    }

    void ChangeCompact()
    {
        if (isCompact)
        {
            scWindow.set_child(contentBox);
            isCompact = false;
            onSearchChanged();
        }
        else
        {
            scWindow.set_child(contentGrid);
            isCompact = true;
            onSearchChanged();
        }
        settings->set_boolean("compact", isCompact);
    }

    void OnMaxResultsChanged(SpinButton *sb)
    {
        maxResults = sb->get_value();
        onSearchChanged();
        settings->set_int("max-results", maxResults);
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
            contentGrid.attach(*dbut, column++, row);
            if (column >= 6)
            {
                column = 0;
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
        column = 0;
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

        if (replacedButton != nullptr)
        {
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
    Glib::RefPtr<Gio::Settings> settings;

    int column = 0;
    int row = 0;
};
