# Concurrent-File-Transfer
An application that enables a client to download “chunks” of a file from multiple servers distributed over the Internet, and assemble the chunks to form the complete file.

# README
Name: Shuli He
Email: she77@ucsc.edu
Using C++
# Files
bin - compiled program; dest/ destination fold; files/ server files fold
doc - report: design details
src - source file

#Usage
Use command 'make' (Makefile) to compile the source code.(Using g++)

Client:
./myclient <server-info.txt> <num-connections> <filename>
example: ./myclient serverinfo.txt 3 set

Server:
./myserver <port>
example: ./myserver 12345
