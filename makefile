all:
	g++ -Wall -Wextra -O3 -c tasksys.cpp
	gcc -Wall -Wextra -O3 -mavx2 -march=native -c mandelbrot_serial.c
	gcc -Wall -Wextra -O3 -c main.c
	ispc -O3 --target=avx2-i32x16 mandelbrot_ispc.ispc -o mandelbrot_ispc.o
	g++ -Wall -Wextra -O3 -o mandel main.o mandelbrot_serial.o mandelbrot_ispc.o tasksys.o -lpthread

clean:
	-rm mandel
	-rm *.o
	-rm *.ppm
