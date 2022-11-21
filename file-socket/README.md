# Get Files Over TCP Socket
`server.c`: Simple socket server that sends the files requested by the client which requsets the file by its name.
`client.py`: A testing client.

## Build
```
make
```
## Run
- Run the server:
```
./server.out
```

- Run the client:
```
python3 client.py [FILE_NAME]
```
## Test
`multi-client-test.sh`: A script that spin multiple instances of `client.py` to test the multithreading capabilities of `server.c` app. It also prints out the execution time.