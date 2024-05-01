#include <stdio.h>
#include <stdlib.h>

#define MAX_EFFECTS 20
#define MAX_NAME_SIZE 25

typedef struct healthStruct
{
    int maxHealth; //max health for each body part
    int rArm, lArm, rLeg, lLeg, body, head;
    double sanity;
    int poise;
}health;

typedef struct effectStruct
{
    char name[MAX_NAME_SIZE];
    int type; //-1 - nothing
    int duration; // -1 - n/a
}effect;

//nonmagic things
typedef struct itemStruct{
    char name[MAX_NAME_SIZE];
    int type;     // -1 - nothing
    int affinity; // -1 - no affinity
}item;

//very magic things
typedef struct spellStruct
{
    //what is it
    char name[MAX_NAME_SIZE];
    unsigned char type; // 0 - effect, 1 - damage, 2 - damage+effect, 3 - summon
    unsigned int affinities; //bit 0-3 - Air, 4-7 - Dark, 8-11 - Earth, 12-15 - fire, 16-19 - Light, 20-23 - water, 24-27 - enigma, 28-31 - chaos/neutral
    int range; //0 - self, 1+ num tiles
    void* effectOrSummon; //what are you creating or adding to that location
    int mystDrain;
    double castChance;
}spell;

//contains all stats pertaining solely to wizards
typedef struct wizardStruct
{
    int maxMyst;
    unsigned int affinities; //bit 0-3 - Air, 4-7 - Dark, 8-11 - Earth, 12-15 - fire, 16-19 - Light, 20-23 - water, 24-27 - enigma, 28-31 - chaos/neutral
}wizardStats;

//all data needed for any living thing
typedef struct critterStruct
{
    char name[MAX_NAME_SIZE];
    health hp;
    effect *activeEffects;
    item *inventory;
    int isWizard, inventorySize, armorValue;
    wizardStats* wiz; //data for wizards
}critter;

//HELPER FUNCTIONS

int charArrayCpy(char* dest, char* source, int destLen){
    int l1 = 0;
    //count array length
    while(source[l1] != '\0' || source[l1] != 0){
        l1++;
    }
    if(!l1){
        printf("Error: source char array is of length 0\n");
        return 1;
    }
    for (int i = 0; i < destLen; i++)
    {
        if(i <= l1){
            dest[i] = source[i];
        }else{
            dest[i] = '\0';
        }
    }
    return 0;
}

int sumAether(unsigned int AetheriumCount){
    int total = 0;
    for(int i = 0; i < 6; i++){
        unsigned int tmp = AetheriumCount;
        tmp = (tmp >> (i*4)) & 0xF;
        total+= tmp;
    }
    return total;
}

//Aeth - 1 - Air, 2 - Dark, 3 - Earth, 4 - fire, 5 - Light, 6 - water, 7 - enigma
unsigned int addAether(int AethType, unsigned int affinities){
    AethType--;//switch for bitwise operation
    affinities = affinities + (1 << (AethType*4));
    return affinities;
}

//Aeth - 1 - Air, 2 - Dark, 3 - Earth, 4 - fire, 5 - Light, 6 - water, 7 - enigma 
unsigned int subAether(int AethType, unsigned int affinities){
    AethType--;//switch for bitwise operation
    //check if zero aether of type
    if((affinities >> (AethType*4)) & 0xF){
        affinities = affinities - (1 << (AethType*4));
    }else{
        printf("Error: no Aether of this type\n");
    }
    return affinities;
}
//bit 0-3 - Air, 4-7 - Dark, 8-11 - Earth, 12-15 - fire, 16-19 - Light, 20-23 - water, 24-27 - enigma, 28-31 - SPELLS
unsigned int createAffinities(int startingType){
    unsigned int a;
    switch (startingType)
    {
    case 1:
        a = 0x1;
        break;
    case 2:
        a = 0x10;
        break;
    case 3:
        a = 0x100;
        break;
    case 4:
        a = 0x1000;
        break;
    case 5:
        a = 0x10000;
        break;
    case 6:
        a = 0x100000;
        break;
    default:
        a = 0x1111111;
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
 * @brief Adds item to critter inventory
 * @return 1 if inventory is full, 0 otherwise
*/
int addItemToInventory(critter* w, item q){
    //search for open spot
    for(int i = 0; i < w->inventorySize; i++){
        if(w->inventory[i].type == -1){
            w->inventory[i].type = q.type;
            w->inventory[i].affinity = q.affinity;
            charArrayCpy(w->inventory[i].name, q.name, MAX_NAME_SIZE);
            return 0;
        }
    }
    //inventory full
    return 1;
}

/**
 * @brief Adds effect to critter
 * @return 1 if effects are full, 0 otherwise
*/
int addEffect(critter* c, effect e){
        //search for open spot
    for(int i = 0; i < MAX_EFFECTS; i++){
        if(c->activeEffects[i].type == -1){
            c->activeEffects[i].type = e.type;
            c->activeEffects[i].duration = e.duration;
            charArrayCpy(c->activeEffects[i].name, e.name, MAX_NAME_SIZE);
            return 0;
        }
    }
    //inventory full
    return 1;
}

/**
 * @brief Creates the default data structure stats for a wizard-type critter struct
 * @var startingAeth - 1 - Air, 2 - Dark, 3 - Earth, 4 - fire, 5 - Light, 6 - water, other - enigma
*/
critter* createWizard(char* name, int startingAeth){
    critter* c = (critter*)malloc(sizeof(critter));
    c->wiz = (wizardStats*)malloc(sizeof(wizardStats));
    c->isWizard = 1;
    
    //name
    if(charArrayCpy(c->name, name, MAX_NAME_SIZE)){
        printf("Error: no name given.\n");
        return NULL;
    }

    //hp
    c->hp = createHealth(100, 100.0, 50);
    //Aeth
    c->wiz->affinities = createAffinities(startingAeth);
    //effects
    c->activeEffects = (effect*)malloc(MAX_EFFECTS*sizeof(effect));
    for (int i = 0; i < MAX_EFFECTS; i++){
        clearEffectSlot(c->activeEffects, i);
    }
    //inventory
    c->inventorySize = 4;
    c->inventory = (item*)malloc(4*sizeof(item));
    for (int i = 0; i < 4; i++){
        clearInventorySlot(c->inventory, i);
    }

    //critter values
    c->armorValue = 0;

    return c;
}

/**
 * @brief Creates the  data structure stats for a nonwizard-type critter
*/
critter* createCritter(char* name, int maxHealth, int sanity, int poise, int AV, int invSize, int numEffects, effect* e){
    critter* c = (critter*)malloc(sizeof(critter));
    c->wiz = NULL;
    c->isWizard = 0;
    
    //name
    if(charArrayCpy(c->name, name, MAX_NAME_SIZE)){
        printf("Error: no name given.\n");
        return NULL;
    }

    //hp
    c->hp = createHealth(maxHealth, sanity, poise);
    //effects
    c->activeEffects = (effect*)malloc(MAX_EFFECTS*sizeof(effect));
    for (int i = 0; i < MAX_EFFECTS; i++){
        if(i < numEffects){
            addEffect(c, e[i]);
        }else{
            clearEffectSlot(c->activeEffects, i);
        }
    }
    //inventory
    c->inventorySize = invSize;
    if(invSize){
        c->inventory = (item*)malloc(invSize*sizeof(item));
        for (int i = 0; i < invSize; i++){
            clearInventorySlot(c->inventory, i);
        }
    }else{
        c->inventory = NULL;
    }

    //critter values
    c->armorValue = AV;

    return c;
}