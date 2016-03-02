cc = clang
objs = main.o dragonTamer.o utilities.o
flags = -g -L/usr/lib64 -lX11 -lGL -lGLU -lglut -lm

parser: $(objs)
	$(cc) -o parser $(objs) $(flags)

main.o : dragonTamer.h utilities.h
dragonTamer.o : dragonTamer.h utilities.h

clean:
	rm parser $(objs)
