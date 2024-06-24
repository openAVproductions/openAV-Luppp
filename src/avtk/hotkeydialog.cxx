#include "hotkeydialog.hxx"
#include <FL/Fl.H>
#include <iostream>

HotkeyDialog::HotkeyDialog(int w, int h, const char* title, const char* message_text)
    : Fl_Window(w, h, title), hotkey(0) {
    message = new Fl_Box(10, 10, w - 20, 30, message_text);
    cancel_button = new Fl_Button(w / 2 - 40, h - 40, 80, 30, "Cancel");
    cancel_button->callback(Cancel_CB, this);
    end();
}

int HotkeyDialog::getHotkey() const {
    return hotkey;
}

int HotkeyDialog::handle(int event) {
    switch (event) {
    case FL_KEYDOWN:
        hotkey = Fl::event_key();
        //std::cout << "Hotkey captured: " << hotkey << std::endl;
        hide();
        return 1;
    case FL_SHORTCUT:
        if (Fl::event_key() == FL_Escape) {
            hide();
            return 1;
        }
        break;
    default:
        break;
    }
    return Fl_Window::handle(event);
}

void HotkeyDialog::Cancel_CB(Fl_Widget*, void* v) {
    HotkeyDialog* dialog = (HotkeyDialog*)v;
    dialog->hide();
}
