// Ignore internal FLTK warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wall"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Tabs.H>
#pragma GCC diagnostic pop

#include "editor.hpp"
#include <cstring> //for strncmp()
#include <memory>
#include <algorithm>
#include <vector>

static Fl_Font setupFonts()
{
    Fl_Font curr = 0;
    // Search for user-specified default font; try to set it
    while(Fl::get_font(curr) != nullptr) {
	if(strncmp(Fl::get_font(curr), Font,
		   std::size(Font)) != 0) {
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

static void update_current_tab(Fl_Widget *w, void *data);

class TabManager {
private:
    std::vector<std::unique_ptr<Fl_Text_Buffer>> m_buffers;
    std::vector<Editor*> m_editors;
    Fl_Tabs *m_tabs;
    std::size_t m_currIndex = 0;
    Fl_Font m_font;
public:
    TabManager(Fl_Tabs *parent, Fl_Font font)
	: m_tabs(parent), m_font(font)
    {
	m_tabs->begin();
            m_buffers.push_back(std::make_unique<Fl_Text_Buffer>(Initial_Buffer_Size));
	    m_editors.push_back(new Editor(m_buffers[0].get(), 0, EditorY, Width,
					   EditorHeight, m_font));
	m_tabs->end();
	m_tabs->when(FL_WHEN_CHANGED | FL_WHEN_RELEASE);
	m_tabs->callback(update_current_tab, this);
    }

    void curr_editor(const Editor *selected)
    {
	auto it = std::find(m_editors.begin(), m_editors.end(), selected);
	if(it != m_editors.end()) {
	    m_currIndex = std::distance(m_editors.begin(), it);
	}
    }

    Editor* curr_editor() { return m_editors[m_currIndex]; }

    void add_tab(const char *filename)
    {
	m_buffers.push_back(std::make_unique<Fl_Text_Buffer>(Initial_Buffer_Size));
	m_currIndex = m_buffers.size() - 1;
	m_tabs->begin();
        m_editors.push_back(new Editor(m_buffers[m_currIndex].get(), 0, EditorY,
				       Width, EditorHeight, m_font, filename));
	m_tabs->end();
    }

    void remove_tab(const Fl_Widget *tab) {
	if(tab == nullptr || m_editors.size() <= 1)
	    // Don't remove non-existent or the only remaining tab
	    return;
	auto it = std::find(m_editors.begin(), m_editors.end(), tab);
	if(it != m_editors.end()) {
	    auto index = std::distance(m_editors.begin(), it);
	    m_tabs->remove(static_cast<int>(index));
	    m_editors.erase(it);
	    m_buffers.erase(m_buffers.begin()+index);
	    if(m_currIndex >= index)
		--m_currIndex;
	}
    }
};

static void update_current_tab(Fl_Widget *w, void *data)
{
    auto *tabs = static_cast<Fl_Tabs*>(w);
    auto *manager = static_cast<TabManager*>(data);
    if(Fl::event_button() == FL_LEFT_MOUSE) {
	// Select a tab with primary click
	auto *selected_editor = static_cast<Editor*>(tabs->value());
	manager->curr_editor(selected_editor);
    } else if(Fl::event_button() == FL_RIGHT_MOUSE) {
	// Close a tab with secondary click
	Fl_Widget *selected = tabs->which(Fl::event_x(), Fl::event_y());
	manager->remove_tab(selected);
    }
}


static void save_buffer(Fl_Widget*, void *data)
{
    Editor *editor = static_cast<TabManager*>(data)->curr_editor();
    editor->save();
}

static void save_prompt(Fl_Widget*, void *data)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	Editor *editor = static_cast<TabManager*>(data)->curr_editor();
	editor->save_as(chooser.filename());
    }
}

static void open_prompt(Fl_Widget*, void *data)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	Editor *editor = static_cast<TabManager*>(data)->curr_editor();
	editor->open(chooser.filename());
    }
}

static void open_tab(Fl_Widget*, void *data)
{
    Fl_Native_File_Chooser chooser(Fl_Native_File_Chooser::BROWSE_FILE);
    if(chooser.show() == 0) {
	// User successfully picks a file
	auto *manager = static_cast<TabManager*>(data);
	manager->add_tab(chooser.filename());
    }
}

static void prev_word(Fl_Widget*, void *data)
{
    Editor *editor = static_cast<TabManager*>(data)->curr_editor();
    editor->previous_word();
}

static void next_word(Fl_Widget*, void *data)
{
    Editor *editor = static_cast<TabManager*>(data)->curr_editor();
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

    auto *window = new Fl_Window(Width, Width);
    window->copy_label("Editor");
    window->begin();
        auto *tabs = new Fl_Tabs(0, TabY, Width, Width);
	TabManager manager(tabs, defaultFont);

        auto *menu = new Fl_Sys_Menu_Bar(0, 0, Width, MenuHeight);
	    menu->insert(0, "View", 0, 0, 0, FL_SUBMENU);
	        menu->insert(1, "Next Word", FL_COMMAND+'f',
			     next_word, &manager);
		menu->insert(1, "Prior Word", FL_COMMAND+'b',
			     prev_word, &manager);
	    menu->insert(0, "File", 0, 0, 0, FL_SUBMENU);
	        menu->insert(1, "Save", FL_COMMAND+'s',
			     save_buffer, &manager);
		menu->insert(1, "Save As", FL_COMMAND+FL_SHIFT+'s',
			     save_prompt, &manager);
		menu->insert(1, "Open in New Tab", FL_COMMAND+FL_SHIFT+'o',
			     open_tab, &manager);
		menu->insert(1, "Open", FL_COMMAND+'o',
			     open_prompt, &manager);

        window->resizable(tabs);
    window->end();
    window->show(argc, argv);
    auto status = Fl::run();
    // Window must be explicitly deleted before the buffers in TabManager; otherwise
    // destructors of those buffers will print out error messages
    delete window;
    return status;
}
