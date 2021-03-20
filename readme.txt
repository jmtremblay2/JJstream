make

# print all ts packets from your file
./jjstream /home/jm/Videos/video.ts print_packets max_print 1000

# print all pids found and print only the unique ones
./jjstream /home/jm/Videos/video.ts print_pids | sort | uniq

# print the PAT packets
./jjstream /home/jm/Videos/video.ts print_pat_packets print_formatted_pat max_print 1000