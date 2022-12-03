#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SIZE 15      // Taille du tableau
#define MAXSHIP 10   

const char PORTE_AVION = 'P';
const char CROISEUR = 'C';
const char DESTOYER = 'D';
const char SOUSMARIN = 'S';

const int L_PORTE_AVION = 7; // Longueur de bateau
const int L_CROISEUR = 5;
const int L_DESTOYER = 3;
const int L_SOUSMARIN = 1;

const int MAX_PORTE_AVION = 1; // Nombre maximum de bateau
const int MAX_CROISEUR = 10;
const int MAX_DESTOYER = 3;
const int MAX_SOUSMARIN = 4;

typedef struct{
    int row;      // Ligne
    int col;      // Colonne
}Coords;

typedef struct{
    char type;          // Type du bateau
    int length;         // Longueur du bateau
    char orientation;   // Orientation du bateau
    int life;           // Vie(s) du bateau
    int *states;        // Etat de chaque partie du bateau (0:détruit, 1:intact)
    Coords coord;       // Coordonnées du bateau
}Ship;

typedef struct{
    Ship *ships;        // Bateau(x) du joueur
    int nShip;          //  Nombre de bateaux
    char **tab1;        // Grille 1 du joueur
    char **tab2;        // Grille 2 (celle de l'adversaire)
}Player;


// Prototypes 

void displayGap();
void displayColumnIndex();
void displayLine();
void displayFirstLine();
void displayTabContent(char **tab, int row);
void display2( void (*display)());
void display(Player *player);

Player newPlayer();
Ship newShip(char type);
int placeShip(char **tab, Ship *ship, Coords coord, char orientation);
void placePlayerShips(Player *player);
char **newTab();

int sameCoords(Coords coord1, Coords coord2);

void attack(Player *attacker, Player *target, Coords coord);
void IAattack(Player *attacker, Player *target, Coords coord);
int getCorespondingShipIndex(Player *target, Coords coord);
int belongToShip(Ship *ship, Coords coord);

// Boucle principale

int main(){
    srand(time(NULL));                  // Initialisation du temps
    Player player1 = newPlayer();       // Création du joueur humain (jeu solo)
    Player computer = newPlayer();      // Création de l'IA en tant que joueur

    placePlayerShips(&player1);         // Appel de la fonction qui place les bateaux du joueur humain
    placePlayerShips(&computer);        // Appel de la fonction qui place les bateaux de l'IA

    int choice;
    do{
        display(&player1);              // Afficher les bateaux du joueur humain (ceux de l'IA sont cachés)
        printf("It's your turn to play !\n");
        printf("Menu\n");               
        printf("1-Attack\n");
        printf("2-\n");
        printf("3-\n");
        printf("4-Exit\n");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                attack(&player1, &computer, (Coords){5, 5});   // Appel de la fonction qui permet d'attaquer l'IA 
                printf("Your turn is over. Your opponent is attacking !\n");
                IAattack(&computer, &player1, (Coords){5, 5});
                break;
            default:     
                break;
        }
    }while(choice != 4);    // Blindage du choix du menu
    return 0;
}

// Fonction qui créé un espace entre les tableau

void displayGap(){
    int gapBetweenTab = 3;
    for(int col=0;col<gapBetweenTab;col++){
        printf(" ");
    }
}

// Construction de la première ligne du tableau (Index)

void displayColumnIndex(){
    for(int col=0;col<SIZE+1;col++){
        if(col == 0){
            printf("| / ");
        }
        else{
            printf("|%2d ", col - 1);
        }
        
        if(col == SIZE + 1 - 1){
            printf("|");
        }
    }
}

// Fonction qui créé une des lignes de séparation des cases

void displayLine(){
    for(int col=0;col<SIZE+1;col++){
        printf("|___");
        if(col == SIZE + 1 - 1){
            printf("|");
        }
    }
}

// Affiche la toute première ligne qui ferme le tableau 

void displayFirstLine(){
    for(int col=0;col<SIZE+1;col++){
        printf(" ___");
        if(col == SIZE + 1 - 1){
            printf(" ");
        }
    }
}

// Fonction qui affiche le contenu du tableau

void displayTabContent(char **tab, int row){
    for(int col=0;col<SIZE+1;col++){
        if(col == 0){
            printf("|%2c ", 'a' + row/2 - 1);
        }
        else{
            printf("|%2c ", tab[row/2 - 1][col - 1]);
        }
        if(col == SIZE + 1 - 1){
            printf("|");
        }
    }
}

// Fonction qui appelle les deux fonctions d'affichages ainsi que la fonction qui créé l'espace entre les tableaux

void display2( void (*display)()){
    (*display)();
    displayGap();
    (*display)();
}

// Fonction qui gère l'affichage 

void display(Player *player){
    int tabSize = SIZE + 1;

    for(int row=0;row<2*tabSize;row++){
        if(row%2 != 0){
            if(row == 1){
                display2(displayColumnIndex);
            }
            else{
                displayTabContent(player->tab1, row);
                displayGap();
                displayTabContent(player->tab2, row);
            }
            printf("\n");
        }
        else{
            if(row != 0){
                display2(displayLine);
            }
            else{
                display2(displayFirstLine);
            }
            printf("\n");
        }
    }
    display2(displayLine);
    printf("\n");
}

// Fonction qui créé un nouveau joueur

Player newPlayer(){
    Player player;
    int totalNumberOfShip = MAX_CROISEUR + MAX_DESTOYER + MAX_PORTE_AVION + MAX_SOUSMARIN;  // Bateaux à avoir initialement
    player.ships = malloc(totalNumberOfShip * sizeof(Ship));                               // Allocation dynamique du tableau de bateaux
    int shipIndex = 0;
    for(int i=0;i<MAX_PORTE_AVION;i++){
        player.ships[shipIndex++] = newShip(PORTE_AVION);
    }
    for(int i=0;i<MAX_CROISEUR;i++){
        player.ships[shipIndex++] = newShip(CROISEUR);
    }
    for(int i=0;i<MAX_DESTOYER;i++){
        player.ships[shipIndex++] = newShip(DESTOYER);
    }
    for(int i=0;i<MAX_SOUSMARIN;i++){
        player.ships[shipIndex++] = newShip(SOUSMARIN);
    }
    player.nShip = totalNumberOfShip;    // Sauvegarde du nombre total de bateaux
    player.tab1 = newTab();              // Appel de la fonction qui créé le tableau 1
    player.tab2 = newTab();              // Idem pour le tableau 2
    return player;
}

// Fonction qui créé un nouveau bateau

Ship newShip(char type){
    Ship ship;
    ship.type = type;                // Sauvegarde du type entré en paramètre
    if(type == PORTE_AVION){
        ship.length = L_PORTE_AVION;
    }
    else if(type == DESTOYER){
        ship.length = L_DESTOYER;
    }
    else if(type == CROISEUR){
        ship.length = L_CROISEUR;
    }
    else if(type == SOUSMARIN){
        ship.length = L_SOUSMARIN;
    }
    ship.life = ship.length;                             // La vie vaut le nombre de case que prend le bateau
    ship.states = malloc(ship.length * sizeof(int));     // Allocation dynamique du tableau de l'état de chaque partie

    for(int i=0;i<ship.length;i++){
        ship.states[i] = 1;                             // Initialisation de l'état
    }
    return ship;
}

// Fonction qui place le bateau d'un joueur aléatoirement dans la grille

void placePlayerShips(Player *player){
    for(int shipIndex=0;shipIndex<player->nShip;shipIndex++){   // Pour i allant de 0 au nombre de bateaux du joueur :
        char orientation;                                      
        int row;
        int col;
        do{
            // printf("i : %d %c\n", shipIndex, player->ships[shipIndex].type);
            if(rand() % 2 == 0){ // vertical orientation
                orientation = 'h';
                row = rand() % SIZE;
                col = rand() % (SIZE - player->ships[shipIndex].length + 1);
            }
            else{ // horizontal orientation
                orientation = 'v';
                row = rand() % (SIZE - player->ships[shipIndex].length + 1);
                col = rand() % SIZE;
            }
            // printf("Coords : %c-%d-%c\n", 'a' + row, col, orientation);
        }while(!placeShip(player->tab1, &player->ships[shipIndex], (Coords){row, col}, orientation));  // Placement du bateau
    }
}

// Fonction qui sauvegarde et écrit le placement de chaque bateau dans la grille du joueur

int placeShip(char **tab, Ship *ship, Coords coord, char orientation){
    int shipPlaced = 0;                 // Boolean permettant de savoir si le bateau a bien été placé
    int count = 0;                      // Initialisation du compteur à 0
    if(orientation == 'h'){            
        while(count < ship->length && tab[coord.row][coord.col + count] == ' '){   // Incrementation sur la colonne car horizontal
            count++;
        }
        if(count == ship->length){                      // Si le compteur représente bien la longueur du bateau horizontalement :
            ship->coord = coord;                        // Sauvegarde des coordonées du bateau
            ship->orientation = orientation;            // Sauvegarde de l'orientation du tableau
            for(int i=0;i<ship->length;i++){
                tab[coord.row][coord.col + i] = ship->type;  // Ecriture de la lettre du type dans les cases correspondantes au placement 
            }
            shipPlaced = 1;    // Bateau placé
        }        
    }
    else if(orientation == 'v'){
        while(count < ship->length && tab[coord.row + count][coord.col] == ' '){  // Incrementation sur la colonne car vertical
            count++;
        }
        if(count == ship->length){
            ship->coord = coord;
            ship->orientation = orientation;
            for(int i=0;i<ship->length;i++){
                tab[coord.row + i][coord.col] = ship->type;
            }
            shipPlaced = 1;
        }  
    }
    return shipPlaced;
}

// Fonction qui créé un tableau dynamique

char **newTab(){
    char **tab = malloc(SIZE * sizeof(char*));
    for(int row=0;row<SIZE;row++){
        tab[row] = malloc(SIZE * sizeof(char));
    }
    for(int row=0;row<SIZE;row++){
        for(int col=0;col<SIZE;col++){
            tab[row][col] = ' ';            // Le tableau est complètement vide initialement
        }
    }
    return tab;
}


// Fonction qui traite l'attaque d'un joueur sur une cible | 'a' vaut 97 transtypé en int on initialisera à cette valeur par facilité

void attack(Player *attacker, Player *target, Coords coord){
    printf("Enter attack's coordinates : ");
    char row;
    getchar();
    scanf("%c %d", &row, &coord.col);
    coord.row = row - 'a';                 // Initialisation de la ligne des coordonnées

    printf("Attack at %c%d : ", 'a' + coord.row, coord.col);
    if(target->tab1[coord.row][coord.col] != ' '){              // Si la case est non-vide
        attacker->tab2[coord.row][coord.col] = 'T';             // Touché
        printf("Touched\n");
        printf("The ship number %d is under attack !\n", getCorespondingShipIndex(target, coord));
    }
    else{
        attacker->tab2[coord.row][coord.col] = 'E';
        printf("Missed\n");                                    // Raté
    }
}

// Fonction qui gère le tour de l'IA

void IAattack(Player *attacker, Player *target, Coords coord){
char row = rand() % SIZE;
coord.col = rand() % SIZE;
coord.row = row;

printf("Your opponent attacks at %c%d : ", 'a' + coord.row, coord.col);
if(target->tab1[coord.row][coord.col] != ' '){              // Si la case est non-vide
        target->tab1[coord.row][coord.col] = 'T';             // Touché
        printf("Touched\n");
        printf("%d\n", getCorespondingShipIndex(target, coord));
    }
else{
        target->tab1[coord.row][coord.col] = 'E';
        printf("Missed\n");                                    // Raté
    }

}

// Fonction qui compare les coordonnées entrées en paramètre et renvoie le booléen correspondant

int sameCoords(Coords coord1, Coords coord2){
    // printf("%c%d %c%d\n", 'a' + coord1.row, coord1.col, 'a' + coord2.row, coord2.col);
    return (coord1.col == coord2.col && coord1.row == coord2.row);
}

//  Fonction qui renvoie l'index du bateau touché

int getCorespondingShipIndex(Player *target, Coords coord){
    int index = 0;
    while(!belongToShip(&target->ships[index], coord)){
        index++;
    }
    return index;
}

// Fonction qui prend en compte la réduction de vie du bateau et change l'état de la partie concernée

void attackSucceed(Ship *ship, int shipPartIndex){
    ship->states[shipPartIndex] = 0;
    if(ship->life > 0){
        ship->life--;
    }
    
}

// Fonction qui permet de détecter si la case concernée appartient à un bateau

int belongToShip(Ship *ship, Coords coord){
    // printf("type : %c, length : %d", ship->type, ship->length);
    // printf(", orientation : %c, coord : %c%d\n",ship->orientation, 'a' + ship->coord.row, ship->coord.col);
        
    int belong = 0;                  // Booléen sur l'appartenance

    // Deux cas à traiter : vertical et horizontal

    if(ship->orientation == 'v'){
        for(int i=0;i<ship->length;i++){
            if(sameCoords(coord, (Coords){ship->coord.row + i, ship->coord.col})){  
                attackSucceed(ship, i);          // Sauvegarde de la perte de vie et l'état de la partie du bateau
                belong = 1;
            }
        }
    }
    else if(ship->orientation == 'h'){
        for(int i=0;i<ship->length;i++){
            if(sameCoords(coord, (Coords){ship->coord.row, ship->coord.col + i})){
                attackSucceed(ship, i);
                belong = 1;
            }
        }
    }
    return belong;
}