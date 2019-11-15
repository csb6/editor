#ifndef EDITOR_APP_H
#define EDITOR_APP_H
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <string>
#include <utility>
#include <map>

class Editor;
typedef void (*Editor_Callback)(Editor*);
struct Key_Bind {
    std::pair<int,int> key; //{event_key, event_state}
    Editor_Callback callback;
};

class Editor {
private:
    std::string m_filename;
    Fl_Font m_font = 0;
    void initFonts();
    Fl_Text_Editor* m_editor;
public:
    static constexpr int Width = 500;
    // Name of editor font
    static constexpr char Font[] = "Monaco";
    // Width of column on side of screen that contains line numbers
    static constexpr int Line_Number_Width = 30; // in px
    // Size of editor buffer when program starts
    static constexpr int Initial_Buffer_Size = 200; // in bytes?
    explicit Editor(int x, int y, const std::string filename);
    void save(const char *filename);
    void save() { save(m_filename.c_str()); };
    void save_as();
};

constexpr Key_Bind Editor_Bindings[] = {
    // Move cursor back One Word
    {{'b', FL_COMMAND}, [](auto *editor) {
			    editor->previous_word();
			}},
    // Move cursor forward One Word
    {{'f', FL_COMMAND}, [](auto *editor) {
			   editor->next_word();
		       }},
    // Search function
    {{'s', FL_CONTROL}, [](auto *editor) {
			  return;
		       }},
    // Save current buffer
    {{'s', FL_COMMAND}, [](auto *editor) {
			  editor->save();
		       }},
    // Save As current buffer
    {{'s', FL_COMMAND + FL_SHIFT}, [](auto *editor) {
				      editor->save_as();
				   }}
};
#endif
