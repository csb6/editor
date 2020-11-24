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
	       const char *file_name)
    : Fl_Text_Editor(x, y, w, h), m_font(font)
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

    set_filename(file_name);
    open(file_name);
    set_saved();
}

Editor::~Editor()
{}

bool Editor::operator==(const Editor *other) const
{
    return m_currFile == other->m_currFile && mBuffer == other->mBuffer;
}

void Editor::set_saved()
{
    m_saved = true;
    // Exclude leading '*'
    point_label(m_currFile.data() + 1);
}

void Editor::set_unsaved()
{
    m_saved = false;
    // Include leading '*'
    point_label(m_currFile.data());;
}

void Editor::set_filename(const char *new_name)
{
    m_currFile.clear();
    m_currFile = "*";
    m_currFile += new_name;
}

const char* Editor::filename() const
{
    return m_currFile.data() + 1;
}

void Editor::point_label(const char *newLabel)
{
     label(newLabel);
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
            set_unsaved();
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
    int status = buf->savefile(filename(), buf->length());
    if(status != 0) {
        fl_alert("Error: could not save file");
	return false;
    } else {
        set_saved();
        return true;
    }
}

bool Editor::save_as(const char *file_name)
{
    auto *buf = buffer();
    int status = buf->savefile(file_name, buf->length());
    if(status != 0) {
        fl_alert("Error: could not save file");
        return false;
    } else {
        set_filename(file_name);
        set_saved();
        return true;
    }
}

bool Editor::open(const char *file_name)
{
    auto *buf = buffer();
    int status = buf->loadfile(file_name);
    if(status != 0) {
	fl_alert("Error: could not load file");
        return false;
    } else {
        set_filename(file_name);
        set_saved();
        return true;
    }
}
