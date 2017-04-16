CC=g++
CFLAGS= -std=c++11 -O3 -ffast-math#-g
LDFLAGS= -L/home/armafire/tools/cilkplus-install/lib64 -lcilkrts -lpthread -lpng -lX11 -I/home/armafire/tools/opencv-3-install-test/include -L/home/armafire/tools/opencv-3-install-test/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lboost_program_options
INCLUDES= -fcilkplus
DEPS = CImg.h graph.h dijkstra_thread.h dijkstra.h
OBJ = graph.o dijkstra.o main.o

worm: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -o $@ $^ && rm *o

$(OBJ) : %.o:  %.cpp $(DEPS)
	 $(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $< -c -o $@

all:
	make worm

clean:
	rm worm
