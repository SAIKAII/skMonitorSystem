VPATH = include:src

OBJECTS = main.o web.o http.o
G++ = g++ -g -o $@ -c $<

mntsys: $(OBJECTS)
	g++ -o mntsys $(OBJECTS)

main.o: main.cc web.h handler.h
	$(G++)

web.o: web.cc web.h endpoint.h config.h utility.h
	$(G++)

http.o: http.cc http.h connection.h endpoint.h
	$(G++)

clean:
	rm *.o mntsys
