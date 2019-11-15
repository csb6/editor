#include <iostream>
#include <cstring> //for strncmp()
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "editor.hpp"

Editor::Editor(int x, int y, const std::string filename)
    : m_editor(x, y, Editor::Width, Editor::Width), m_filename(filename)
{
    buffer(new Fl_Text_Buffer(Initial_Buffer_Size));

    cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    linenumber_width(Line_Number_Width);
    wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS,
	      Fl_Text_Display::WRAP_AT_COLUMN);

    for(const auto &binding : Editor_Bindings) {
	m_key_bindings[binding.key] = binding.callback;
    }
    initFonts();
}

void Editor::initFonts()
{
    Fl_Font curr = 0;
    // Search for user-specified default font; try to set it
    while(Fl::get_font(curr) != nullptr) {
	if(strncmp(Fl::get_font(curr), Editor::Font,
		   sizeof(Editor::Font) / sizeof(char)) != 0) {
	    Fl::set_font(curr, Editor::Font);
	    m_font = curr;
	    return;
	}
	++curr;
    }
}

void Editor::save(const char *filename)
{
    auto *buf = buffer();
    buf->savefile(filename, buf->length());
}

void Editor::save_as()
{
    auto *chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    int status = chooser->show();
    if(status != 0) {
	// User fails to pick a file
	delete chooser;
	return;
    }
    save(chooser->filename());
    delete chooser;
}

int Editor::handle(int event) {
    // Only override Fl_Text_Editor::handle() if the event is for a key
    // press that maps to one of this Editor's bindings
    const int state = Fl::event_state();
    if(event == FL_KEYBOARD && (state == FL_COMMAND || state == FL_CONTROL)) {
	const std::pair<int,int> key(Fl::event_key(), state);
	if(m_key_bindings.count(key) >= 1) {
	    m_key_bindings[key](this);
	    return 1;
	}
    }
    return Fl_Text_Editor::handle(event);
}

/* Disables default behavior of pressing ESC causing the program to close */
static int disable_escape_handler(int event)
{
    if(event == FL_SHORTCUT && Fl::event_key() == FL_Escape) return 1;
    return 0;
}

int main(int argc, char **argv)
{
    Fl::scheme("gtk+"); //none / plastic / gleam
    Fl::add_handler(disable_escape_handler);
    /*const Fl_Text_Display::Style_Table_Entry Style_Table[] = {
	{ FL_BLACK,       font, FL_NORMAL_SIZE  }, // Plain text
	{ FL_DARK_YELLOW, font, FL_NORMAL_SIZE  } // Comments
	};*/
    auto *window = new Fl_Window(Editor::Width, Editor::Width);
    window->begin();
        auto *editor = new Editor(0, 0, "test.txt");
	window->resizable(editor);
    window->end();

    window->show(argc, argv);
    return Fl::run();
}
