CC=g++
# LIBS = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CFLAGS= -std=c++11 -Wno-c++11-extensions #$(LIBS)
DEPS = graph.h dijkstra.h
OBJ = graph.o dijkstra.o main.o

worm: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ ;rm *o

$(OBJ) : %.o:  %.cpp $(DEPS)
	 $(CC) $(CFLAGS) -c -o $@ $<

all:
	make worm

clean:
	rm *o worm
