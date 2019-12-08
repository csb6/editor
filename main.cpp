#include <cstring> //for strncmp()
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include "editor.hpp"

static Fl_Font setupFonts()
{
    Fl_Font curr = 0;
    // Search for user-specified default font; try to set it
    while(Fl::get_font(curr) != nullptr) {
	if(strncmp(Fl::get_font(curr), Font,
		   sizeof(Font) / sizeof(char)) != 0) {
	    Fl::set_font(curr, Font);
	    return curr;
	}
	++curr;
    }
    return 0;
}

/* Disables default behavior of pressing ESC causing the program to close */
static int disable_escape_handler(int event)
{
    if(event == FL_SHORTCUT && Fl::event_key() == FL_Escape) return 1;
    return 0;
}

/* Confirms that the user want to quit the editor */
static int ask_before_closing(int event)
{
    // "Yes" -> 0, "No" -> 1
    if(event == FL_SHORTCUT && Fl::event_state() == FL_COMMAND
       && Fl::event_key() == 'q'
       && fl_choice("Are you sure you want to quit?", "Yes", "No", 0) != 0)
	return 1;
    return 0;
}

int main(int argc, char **argv)
{
    // macOS won't return from Fl::run() otherwise
    #ifdef __APPLE__
        fl_mac_quit_early = 0;
    #endif
    Fl::scheme("gtk+"); //none / plastic / gleam
    setupFonts();
    Fl::add_handler(disable_escape_handler);
    Fl::add_handler(ask_before_closing);
    
    auto *window = new Fl_Window(Width, Width);
    window->begin();
        auto *buffer = new Fl_Text_Buffer(Initial_Buffer_Size);
        auto *editor = new Editor(buffer, 0, 0, Width, Width);
	window->resizable(editor);
    window->end();

    window->show(argc, argv);
    int status = Fl::run();
    delete window;
    delete buffer;
    return status;
}
