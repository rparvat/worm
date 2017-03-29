CC=g++
CFLAGS= -std=c++11
LDFLAGS= -L/home/armafire/tools/cilkplus-install/lib64 -lcilkrts -lpthread -lpng -lX11
INCLUDES= -fcilkplus
DEPS = CImg.h graph.h dijkstra_thread.h dijkstra.h
OBJ = graph.o dijkstra.o main.o

worm: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^ ;rm *o

$(OBJ) : %.o:  %.cpp $(DEPS)
	 $(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $< -c -o $@

all:
	make worm

clean:
	rm *o worm
