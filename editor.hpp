#ifndef EDITOR_APP_H
#define EDITOR_APP_H
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

constexpr int Width = 500;
// Name of editor font
constexpr char Font[] = "Monaco";
// Width of column on side of screen that contains line numbers
constexpr int Line_Number_Width = 30; // in px
// Size of editor buffer when program starts
constexpr int Initial_Buffer_Size = 200; // in bytes?

static void save(Fl_Text_Editor *editor, const char *filename);
static void save_as(Fl_Text_Editor *editor);


constexpr Fl_Text_Editor::Key_Binding Editor_Bindings[] = {
    // Move cursor back One Word
    {'b', FL_COMMAND, [](int, auto *editor) {
			    editor->previous_word();
			    return 1;
			}},
    // Move cursor forward One Word
    {'f', FL_COMMAND, [](int, auto *editor) {
			   editor->next_word();
			   return 1;
		       }},
    // Search function
    {'s', FL_CONTROL, [](int, auto *editor) {
			  return 1;
		       }},
    // Save current buffer
    {'s', FL_COMMAND, [](int, auto *editor) {
			  save(editor, "test.txt");
			  return 1;
		       }},
    // Save As current buffer
    {'s', FL_COMMAND + FL_SHIFT, [](int, auto *editor) {
				      save_as(editor);
				      return 1;
				   }}
};
#endif
