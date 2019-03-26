VPATH = include:src

OBJECTS = main.o web.o http.o websocket.o handler.o
G++ = g++ -lpthread -g -o $@ -c $<

mntsys: $(OBJECTS)
	g++ -lssl -lcrypto -lpthread -o mntsys $(OBJECTS)

main.o: main.cc web.h handler.h
	$(G++)

web.o: web.cc web.h endpoint.h config.h utility.h connection.h http.h websocket.h
	$(G++)

http.o: http.cc http.h connection.h endpoint.h handler.h
	$(G++)

websocket.o: websocket.cc websocket.h crypto.h connection.h message.h endpoint.h
	$(G++)

handler.o: handler.cc handler.h utility.h
	$(G++)

clean:
	rm *.o mntsys
