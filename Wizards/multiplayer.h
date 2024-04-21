#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define INITIAL_PORT 4333
#define BUFFER_SIZE 1024

//initial handshake -- sends port number
int initServer(int playerNum) {
    printf("(%d) Initial Connection: ", playerNum);
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET, ClientSocket = INVALID_SOCKET;
    struct sockaddr_in server, client;
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    int clientSize = sizeof(client);

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(INITIAL_PORT);
   
    bind(ListenSocket, (struct sockaddr*)&server, sizeof(server));
    listen(ListenSocket, SOMAXCONN);

    printf("Server is listening on port %d...", INITIAL_PORT);

    ClientSocket = accept(ListenSocket, (struct sockaddr*)&client, &clientSize);

    printf(" Server accepted client!\n");

    //player 0 client: 4334
    //player 1 client: 4335
    //player 2 client: 4336
    sprintf(buffer, "%d", (INITIAL_PORT + 1 + playerNum));
    printf("Sending %s.\n", buffer);
    send(ClientSocket, buffer, BUFFER_SIZE, 0);

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return (INITIAL_PORT + 1 + playerNum);
}

//initial handshake -- returns port number
int initClient(char* serverIP) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(serverIP);
    server.sin_port = htons(INITIAL_PORT);

    connect(ConnectSocket, (struct sockaddr*)&server, sizeof(server));
    recv(ConnectSocket, buffer, BUFFER_SIZE, 0);
    printf("Received new port from server: %s (", buffer);
    closesocket(ConnectSocket);
    WSACleanup();

    int newPort = atoi(buffer);

    printf("%d)\n", newPort);
    return newPort;
}

//creates a socket for the server to connect to a client
SOCKET server(int port){
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET, ClientSocket = INVALID_SOCKET;
    struct sockaddr_in server, client;
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    int clientSize = sizeof(client);

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(INITIAL_PORT);

    bind(ListenSocket, (struct sockaddr*)&server, sizeof(server));
    listen(ListenSocket, SOMAXCONN);

    printf("Server is listening on port %d...", port);

    ClientSocket = accept(ListenSocket, (struct sockaddr*)&client, &clientSize);
    closesocket(ListenSocket);
    printf("Success!\n");

    return ClientSocket;
}

//creates a socket for the client to connect to the server
SOCKET client(char* IP, int port){
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(INITIAL_PORT);

    printf("Connecting to %s on port %d... ", IP, port);
    connect(ConnectSocket, (struct sockaddr*)&server, sizeof(server));
    printf("Success!\n");

    return ConnectSocket;
}

void sendData(char* message, SOCKET s){
    send(s, message, BUFFER_SIZE, 0);
}

char* recvData(SOCKET s){
    char *buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
    memset(buffer, 0, BUFFER_SIZE);

    recv(s, buffer, BUFFER_SIZE, 0);

    return buffer;
}

void deleteSocket(SOCKET* s){
    closesocket(*s);
    WSACleanup();
}