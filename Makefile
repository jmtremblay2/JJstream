mpegts_header.o: mpegts_header.h mpegts_header.c
	gcc -g -c mpegts_header.c -o mpegts_header.o

mpegts_packet.o: mpegts_packet.h mpegts_packet.c
	gcc -g -c mpegts_packet.c -o mpegts_packet.o

mpegts_pat.o: mpegts_pat.h mpegts_pat.c
	gcc -g -c mpegts_pat.c -o mpegts_pat.o

args.o: args.h args.c
	gcc -g -c args.c -o args.o

utils.o: utils.h utils.c
	gcc -g -c utils.c -o utils.o

features.o: features.h features.c
	gcc -g -c features.c -o features.o

main.o: main.c
	gcc -g -c main.c -o main.o

jjstream: main.o mpegts_header.o utils.o mpegts_packet.o args.o mpegts_pat.o features.o
	gcc -g main.o mpegts_header.o utils.o mpegts_packet.o args.o mpegts_pat.o features.o -o jjstream

clean:
	rm *.o