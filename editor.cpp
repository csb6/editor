#include "editor.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

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
			  editor->save();
		      }},
    // Save As current buffer
    {'s', FL_COMMAND + FL_SHIFT, [](Editor *editor) {
				     save_prompt(editor);
				 }},
    {'o', FL_COMMAND, [](Editor *editor) {
			  open_prompt(editor);
		      }}
};

void save_prompt(Editor *editor)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    int status = chooser.show();
    if(status != 0) {
	// User fails to pick a file
	return;
    }
    editor->save_as(chooser.filename());
}

void open_prompt(Editor *editor)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_FILE);
    int status = chooser.show();
    if(status != 0) {
	// User fails to pick a file
	return;
    }
    editor->open(chooser.filename());
}


Editor::Editor(Fl_Text_Buffer *edit_buffer,
	       int x, int y, int w, int h)
    : Fl_Text_Editor(x, y, w, h), m_currFile("test.txt")
{
    buffer(edit_buffer);
    
    cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    linenumber_width(Line_Number_Width);
    wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS,
	      Fl_Text_Display::WRAP_AT_COLUMN);

    open(m_currFile.c_str());
    window()->label(m_currFile.c_str());
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

bool Editor::save()
{
    auto *buf = buffer();
    int status = buf->savefile(m_currFile.c_str(), buf->length());
    if(status != 0) {
        fl_alert("Error: could not save file");
	return false;
    }
    return true;
}

void Editor::save_as(const char *filename)
{
    m_currFile = filename;
    if(save()) {
	window()->label(filename);
    }
}

void Editor::open(const char *filename)
{
    auto *buf = buffer();
    int status = buf->loadfile(filename);
    if(status != 0) {
	fl_alert("Error: could not load file");
	return;
    }
    m_currFile = filename;
    window()->label(filename);
}
