Server info:
To start server: python chatclient.py

Client info:
To compile client: gcc chatserve.c -o chatserve
To start client: ./chatserve 127.0.0.1

Running program:
1. Start server.
2. Start client.
3. On client terminal, provide a username. Then provide the first message to be sent to server.
4. Respond on server terminal.
5. If either server or client submits "\quit" as a message, information will be provided as to who closed the connection, then the connection will close.
6. Server will continue waiting for connections until SIGINT.