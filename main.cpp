#include <cstring> //for strncmp()
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
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

void save_buffer(Fl_Widget*, void *data)
{
    auto *editor = static_cast<Editor*>(data);
    editor->save();
}

void save_prompt(Fl_Widget*, void *data)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	auto *editor = static_cast<Editor*>(data);
	editor->save_as(chooser.filename());
    }
}

void open_prompt(Fl_Widget*, void *data)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	auto *editor = static_cast<Editor*>(data);
	editor->open(chooser.filename());
    }
}

void prev_word(Fl_Widget*, void *data)
{
    auto *editor = static_cast<Editor*>(data);
    editor->previous_word();
}

void next_word(Fl_Widget*, void *data)
{
    auto *editor = static_cast<Editor*>(data);
    editor->next_word();
}

int main(int argc, char **argv)
{
    // macOS won't return from Fl::run() otherwise
    #ifdef __APPLE__
        fl_mac_quit_early = 0;
    #endif
    Fl::scheme("gtk+"); //none / plastic / gleam
    const Fl_Font defaultFont = setupFonts();
    Fl::add_handler(disable_escape_handler);
    Fl::add_handler(ask_before_closing);

    std::unique_ptr<Fl_Text_Buffer> buffer(new Fl_Text_Buffer(Initial_Buffer_Size));
    std::unique_ptr<Fl_Window> window(new Fl_Window(Width, Width));
    window->begin();
    auto *editor = new Editor(buffer.get(), 0, EditorY, Width, Width, defaultFont);
        auto *menu = new Fl_Sys_Menu_Bar(0, 0, Width, 20);
	    menu->insert(0, "View", 0, 0, 0, FL_SUBMENU);
	        menu->insert(1, "Next Word", FL_COMMAND+'f', next_word, editor);
	        menu->insert(1, "Prior Word", FL_COMMAND+'b', prev_word, editor);
	    menu->insert(0, "File", 0, 0, 0, FL_SUBMENU);
	        menu->insert(1, "Save", FL_COMMAND+'s', save_buffer, editor);
		menu->insert(1, "Save As", FL_COMMAND+FL_SHIFT+'s', save_prompt,
			     editor);
		menu->insert(1, "Open", FL_COMMAND+'o', open_prompt, editor);
	window->resizable(editor);
    window->end();

    window->show(argc, argv);
    return Fl::run();
}
