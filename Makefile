mpegts_header.o: mpegts_header.h mpegts_header.c
	gcc -c mpegts_header.c -o mpegts_header.o

mpegts_packet.o: mpegts_packet.h mpegts_packet.c
	gcc -c mpegts_packet.c -o mpegts_packet.o

mpegts_pat.o: mpegts_pat.h mpegts_pat.c
	gcc -c mpegts_pat.c -o mpegts_pat.o

utils.o: utils.h utils.c
	gcc -c utils.c -o utils.o

main.o: main.c
	gcc -c main.c -o main.o

jjstream: main.o mpegts_header.o utils.o mpegts_packet.o mpegts_pat.o
	gcc main.o mpegts_header.o utils.o mpegts_packet.o mpegts_pat.o -o jjstream

clean:
	rm *.o