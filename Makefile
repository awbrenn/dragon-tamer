cc = clang
objs = main.o dragonTamer.o utilities.o viewPortNavigation.o
flags = -g -L/usr/lib64 -lX11 -lGL -lGLU -lglut -lm

parser: $(objs)
	$(cc) -o dragon_tamer $(objs) $(flags)

main.o : dragonTamer.h utilities.h viewPortNavigation.h
dragonTamer.o : dragonTamer.h utilities.h
viewPortNavigation.o: viewPortNavigation.h

clean:
	rm dragon_tamer $(objs)
