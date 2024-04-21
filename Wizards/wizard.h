#include <stdio.h>
#include <stdlib.h>


#define MAX_EFFECTS 20


typedef struct healthStruct
{
    int maxHealth; //max health for each body part
    int rArm, lArm, rLeg, lLeg, body, head;
    double sanity;
    int poise;
}health;

typedef struct effectStruct
{
    char name[25];
    int type; //-1 - nothing
    int duration; // -1 - n/a
}effect;

//magic junk
typedef struct AetheriumStruct
{
    int ltAeth, dkAeth, fiAeth, wtAeth, erAeth, arAeth;
    int enigma;
}Aetherium;

//nonmagic things
typedef struct itemStruct{
    char name[25];
    int type;     // -1 - nothing
    int affinity; // -1 - no affinity
}item;

typedef struct wizardStruct
{
    char name[25];
    int inventorySize;
    health hp;
    effect *activeEffects;
    Aetherium Aeth;
    item *inventory;
}wizard;

typedef struct critterStruct
{
    char name[25];
    
}critter;

//HELPER FUNCTIONS

int sumAether(Aetherium a){
    int total = a.arAeth + a.dkAeth + a.erAeth + a.fiAeth + a.ltAeth + a.wtAeth;
    return total;
}

Aetherium createAetherium(int startingType){
    Aetherium a;
    a.enigma = 0;
    switch (startingType)
    {
    case 1:
        a.arAeth = 1;
        a.dkAeth = 0;
        a.erAeth = 0;
        a.fiAeth = 0;
        a.ltAeth = 0;
        a.wtAeth = 0;
        break;
    case 2:
        a.arAeth = 0;
        a.dkAeth = 1;
        a.erAeth = 0;
        a.fiAeth = 0;
        a.ltAeth = 0;
        a.wtAeth = 0;
        break;
    case 3:
        a.arAeth = 0;
        a.dkAeth = 0;
        a.erAeth = 1;
        a.fiAeth = 0;
        a.ltAeth = 0;
        a.wtAeth = 0;
        break;
    case 4:
        a.arAeth = 0;
        a.dkAeth = 0;
        a.erAeth = 0;
        a.fiAeth = 1;
        a.ltAeth = 0;
        a.wtAeth = 0;
        break;
    case 5:
        a.arAeth = 0;
        a.dkAeth = 0;
        a.erAeth = 0;
        a.fiAeth = 0;
        a.ltAeth = 1;
        a.wtAeth = 0;
        break;
    case 6:
        a.arAeth = 0;
        a.dkAeth = 0;
        a.erAeth = 0;
        a.fiAeth = 0;
        a.ltAeth = 0;
        a.wtAeth = 1;
        break;
    default:
        a.arAeth = 1;
        a.dkAeth = 1;
        a.erAeth = 1;
        a.fiAeth = 1;
        a.ltAeth = 1;
        a.wtAeth = 1;
        a.enigma = 1;
        break;
    }
    return a;
}

health createHealth(int maxHealth, double sanity, int poise){
    health hp;
    hp.maxHealth = maxHealth;
    hp.sanity = sanity;
    hp.poise = poise;
    hp.rArm = maxHealth;
    hp.lArm = maxHealth;
    hp.rLeg = maxHealth;
    hp.lLeg = maxHealth;
    hp.body = maxHealth;
    hp.head = maxHealth;

    return hp;
}

void clearEffectSlot(effect* e, int index){
    e[index].duration = -1;
    e[index].type = -1;
    e[index].name[0] = '\0';
}

void clearInventorySlot(item* inv, int index){
    inv[index].name[0] = '\0';
    inv[index].type = -1;
    inv[index].affinity = -1;
}

/**
 * @brief Creates the basic data structures for a wizard struct
 * @var startingAeth - 1 - Air, 2 - Dark, 3 - Earth, 4 - fire, 5 - Light, 6 - water, other - enigma
*/
wizard* createWizard(char* name, int startingAeth){
    wizard* w = (wizard*)malloc(sizeof(wizard));
    
    //name
    int l = 0;
        //count name length
    while(name[l] != '\0' || name[l] != 0){
        l++;
    }
    if(!l){
        printf("Error: no name given.\n");
        return NULL;
    }

    for(int i = 0; i < 25; i++){
        if(i < l){
            w->name[i] = name[i];
        }else{
            w->name[i] = '\0';
        }
    }

    //hp
    w->hp = createHealth(100, 100.0, 50);
    //Aeth
    w->Aeth = createAetherium(startingAeth);
    //effects
    w->activeEffects = (effect*)malloc(MAX_EFFECTS*sizeof(effect));
    for (int i = 0; i < MAX_EFFECTS; i++){
        clearEffectSlot(w->activeEffects, i);
    }
    //inventory
    w->inventorySize = 4;
    w->inventory = (item*)malloc(MAX_EFFECTS*sizeof(item));
    for (int i = 0; i < MAX_EFFECTS; i++){
        clearInventorySlot(w->inventory, i);
    }

    return w;
}

