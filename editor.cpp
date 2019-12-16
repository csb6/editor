#include "editor.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <cstring>
#include <iostream>

const Editor::Key_Binding Editor::Key_Bindings[] = {
    // Move cursor back one word
    {'b', FL_COMMAND, [](Editor *editor) {
			  editor->previous_word();
		      }},
    // Move cursor forward one word
    {'f', FL_COMMAND, [](Editor *editor) {
			  editor->next_word();
		      }},
    // Save current buffer
    {'s', FL_COMMAND, save_buffer},
    // Save As current buffer
    {'s', FL_COMMAND + FL_SHIFT, save_prompt},
    // Open file into buffer
    {'o', FL_COMMAND, open_prompt},
    // Search for string
    {'s', FL_ALT, search_prompt}
};

const Editor::Style_Table_Entry styles[] = {
    // Font Color | Font | Font Size
    { FL_YELLOW } // 'A' - Default style
};

void save_buffer(Editor *editor)
{
    editor->save();
}

void save_prompt(Editor *editor)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	editor->save_as(chooser.filename());;
    }
}

void open_prompt(Editor *editor)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	editor->open(chooser.filename());
    }
}

void search_prompt(Editor *editor)
{
    const auto *searchString = fl_input("Search (case-insensitive):", "");
    if(searchString != nullptr && strncmp(searchString, "", 1) != 0) {
        editor->search_forward(editor->insert_position(), searchString);
    }
}


Editor::Editor(Fl_Text_Buffer *edit_buffer,
	       int x, int y, int w, int h)
    : Fl_Text_Editor(x, y, w, h), m_currFile("test.txt")
{
    buffer(edit_buffer);

    // Equivalent of grey23 in emacs
    color(fl_rgb_color(59, 59, 59));
    // Equivalent of cornsilk in emacs
    textcolor(fl_rgb_color(250, 244, 219));
    textsize(13);
    selection_color(FL_BLUE);
    cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    cursor_color(FL_WHITE);
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
	auto state = Fl::event_state();
	if(state == FL_COMMAND
	   || state == FL_COMMAND + FL_SHIFT
	   || state == FL_ALT) {
	    auto key = Fl::event_key();
	    for(auto &each : Editor::Key_Bindings) {
		if(each.key == key && each.state == state) {
		    each.function(this);
		    return 1;
		}
	    }
	}
	auto key_press = Fl::event_key();
	if(m_saved && (std::isalnum(key_press)
		       || std::ispunct(key_press)
		       || std::isspace(key_press)
		       || key_press == FL_Enter)) {
	    // Add * to show that file has changed
	    m_saved = false;
	    const auto *currLabel = window()->label();
	    char *newLabel = new char[std::strlen(currLabel)+2];
	    std::strcpy(newLabel, currLabel);
	    std::strcat(newLabel, "*");
	    window()->copy_label(newLabel);
	    delete[] newLabel;
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
    if(m_saved) {
	return false;
    }
    auto *buf = buffer();
    int status = buf->savefile(m_currFile.c_str(), buf->length());
    if(status != 0) {
        fl_alert("Error: could not save file");
	return false;
    }
    m_saved = true;
    const auto *currLabel = window()->label();
    auto labelLen = std::strlen(currLabel);
    if(currLabel[labelLen-1] == '*'
       && m_currFile.back() != '*') {
	auto *newLabel = new char[labelLen-1];
	std::strncat(newLabel, currLabel, labelLen-1);
        window()->copy_label(newLabel);
	delete[] newLabel;
    }
    return true;
}

void Editor::save_as(const char *filename)
{
    m_currFile = filename;
    m_saved = false;
    if(save()) {
	window()->copy_label(filename);
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

void Editor::search_forward(int startPos, const char *searchString)
{
    int *foundPos = nullptr;
    mBuffer->search_forward(startPos, searchString, foundPos);
    if(foundPos != nullptr) {
	mBuffer->highlight_position(foundPos, foundPos+1);
    }
}
