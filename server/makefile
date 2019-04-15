VPATH = include:src

OBJECTS = main.o web.o websocketSSL.o handler.o get_info.o to_json.o authentication.o logger.o
G++ = g++ -std=c++11 -lpthread -g -o $@ -c $<

mntsys: $(OBJECTS)
	g++ -o mntsys $(OBJECTS) -std=c++11 -lssl -lcrypto -lpthread

main.o: main.cc web.h handler.h get_info.h to_json.h
	$(G++)

web.o: web.cc web.h config.h utility.h connection.h websocketSSL.h crypto.h handler.h authentication.h total_format.h usage_format.h mem_format.h
	$(G++)

websocketSSL.o: websocketSSL.cc websocketSSL.h connection.h message.h endpoint.h
	$(G++)

handler.o: handler.cc handler.h utility.h to_json.h logger.h display_format.h mem_format.h total_format.h usage_format.h
	$(G++)

get_info.o: get_info.cc get_info.h to_json.h resource_info.h logger.h
	$(G++)

to_json.o: to_json.cc to_json.h resource_info.h endpoint.h websocketSSL.h display_format.h
	$(G++)

authentication.o: authentication.cc authentication.h crypto.h
	$(G++)

logger.o: logger.cc logger.h
	$(G++)

clean:
	rm *.o mntsys
