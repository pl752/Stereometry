CC=/mingw32/bin/clang++.exe
LD=/mingw32/bin/ld.exe
LTO=/mingw32/bin/llvm-lto.exe
CRTLIBPATH=target-i686
CFLAGS=-c -target i386-pc-windows-gnu -march=i686 -mtune=pentium2 -DDLL -I 3rd_party -I . -I /mingw32/include/w32api/ -I /mingw32/include/c++/13.2.0/ -I /mingw32/include/ -I /mingw32/include/c++/13.2.0/i686-w64-mingw32/ -fvisibility=hidden -O2 -flto -ffast-math
LDFLAGS0=-m i386pe --dll -Bdynamic -e _DllMainCRTStartup@12 --enable-auto-image-base $(CRTLIBPATH)/dllcrt2.o $(CRTLIBPATH)/crtbegin.o -LC:$(CRTLIBPATH)/
LDFLAGS1=-lopengl32 -lglu32 -lgdi32 -Bstatic -lstdc++ -Bdynamic -lmingw32 -lgcc -lgcc_eh -lmoldname -lmingwex -lmsvcrt -lpthread -ladvapi32 -lshell32 -luser32 -lkernel32 -lmingw32 -lgcc -lgcc_eh -lmoldname -lmingwex -lmsvcrt -lkernel32 $(CRTLIBPATH)/crtend.o
SOURCES=$(wildcard *.cpp Figures/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Stereometry.dll

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): all.obj
	$(LD) -o $@ $(LDFLAGS0) $< $(LDFLAGS1)

all.obj: $(OBJECTS) 
	$(LTO) -O2 -o $@ $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm $(OBJECTS) $(EXECUTABLE) all.obj