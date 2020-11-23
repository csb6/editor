#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wall"
#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#pragma GCC diagnostic pop

#include "editor.hpp"
#include <cstring>
#include <string_view>
#include <algorithm>

Editor::Editor(Fl_Text_Buffer *edit_buffer,
	       int x, int y, int w, int h, Fl_Font font,
	       const char *filename)
    : Fl_Text_Editor(x, y, w, h), m_currFile(filename), m_font(font)
{
    buffer(edit_buffer);

    // Equivalent of grey23 in emacs
    color(fl_rgb_color(59, 59, 59));
    // Equivalent of cornsilk in emacs
    textcolor(fl_rgb_color(255, 248, 220));
    textfont(m_font);

    selection_color(FL_BLUE);
    cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    cursor_color(FL_WHITE);
    linenumber_width(Line_Number_Width);
    wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS,
	      Fl_Text_Display::WRAP_AT_COLUMN);

    open(m_currFile.c_str());
    change_label(m_currFile.c_str());
}

Editor::~Editor()
{}

bool Editor::operator==(const Editor *other) const
{
    return m_currFile == other->m_currFile && mBuffer == other->mBuffer;
}

void Editor::change_label(const char *newLabel)
{
     copy_label(newLabel);
     parent()->redraw();
}

int Editor::handle(int event)
{
    if(event == FL_KEYDOWN) {
	auto state = Fl::event_state();
	if(state == FL_COMMAND || state == FL_COMMAND + FL_SHIFT
	   || state == FL_ALT || state == FL_META)
	    return Fl_Text_Editor::handle(event);
	auto key_press = Fl::event_key();
        if(m_saved && (std::isalnum(key_press)
		       || std::ispunct(key_press)
		       || std::isspace(key_press)
		       || key_press == FL_Enter
		       || key_press == FL_BackSpace)) {
	    // Add * to show that file has changed
	    m_saved = false;
	    const auto *currLabel = label();
	    char *newLabel = new char[std::strlen(currLabel)+2];
	    std::strcpy(newLabel, currLabel);
	    std::strcat(newLabel, "*");
	    change_label(newLabel);
	    delete[] newLabel;
	}
    }
    return Fl_Text_Editor::handle(event);
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
    const char *currLabel = label();
    const auto currLabelLen = std::strlen(currLabel);
    if(currLabel[currLabelLen-1] == '*') {
	auto *newLabel = new char[currLabelLen];
	std::copy(currLabel, currLabel+currLabelLen-1, newLabel);
	newLabel[currLabelLen-1] = '\0';
        change_label(newLabel);
	delete[] newLabel;
    }
    return true;
}

void Editor::save_as(const char *filename)
{
    m_currFile = filename;
    m_saved = false;
    if(save()) {
	change_label(filename);
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
    change_label(filename);
}
