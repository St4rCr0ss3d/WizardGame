#include "critter.h"

typedef struct nodeStruct
{
    //node data
    int numConnections;
    struct nodeStruct** connectedNodes;

    //map data
    int type;
    char name[25];
}node;

#define MAX_CONNECTIONS 4

typedef struct mapStruct
{
    node* currentNode; //pointer to current node
    node* nodeList; //list for data management purposes
    int numNodes;
}map;

node createNode(int type, char* name){
    node n;
    n.numConnections = 0;
    n.type = type;
    charArrayCpy(n.name, name, 25);
    n.connectedNodes = (node**)malloc(MAX_CONNECTIONS * sizeof(node*));
    for(int i = 0; i < MAX_CONNECTIONS; i++){
        n.connectedNodes[i] = NULL;
    }
    return n;
}

//create connections from every node to another - starts with an end-to-end line and adds a few additional edges
void connectNodes(node* n, int numNodes){
    //create an unvisited lists
    int numUnvisited = numNodes;
    int* unvisited = (int*)malloc(numNodes * sizeof(int));
    for (int i = 0; i < numNodes; i++){
        unvisited[i] = 1; //everyone has not been visited
    }
    int curNode = rand()%numUnvisited;
    unvisited[curNode] = 0;
    numUnvisited--;
    //printf("1 ");
    while(numUnvisited){
        //printf(". ");
        int nextNode = rand()%numUnvisited;
        for (int i = 0; i <= nextNode; i++){
            if(!unvisited[i]){
                nextNode++;
            }
        }
        //printf("!!%d %d",curNode, nextNode);
        n[curNode].connectedNodes[n[curNode].numConnections] = &n[nextNode];
        n[curNode].numConnections++;
        n[nextNode].connectedNodes[n[nextNode].numConnections] = &n[curNode]; //garuntees almost all are going to be have 2 connections
        n[nextNode].numConnections++;
        curNode = nextNode;
        unvisited[curNode] = 0;
        numUnvisited--;
    }
    free(unvisited);

    //add additional edges
    //printf("2 ");
    for(int i = 0; i < numNodes; i++){
        //printf(". ");
        int additionalConnections = rand()%2 + n[i].numConnections;
        if(additionalConnections > MAX_CONNECTIONS){
            additionalConnections = MAX_CONNECTIONS;
        }
        while(n[i].numConnections < additionalConnections){
            int j = rand()%numNodes;
            while(j != i && n[j].numConnections >= MAX_CONNECTIONS){
                j = rand()%numNodes;
            }
            n[i].connectedNodes[n[i].numConnections] = &n[j];
            n[i].numConnections++;
            n[j].connectedNodes[n[j].numConnections] = &n[i];
            n[j].numConnections++;
        }
    }
    //printf(" F");
}

map createMap(int numNodes){
    map m;
    m.numNodes = numNodes;
    m.nodeList = (node*)malloc(m.numNodes * sizeof(node));
    for(int i = 0; i < m.numNodes; i++){
        //printf("Creating Node %d\n", i);
        m.nodeList[i] = createNode(i, "test node"); //NAMING AND TYPING WIP
    }
    connectNodes(m.nodeList, m.numNodes);
    m.currentNode = &m.nodeList[0];
    //printf("Returning\n");
    return m;
}