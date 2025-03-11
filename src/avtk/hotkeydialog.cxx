#include "hotkeydialog.hxx"
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <iostream>

HotkeyDialog::HotkeyDialog(int w, int h, int currentKey)
    : Fl_Window(w, h, "Assign New Hotkey"), hotkey(0) {
    this->set_modal(); // Prevent application closing without this window closing first
    current_key_label = new Fl_Box(10, 10, w - 20, 20, "Current key:");
    current_key_label->box(FL_NO_BOX);
    current_key_label->align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    
    // Concert currentKey to string if available
    if (currentKey) {
        current_key_str = static_cast<char>(currentKey);
    } else {
        current_key_label->hide();
        current_key_str = "No key set";
    }
    
    current_key_display = new Fl_Box(10, 30, w - 20, 30, current_key_str.c_str());
    current_key_display->box(FL_NO_BOX);
    current_key_display->align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    current_key_display->labelfont(FL_HELVETICA_BOLD);
    current_key_display->labelsize(14);

    instructions_box = new Fl_Box(10, 60, w - 20, 30, "Press new hotkey: \n(Esc to cancel)");
    instructions_box->box(FL_NO_BOX);
    instructions_box->align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

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
        if (Fl::event_key() == FL_Escape || Fl::event_key() == FL_Shift_L || Fl::event_key() == FL_Shift_R) {
            hide();
            return 1;
        }
        hotkey = Fl::event_key();
        //std::cout << "Hotkey captured: " << hotkey << std::endl;
        hide();
        return 1;
    
    default:
        break;
    }
    return Fl_Window::handle(event);
}

void HotkeyDialog::Cancel_CB(Fl_Widget*, void* v) {
    HotkeyDialog* dialog = (HotkeyDialog*)v;
    dialog->hide();
}
