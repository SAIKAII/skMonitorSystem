VPATH = include:src

OBJECTS = main.o web.o websocketSSL.o handler.o get_info.o to_json.o
G++ = g++ -lpthread -g -o $@ -c $<

mntsys: $(OBJECTS)
	g++ -o mntsys $(OBJECTS) -lssl -lcrypto -lpthread

main.o: main.cc web.h handler.h get_info.h to_json.h
	$(G++)

web.o: web.cc web.h config.h utility.h connection.h websocketSSL.h crypto.h handler.h
	$(G++)

websocketSSL.o: websocketSSL.cc websocketSSL.h connection.h message.h endpoint.h
	$(G++)

handler.o: handler.cc handler.h utility.h
	$(G++)

get_info.o: get_info.cc get_info.h to_json.h resource_info.h
	$(G++)

to_json.o: to_json.cc to_json.h resource_info.h endpoint.h websocketSSL.h
	$(G++)

clean:
	rm *.o mntsys
