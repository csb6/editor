compiler := clang++
flags := -std=c++17 -Wall -Wextra -pedantic-errors -Ifltk

object_files := editor.o main.o
fltk_flags := `fltk/fltk-config --cxxflags`
link_flags := `fltk/fltk-config --ldflags`

editor: $(object_files) fltk/lib/libfltk.a
	$(compiler) -o editor $(link_flags) $(flags) $(object_files) fltk/lib/libfltk.a

$(object_files): %.o: %.cpp
	$(compiler) -c --write-dependencies -MF$*.dep $(fltk_flags) $(flags) $*.cpp -o $*.o

-include $(object_files:.o=.dep)

fltk/lib/libfltk.a:
	cd fltk/src; make -j

clean:
	-rm *.o *.dep
	-rm *~
	-rm editor
