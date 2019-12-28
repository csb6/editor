#ifndef EDITOR_APP_H
#define EDITOR_APP_H
#include <FL/Fl_Text_Editor.H>
#include <string>

constexpr int Width = 500;
constexpr int EditorHeight = Width - 30;
constexpr int MenuHeight = 20;
#ifdef __APPLE__
    // macOS menu is in a taskbar, so don't need to make room for it in the
    // main window above the editor widget
    constexpr int TabY = 0;
#else
    constexpr int TabY = 20;
#endif
constexpr int EditorY = TabY + 30;
// Name of editor font
constexpr char Font[] = "Monaco";
// Width of column on side of screen that contains line numbers
constexpr int Line_Number_Width = 30; // in px
// Size of editor buffer when program starts
constexpr int Initial_Buffer_Size = 200; // in bytes?

class Editor : public Fl_Text_Editor {
private:
    std::string m_currFile;
    Fl_Font m_font;
    bool m_saved = true;
    const Style_Table_Entry m_styletable[4] = {
	//     Font Color            | Font  | Font Size
	{ fl_rgb_color(250, 244, 219), m_font,    13    }, // 'A' - Default style
	{ fl_rgb_color(236, 132, 62),  m_font,    13    }, // 'B' - Comments
	{ fl_rgb_color(242, 164, 129), m_font,    13    }, // 'C' - Strings
	{ fl_rgb_color(108, 232, 235), m_font,    13    }  // 'D' - Keywords
    };
    Fl_Text_Buffer *m_stylebuf;
    friend void style_update(int pos, int nInserted, int nDeleted, int nRestyled,
			     const char *deletedText, void *data);
    inline void change_label(const char *label);
public:
    Editor(Fl_Text_Buffer *edit_buffer, int x, int y, int w, int h,
	   Fl_Font defaultFont, const char *filename = "test.txt");
    ~Editor();
    bool operator==(const Editor *other) const;
    int handle(int event) override;
    // File handling
    bool save();
    void save_as(const char *filename);
    void open(const char *filename);
    // Searching
    void search_forward(int startPos, const char *searchString);
};

void search_prompt(Editor *editor);
#endif
