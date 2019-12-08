#!/usr/bin/env sh
clang++ -std=c++17 -Wall -Wfloat-conversion -Wconversion -Wno-sign-conversion -Wfor-loop-analysis -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT /usr/local/lib/libfltk.a -lpthread -framework Cocoa -I/usr/local/include -o editor main.cpp editor.cpp
