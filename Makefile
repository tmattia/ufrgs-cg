all:
	g++ -o main.out main.cpp -framework Cocoa -framework OpenGL -framework GLUT -framework GLUI -I.
	./main.out
	rm main.out
