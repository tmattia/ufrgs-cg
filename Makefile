all:
	g++ -o main.out main.cpp -framework Cocoa -framework OpenGL -framework GLUT -framework GLUI -I. -L/usr/local/lib -lFreeImage
	./main.out
	rm main.out
