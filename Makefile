.POSIX:
compiler := clang++
flags := -std=c++17 -Wall -Wextra -pedantic-errors -Ifltk

object_files := editor.o main.o
fltk_lib := fltk/lib/libfltk.a
fltk_flags := `fltk/fltk-config --cxxflags`
link_flags := `fltk/fltk-config --ldflags`

editor: $(object_files) $(fltk_lib)
	$(compiler) -o editor $(link_flags) $(flags) $(object_files) $(fltk_lib)

$(object_files): %.o: %.cpp
	$(compiler) -c -MD -MF$*.dep $(fltk_flags) $(flags) $*.cpp -o $*.o

-include $(object_files:.o=.dep)

$(fltk_lib):
	cd fltk/src; make -j

.PHONY: clean
clean:
	-rm *.o *.dep
	-rm *~
	-rm editor
