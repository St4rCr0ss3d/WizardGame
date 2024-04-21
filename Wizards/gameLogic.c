#include "wizard.h"

typedef struct gameLogicStruct
{
    int day;
    wizard *player;
    /* data */
}gameData;


void initializeGame(){

}

//gets the data sent from a client or the server to update the game data
int processMultiplayerData(char* msg, int isServer, int playerNum){

}