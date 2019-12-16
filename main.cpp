#include <cstring> //for strncmp()
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include "editor.hpp"
#include <memory>

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

template<void(*Func)(Editor*)>
static void editor_callback(Fl_Widget*, void *editor_obj)
{
    Func(static_cast<Editor*>(editor_obj));
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
    auto *buffer = new Fl_Text_Buffer(Initial_Buffer_Size);
    window->begin();
        auto *editor = new Editor(buffer, 0, EditorY, Width, Width);
        auto *menu = new Fl_Sys_Menu_Bar(0, 0, Width, 20);
	    menu->insert(0, "Edit", 0, 0, 0, FL_SUBMENU);
	    menu->insert(0, "File", 0, 0, 0, FL_SUBMENU);
	    menu->insert(1, "Save", 0, editor_callback<save_buffer>, editor);
	window->resizable(editor);
    window->end();

    window->show(argc, argv);
    int status = Fl::run();
    delete window;
    delete buffer;
    return status;
}
