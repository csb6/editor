#ifndef EDITOR_APP_H
#define EDITOR_APP_H
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

constexpr int Width = 500;
// Name of editor font
constexpr char Font[] = "Monaco";
// Width of column on side of screen that contains line numbers
constexpr int Line_Number_Width = 30; // in px
// Size of editor buffer when program starts
constexpr int Initial_Buffer_Size = 200; // in bytes?

class Editor : public Fl_Text_Editor {
public:
    typedef void (*Key_Func)(Editor *editor);
    struct Key_Binding {
	int key;
	int state;
	Key_Func function;
    };
    static Editor::Key_Binding Key_Bindings[6];
    Editor(Fl_Text_Buffer *edit_buffer, int x, int y, int w, int h);
    int handle(int event) override;
    ~Editor();
};

void save(Editor *editor, const char *filename);
void save_as(Editor *editor);
void open(Editor *editor);
#endif
