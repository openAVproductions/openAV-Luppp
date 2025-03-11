#ifndef HOTKEYDIALOG_H
#define HOTKEYDIALOG_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <string>

class HotkeyDialog : public Fl_Window {
public:
    HotkeyDialog(int w, int h, int currentKey);
    int getHotkey() const;

protected:
    int handle(int event) override;

private:
    static void Cancel_CB(Fl_Widget*, void* v);

    Fl_Box* current_key_label;
    Fl_Box* current_key_display;
    Fl_Box* instructions_box;
    Fl_Button* cancel_button;
    int hotkey;

    std::string message_text;
    std::string current_key_str;
};

#endif // HOTKEYDIALOG_H
