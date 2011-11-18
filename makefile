all:
	g++ -c coord.cpp
	g++ -c plot.cpp
	g++ -c plot_op.cpp
	g++ opengltest.cpp -lglut -o opengltest coord.o plot.o plot_op.o
