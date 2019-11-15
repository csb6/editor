#include <cstring> //for strncmp()
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "editor.hpp"

static Fl_Font initFonts()
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

static Fl_Text_Buffer* initEditor(Fl_Text_Editor *editor)
{
    auto *buffer = new Fl_Text_Buffer(Initial_Buffer_Size);
    editor->buffer(buffer);
    editor->cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    editor->linenumber_width(Line_Number_Width);
    editor->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS,
		      Fl_Text_Display::WRAP_AT_COLUMN);

    for(const auto &binding : Editor_Bindings) {
	editor->add_key_binding(binding.key, binding.state, binding.function);
    }
    return buffer;
}

static void save(Fl_Text_Editor *editor, const char *filename)
{
    auto *buf = editor->buffer();
    buf->savefile(filename, buf->length());
}

static void save_as(Fl_Text_Editor *editor)
{
    auto *chooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    int status = chooser->show();
    if(status != 0) {
	// User fails to pick a file
	delete chooser;
	return;
    }
    save(editor, chooser->filename());
    delete chooser;
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
    Fl::scheme("gtk+"); //none / plastic / gleam
    // macOS won't return from Fl::run() otherwise
    #ifdef __APPLE__
      fl_mac_quit_early = 0;
    #endif
    Fl::add_handler(disable_escape_handler);
    Fl::add_handler(ask_before_closing);
    /*const Fl_Text_Display::Style_Table_Entry Style_Table[] = {
	{ FL_BLACK,       font, FL_NORMAL_SIZE  }, // Plain text
	{ FL_DARK_YELLOW, font, FL_NORMAL_SIZE  } // Comments
	};*/
    auto *window = new Fl_Window(Width, Width);
    const Fl_Font font = initFonts();
    window->begin();
        auto *editor = new Fl_Text_Editor(0, 0, Width, Width);
        editor->buffer(new Fl_Text_Buffer(Initial_Buffer_Size));
	window->resizable(editor);
	auto *buffer = initEditor(editor);
    window->end();

    window->show(argc, argv);
    int status = Fl::run();
    delete buffer;
    return status;
}
