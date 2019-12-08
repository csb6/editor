#include "editor.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>

Editor::Key_Binding Editor::Key_Bindings[] = {
    {'b', FL_COMMAND, [](Editor *editor) {
			  editor->previous_word();
		      }},
    // Move cursor forward One Word
    {'f', FL_COMMAND, [](Editor *editor) {
			  editor->next_word();
		      }},
    // Save current buffer
    {'s', FL_COMMAND, [](Editor *editor) {
			  save(editor, "test.txt");
		      }},
    // Save As current buffer
    {'s', FL_COMMAND + FL_SHIFT, [](Editor *editor) {
				     save_as(editor);
				 }},
    {'o', FL_COMMAND, [](Editor *editor) {
			      
		      }}
};

void save(Editor *editor, const char *filename)
{
    auto *buf = editor->buffer();
    buf->savefile(filename, buf->length());
}

void save_as(Editor *editor)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    int status = chooser.show();
    if(status != 0) {
	// User fails to pick a file
	return;
    }
    save(editor, chooser.filename());
}


Editor::Editor(Fl_Text_Buffer *edit_buffer,
	       int x, int y, int w, int h)
    : Fl_Text_Editor(x, y, w, h)
{
    buffer(edit_buffer);
    
    cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    linenumber_width(Line_Number_Width);
    wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS,
	      Fl_Text_Display::WRAP_AT_COLUMN);
}

Editor::~Editor()
{
    // mBuffer is private buffer variable; see Fl_Text_Display's header
    delete mBuffer;
}

int Editor::handle(int event)
{
    switch(event) {
    case FL_KEYDOWN: {
	if(Fl::event_state() == FL_COMMAND
	   || Fl::event_state() == FL_COMMAND + FL_SHIFT) {
	    auto state = Fl::event_state();
	    auto key = Fl::event_key();
	    for(auto &each : Editor::Key_Bindings) {
		if(each.key == key && each.state == state) {
		    each.function(this);
		    return 1;
		}
	    }
	}
	return Fl_Text_Editor::handle(event);
    }	
    default:
	return Fl_Text_Editor::handle(event);
    }
    return 0;
}
