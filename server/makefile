VPATH = include:src

OBJECTS = main.o web.o http.o handler.o
G++ = g++ -lpthread -g -o $@ -c $<

mntsys: $(OBJECTS)
	g++ -lpthread -o mntsys $(OBJECTS)

main.o: main.cc web.h handler.h
	$(G++)

web.o: web.cc web.h endpoint.h config.h utility.h
	$(G++)

http.o: http.cc http.h connection.h endpoint.h handler.h
	$(G++)

handler.o: handler.cc handler.h utility.h
	$(G++)
	
clean:
	rm *.o mntsys
