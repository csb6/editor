#include "editor.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <cstring>
#include <string_view>
#include <algorithm>

void search_prompt(Editor *editor)
{
    const auto *searchString = fl_input("Search (case-insensitive):", "");
    if(searchString != nullptr && strncmp(searchString, "", 1) != 0) {
        editor->search_forward(editor->insert_position(), searchString);
    }
}

void style_parse(const std::string_view text, std::string &style)
{
    if(text.size() < 1) return;
    int i = 0;
    char currStyle = style[0];
    while(i < text.size()) {
	if(currStyle == 'A') {
	    if(text[i] == '/' && i < text.size()-1 && text[i+1] == '*') {
		currStyle = 'B';
		style[i++] = currStyle;
		style[i++] = currStyle;
	    } else {
		i++;
	    }
	} else if(currStyle == 'B') {
	    if(text[i] == '*' && i < text.size()-1 && text[i+1] == '/') {
		style[i++] = currStyle;
		style[i++] = currStyle;
		currStyle = 'A';
	    } else {
		style[i++] = currStyle;
	    }
	} else {
	    ++i;
	}
    }
}

void style_update(int pos, int nInserted, int nDeleted, int nRestyled,
		  const char *deletedText, void *data)
{
    auto *editor = static_cast<Editor*>(data);
    auto *stylebuf = editor->m_stylebuf;
    auto *buf = editor->buffer();
    if(nInserted > 0 && nDeleted == 0) {
	// Selection change
	stylebuf->unselect();
	return;
    }
    if(nInserted > 0) {
	// Insert characters into style buffer
	char *style = new char[nInserted + 1];
	std::fill_n(style, nInserted, 'A');
	style[nInserted+1] = '\0';
	stylebuf->replace(pos, pos + nDeleted, style);
	delete[] style;
    } else {
	// Delete chars from style buffer
	stylebuf->remove(pos, pos + nDeleted);
    }

    stylebuf->select(pos, pos + nInserted - nDeleted);

    auto start = buf->line_start(pos);
    auto end = buf->line_end(pos + nInserted - nDeleted);
    char *text = buf->text_range(start, end);
    std::string_view text_view(text, end - start);
    std::string style(stylebuf->text_range(start, end));
    auto last = style[end - start - 1];
    style_parse(text_view, style);

    stylebuf->replace(start, end, style.c_str());
    editor->redisplay_range(start, end);
    if(last != style[end - start - 1]) {
	free(text);

	end = buf->length();
	text = buf->text_range(start, end);
	text_view = text;
	style = stylebuf->text_range(start, end);
	style_parse(text_view, style);
	stylebuf->replace(start, end, style.c_str());
	editor->redisplay_range(start, end);
    }
    free(text);
}


Editor::Editor(Fl_Text_Buffer *edit_buffer,
	       int x, int y, int w, int h, Fl_Font defaultFont)
    : Fl_Text_Editor(x, y, w, h), m_currFile("test.txt"),
      m_font(defaultFont), m_stylebuf(new Fl_Text_Buffer(edit_buffer->length()))
{
    buffer(edit_buffer);
    
    // Equivalent of grey23 in emacs
    // Default text color is equivalent of cornsilk in emacs
    color(fl_rgb_color(59, 59, 59));
    highlight_data(m_stylebuf, m_styletable, sizeof(m_styletable) / sizeof(m_styletable[0]),
		   0, [](int pos, void *data) {
			  auto *stylebuf = static_cast<Fl_Text_Buffer*>(data);
			  stylebuf->replace(pos, pos+1, "A");
		      }, m_stylebuf);
    //mBuffer->add_modify_callback(style_update, this);

    selection_color(FL_BLUE);
    cursor_style(Fl_Text_Display::HEAVY_CURSOR);
    cursor_color(FL_WHITE);
    linenumber_width(Line_Number_Width);
    wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS,
	      Fl_Text_Display::WRAP_AT_COLUMN);

    open(m_currFile.c_str());
    //style_update(0, mBuffer->length(), 0, 0, nullptr, this);
    change_label(m_currFile.c_str());
}

Editor::~Editor()
{
    delete m_stylebuf;
}

void Editor::change_label(const char *newLabel)
{
     window()->copy_label(newLabel);
     copy_label(newLabel);
     parent()->redraw();
}

int Editor::handle(int event)
{
    if(event == FL_KEYDOWN) {
	auto state = Fl::event_state();
	if(state == FL_COMMAND || state == FL_ALT || state == FL_META)
	    return Fl_Text_Editor::handle(event);
	auto key_press = Fl::event_key();
        if(m_saved && (std::isalnum(key_press)
		       || std::ispunct(key_press)
		       || std::isspace(key_press)
		       || key_press == FL_Enter
		       || key_press == FL_BackSpace)) {
	    // Add * to show that file has changed
	    m_saved = false;
	    const auto *currLabel = window()->label();
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
    const char *currLabel = window()->label();
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

void Editor::search_forward(int startPos, const char *searchString)
{
    int *foundPos = nullptr;
    mBuffer->search_forward(startPos, searchString, foundPos);
    if(foundPos != nullptr) {
	mBuffer->highlight_position(foundPos, foundPos+1);
    }
}
