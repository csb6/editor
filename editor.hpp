#ifndef EDITOR_APP_H
#define EDITOR_APP_H
#include <FL/Fl_Text_Editor.H>
#include <string>

constexpr int Width = 500;
#ifdef __APPLE__
    // macOS menu is in a taskbar, so don't need to make room for it in the
    // main window above the editor widget
    constexpr int EditorY = 0;
#else
    constexpr int EditorY = 20;
#endif
// Name of editor font
constexpr char Font[] = "Monaco";
// Width of column on side of screen that contains line numbers
constexpr int Line_Number_Width = 30; // in px
// Size of editor buffer when program starts
constexpr int Initial_Buffer_Size = 200; // in bytes?

class Editor : public Fl_Text_Editor {
private:
    std::string m_currFile;
    bool m_saved = true;
public:
    Editor(Fl_Text_Buffer *edit_buffer, int x, int y, int w, int h);
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
