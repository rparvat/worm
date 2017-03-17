CC=g++
LIBS = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CFLAGS=$(LIBS) -std=c++11 -stdlib=libc++ -Wno-c++11-extensions
DEPS = CImg.h dijkstra.h
OBJ = dijkstra.o main.o

worm: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ ;rm *o

$(OBJ) : %.o:  %.cpp $(DEPS)
	 $(CC) $(CFLAGS) -c -o $@ $<

all:
	make worm

clean:
	rm *o worm
