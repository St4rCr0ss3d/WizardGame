#include "map.h"

typedef struct gameLogicStruct
{
    int day;
    critter* player;
    /* data */
}gameData;


void initializeGame(int mapSize, int numPlayers){
    //Create wizards

    //Generate map

}

//gets the data sent from a client or the server to update the game data
int processMultiplayerData(char* msg, int isServer, int playerNum){

}