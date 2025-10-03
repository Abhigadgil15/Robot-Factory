#For connecting to server
ssh [YOUR KHOURY ID]@linux-075.khoury.northeastern.edu

#For starting the server
./server 11111

#For starting the client
./client 127.0.0.1 11111

#For starting client with 1000 customers
./client 127.0.0.1 11111 16 1000 0
./client 129.10.8.178 11111 16 1000 0


./client 127.0.0.1 11111 32 10000 0

#TODO 
Fix Threads logic on server side 
Fix threads logic on client side if its going wrong
