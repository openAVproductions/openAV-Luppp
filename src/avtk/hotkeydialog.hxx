#ifndef HOTKEYDIALOG_H
#define HOTKEYDIALOG_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

class HotkeyDialog : public Fl_Window {
public:
    HotkeyDialog(int w, int h, const char* title = 0, const char* message_text = 0);
    int getHotkey() const;

protected:
    int handle(int event) override;

private:
    static void Cancel_CB(Fl_Widget*, void* v);

    Fl_Box* message;
    Fl_Button* cancel_button;
    int hotkey;
};

#endif // HOTKEYDIALOG_H
