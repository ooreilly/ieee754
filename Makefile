all: ieee754

ieee754:
	gcc ieee754.c -O3 -o $@.x

clean:
	rm *.x
