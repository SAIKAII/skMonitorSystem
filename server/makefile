VPATH = include:src

OBJECTS = main.o web.o http.o websocket.o handler.o get_info.o to_json.o
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

get_info.o: get_info.cc get_info.h to_json.h resource_info.h
	$(G++)

to_json.o: to_json.cc to_json.h resource_info.h endpoint.h websocket.h
	$(G++)

clean:
	rm *.o mntsys
