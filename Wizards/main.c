#include "gameLogic.c"
#include "color.h"
#include "multiplayer.h"

#include <windows.h>



//Globals for Multiplayer Structure
SOCKET *s;

int isMultiplayer = 1;
int isServer = 1;
int closeSockets = 0;

//send data thread globals
char* sendMsg;
int *newMsg;

DWORD WINAPI SendDataThread(LPVOID param) {
    
    // This thread would handle sending data
    int sockIndex = *(int*)param;
    printf("SendThread %d created.\n", sockIndex);

    while(!closeSockets){
        //wait for new message to send
        while(!newMsg[sockIndex]){Sleep(100);}
        sendData(sendMsg, s[sockIndex]);

        //clear out msg data for new message
        newMsg[sockIndex] = 0;
        free(sendMsg);
    }
    deleteSocket(&s[sockIndex]);
    return 0;
}

DWORD WINAPI ReceiveDataThread(LPVOID param) {
    // This thread would handle receiving data
    int sockIndex = *(int*)param;
    printf("RecvThread %d created.\n", sockIndex);

    while(!closeSockets){
        char* msg = recvData(s[sockIndex]);
        //send to game logic to update client game
        //Server - forward info to other clients
        processMultiplayerData(msg, isServer, sockIndex);
        free(msg);
    }
    deleteSocket(&s[sockIndex]);
    return 0;
}

DWORD WINAPI DrawThread(LPVOID param) {
    // This thread would handle drawing
    printf("Draw thread is running.\n");
    return 0;
}

DWORD WINAPI GameLogicThread(LPVOID param) {
    // This thread would handle game logic
    printf("Game logic thread is running.\n");
    return 0;
}

int main() {
    HANDLE* sendThread, *receiveThread;
    HANDLE drawThread, gameLogicThread;
    char choice;
    char serverIP[16];
    int numClients;

    printf("Is this a multiplayer game? [y/n] ");
    scanf("%c", &choice);
    if(choice == 'n'){
        isMultiplayer = 0;
    }

    //framework for initial connection
    if(isMultiplayer){
        while(1){
            printf("Are you a client (c) or server (s)? ");
            scanf("%c", &choice);//hacky fix to tmp code
            scanf("%c", &choice);
            if(choice == 's' || choice == 'c'){
                break;
            }
        }
        

        if (choice == 's') {
            //get number of clients
            printf("Each number of clients: ");
            scanf("%d", &numClients);

            system("ipconfig | findstr IPv4");

            //needs variable number of connections and threads
            s = (SOCKET*)malloc(numClients * sizeof(SOCKET));
            sendThread = (HANDLE*)malloc(numClients * sizeof(HANDLE));
            receiveThread = (HANDLE*)malloc(numClients * sizeof(HANDLE));

            //connect to each client and give each a SOCKET
            for(int i = 0; i < numClients; i++){
                int clientPort = initServer(i);
                s[i] = server(clientPort);
            }
        }else if(choice == 'c') {
            isServer = 0;
            //only need 1 connection - initialize HANDLES for threads
            s = (SOCKET*)malloc(sizeof(SOCKET));
            sendThread = (HANDLE*)malloc(sizeof(HANDLE));
            receiveThread = (HANDLE*)malloc(sizeof(HANDLE));

            printf("Enter the server IP: ");
            scanf("%s", serverIP);

            int gamePort = initClient(serverIP);
            client(serverIP, gamePort);
        }else{
            printf("Invalid choice.\n");
        }
    }else{
        //OLD TESTS FOR WIZARD CREATION AND INVENTORY MANAGMENT
        /*critter* w = createWizard("Gandalf the Grey", 0);
        item i1;
        charArrayCpy(i1.name, "Minor Healing Potion", MAX_NAME_SIZE);
        i1.type = 1;
        i1.affinity = 1;
        item i2;
        charArrayCpy(i2.name, "Glock 43X MOS", MAX_NAME_SIZE);
        i2.type = 2;
        i2.affinity = 4;
        printf("Wizard: %s\tSanity: %.1lf\nTotal Aetherium: %d\nInventory Size: %d\n",w->name,w->hp.sanity, sumAether(w->wiz->Aeth), w->inventorySize);
        addItemToInventory(w, i1);
        addItemToInventory(w, i2);
        addItemToInventory(w, i1);
        addItemToInventory(w, i1);
        printf("1.%s > %d\n2.%s > %d\n3.%s > %d\n4.%s > %d\n", w->inventory[0].name, w->inventory[0].type, w->inventory[1].name, w->inventory[1].type, w->inventory[2].name, w->inventory[2].type, w->inventory[3].name, w->inventory[3].type);
        effect e;
        charArrayCpy(e.name, "Plot Armor", MAX_NAME_SIZE);
        e.type = 1;
        e.duration = -1;
        addEffect(w, e);
        printf("Status Effect:\na) %s\t Duration: %d\n", w->activeEffects[0].name, w->activeEffects[0].duration);

        map m = createMap(25); //Sweats in Spanish
        for(int i =0; i < 25; i++){
            printf("%d -> ", m.nodeList[i].type);
            for(int j =0; j < m.nodeList[i].numConnections; j++){
                printf("%d, ", m.nodeList[i].connectedNodes[j]->type);
            }
            printf("\n");
        }*/
        /* TEST FOR ADDING AND SUBTRACTING AETHER
        unsigned int test1 = createAffinities(1);
        unsigned int test2 = createAffinities(2);
        unsigned int test3 = createAffinities(3);
        unsigned int test4 = createAffinities(7);
        unsigned int test5 = 0x123456;
        printf("test 1: %d\ntest 2: %d\ntest 3: %d\ntest 4: %d\ntest 5: %d\n", sumAether(test1), sumAether(test2), sumAether(test3), sumAether(test4), sumAether(test5));
        unsigned int test6 = addAether(3, test5);
        unsigned int test7 = addAether(1, test1);
        unsigned int test8 = addAether(4, test2);
        printf("test 6: %d\ntest 7: %d\ntest 8: %d\n", sumAether(test6), sumAether(test7), sumAether(test8));
        unsigned int test9 = subAether(2, test8);
        unsigned int test10 = subAether(1, test1);
        unsigned int test11 = subAether(4, test6);
        printf("test 9: %d\ntest 10: %d\ntest 11: %d\n", sumAether(test9), sumAether(test10), sumAether(test11));
        */
    }

    system("pause");

    //Create Threads
    if(isMultiplayer && !isServer){
        //clients onlt need 1 of each thread
        int client = 0;
        sendThread[0] = CreateThread(NULL, 0, SendDataThread, &client, 0, NULL);
        receiveThread[0] = CreateThread(NULL, 0, ReceiveDataThread, &client, 0, NULL);
    }else if(isMultiplayer && isServer){
        //create threads for each client and send each thread a number to demark the index of its SOCKET to use
        for(int i = 0; i < numClients; i++){
            int* client = (int*)malloc(sizeof(int));
            *client = i;
            sendThread[i] = CreateThread(NULL, 0, SendDataThread, client, 0, NULL);
            receiveThread[i] = CreateThread(NULL, 0, ReceiveDataThread, client, 0, NULL);
        }
    }

    drawThread = CreateThread(NULL, 0, DrawThread, NULL, 0, NULL);
    gameLogicThread = CreateThread(NULL, 0, GameLogicThread, NULL, 0, NULL);

    //error checking
    if (sendThread[0] == NULL || receiveThread[0] == NULL || drawThread == NULL || gameLogicThread == NULL) {
        printf("Failed to create one or more threads.\n");
        return 1;
    }

    system("pause");

    //Wait for all threads to close
    if(isMultiplayer && !isServer){
        WaitForSingleObject(sendThread[0], INFINITE);
        WaitForSingleObject(receiveThread[0], INFINITE);
    }else if(isMultiplayer && isServer){
        for(int i = 0; i < numClients; i++){
            WaitForSingleObject(sendThread[i], INFINITE);
            WaitForSingleObject(receiveThread[i], INFINITE);
        }
    }
    WaitForSingleObject(drawThread, INFINITE);
    WaitForSingleObject(gameLogicThread, INFINITE);


    //Cleanup
    if(isMultiplayer && !isServer){
        CloseHandle(sendThread[0]);
        CloseHandle(receiveThread[0]);
    }else if(isServer){
        for(int i = 0; i < numClients; i++){
            CloseHandle(sendThread[i]);
            CloseHandle(receiveThread[i]);
        }
    }
    CloseHandle(drawThread);
    CloseHandle(gameLogicThread);


    free(s);
    free(sendThread);
    free(receiveThread);
    return 0;
}
