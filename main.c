#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <Windows.h>
#include <string.h>
#include <dirent.h>
#include <conio.h>

#pragma warning(disable : 4996)

#define SIZE 15      // Taille du tableau
#define MAXSHIP 10   

const char PORTE_AVION = 'P';
const char CROISEUR = 'C';
const char DESTOYER = 'D';
const char SOUSMARIN = 'S';

// Longueur de bateau
const int L_PORTE_AVION = 7;
const int L_CROISEUR = 5;
const int L_DESTOYER = 3;
const int L_SOUSMARIN = 1;

// Nombre maximum de bateau
const int MAX_PORTE_AVION = 1;
const int MAX_CROISEUR = 2;
const int MAX_DESTOYER = 3;
const int MAX_SOUSMARIN = 4;

const char* SAVE_DIR = "SavedGames";


typedef struct {
    int row;      // Ligne
    int col;      // Colonne
}Coords;


typedef struct {
    char type;          // Type du bateau
    int length;         // Longueur du bateau
    char orientation;   // Orientation du bateau
    int life;           // Vie(s) du bateau
    int* states;        // Etat de chaque partie du bateau (0:détruit, 1:intact)
    Coords coord;       // Coordonn�es du bateau
}Ship;


typedef struct {
    Ship* ships;        // Bateau(x) du joueur
    int nShip;          //  Nombre de bateaux
    char** tab1;        // Grille 1 du joueur
    char** tab2;        // Grille 2 (celle de l'adversaire)
}Player;

void Load(Player* player, Player* computer)
{

}


// Prototypes 
void displayGap();
void displayColumnIndex();
void displayLine();
void displayFirstLine();
void displayTabContent(char** tab, int row);
void display2(void (*display)());
void display(Player* player);
int sameCoords(Coords coord1, Coords coord2);
void attack(Player* attacker, Player* target, Coords coord);
int getCorespondingShipIndex(Player* target, Coords coord);
int belongToShip(Ship* ship, Coords coord);
Player newPlayer();
Ship newShip(char type);
void attackSucceed(Ship* ship);
int placeShip(char** tab, Ship* ship, Coords coord, char orientation);
void placePlayerShips(Player* player);
char** newTab();
int sameCoords(Coords coord1, Coords coord2);

//
void reveal(Player* attacker, Player* target, Coords coord);
void move(Player* target, Coords coord);
void step(Ship* ship, Player* player, char dir);

void IAattack(Player* attacker, Player* target, Coords coord);
void play();
void Save(Player player, Player computer);
void savePlayer(Player p, FILE* fp);
void saveShip(Ship ship, FILE* fp);
void saveCoord(Coords coord, FILE* fp);
void Load(Player* player, Player* computer);
//

//vide le buffer en lisant un à un les caractères présent dans celui-ci
void vider_buffer(void)
{
    int c;

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

// Boucle principale

int main() {

    bool quit = false;
    while (!quit)
    {
        //il faut demander s'il veut démmarer un nouveau jeu ou charger un jeu existant
        printf("Menu\n");
        printf("   -1- New game\n");
        printf("   -2- Load game\n");
        printf("   -3- Help\n");
        printf("   -4- Exit\n");
        printf("Enter your choice : ");
        int choicee;
        scanf("%d", &choicee);


        switch (choicee)
        {
        case 1:
        {
            srand(time(NULL));                  // Initialisation du temps
            play();
        }
        case 2:
        {
            struct dirent* dir;
            // opendir() renvoie un pointeur de type DIR. 
            DIR* d = opendir(".");
            if (d)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    printf("%s\n", dir->d_name);
                }
                closedir(d);
            }
        }break;
        case 4:
        {
            quit = true;
            exit(0);
        }break;
        default:
            break;
        }
        if (choicee == 3)
        {
            FILE* fp = NULL;
            int character;
            fp = fopen("C://Users//araso//source//repos//Project_Battle//x64//Debug//README.txt", "r");
            if (fp == NULL)
            {
                printf("The file didn't opened\n");

            }
            else {
                while ((character = fgetc(fp)) != EOF)
                {
                    printf("%c", character);
                }
            }

            fclose(fp);
        }
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui crée un espace entre les tableaux

void displayGap() {
    int gapBetweenTab = 3;
    for (int col = 0; col < gapBetweenTab; col++) {
        printf(" ");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Construction de la premi�re ligne du tableau (Index)


void displayColumnIndex() {
    for (int col = 0; col < SIZE + 1; col++) {
        if (col == 0) {
            printf("| / ");
        }
        else {
            printf("|%2d ", col - 1);
        }

        if (col == SIZE + 1 - 1) {
            printf("|");
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui cr�� une des lignes de s�paration des cases

void displayLine() {
    for (int col = 0; col < SIZE + 1; col++) {
        printf("|___");
        if (col == SIZE + 1 - 1) {
            printf("|");
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Affiche la toute premi�re ligne qui ferme le tableau 


void displayFirstLine() {
    for (int col = 0; col < SIZE + 1; col++) {
        printf(" ___");
        if (col == SIZE + 1 - 1) {
            printf(" ");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui affiche le contenu du tableau

void displayTabContent(char** tab, int row) {
    for (int col = 0; col < SIZE + 1; col++) {
        if (col == 0) {
            printf("|%2c ", 'a' + row / 2 - 1);
        }
        else {
            printf("|%2c ", tab[row / 2 - 1][col - 1]);
        }
        if (col == SIZE + 1 - 1) {
            printf("|");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui appelle les deux fonctions d'affichages ainsi que la fonction qui cr�� l'espace entre les tableaux

void display2(void (*display)()) {
    (*display)();
    displayGap();
    (*display)();
}



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui g�re l'affichage 


void display(Player* player) {
    int tabSize = SIZE + 1;

    for (int row = 0; row < 2 * tabSize; row++) {
        if (row % 2 != 0) {
            if (row == 1) {
                display2(displayColumnIndex);
            }
            else {
                displayTabContent(player->tab1, row);
                displayGap();
                displayTabContent(player->tab2, row);
            }
            printf("\n");
        }
        else {
            if (row != 0) {
                display2(displayLine);
            }
            else {
                display2(displayFirstLine);
            }
            printf("\n");
        }
    }
    display2(displayLine);
    printf("\n");
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui cr�� un nouveau joueur

Player newPlayer() {
    Player player;
    int totalNumberOfShip = MAX_CROISEUR + MAX_DESTOYER + MAX_PORTE_AVION + MAX_SOUSMARIN;  // Bateaux � avoir initialement
    player.ships = malloc(totalNumberOfShip * sizeof(Ship));                               // Allocation dynamique du tableau de bateaux
    int shipIndex = 0;
    for (int i = 0; i < MAX_PORTE_AVION; i++) {
        player.ships[shipIndex++] = newShip(PORTE_AVION);
    }
    for (int i = 0; i < MAX_CROISEUR; i++) {
        player.ships[shipIndex++] = newShip(CROISEUR);
    }
    for (int i = 0; i < MAX_DESTOYER; i++) {
        player.ships[shipIndex++] = newShip(DESTOYER);
    }
    for (int i = 0; i < MAX_SOUSMARIN; i++) {
        player.ships[shipIndex++] = newShip(SOUSMARIN);
    }
    player.nShip = totalNumberOfShip;    // Sauvegarde du nombre total de bateaux
    player.tab1 = newTab();              // Appel de la fonction qui cr�� le tableau 1
    player.tab2 = newTab();              // Idem pour le tableau 2
    return player;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui cr�� un nouveau bateau

Ship newShip(char type) {
    Ship ship;
    ship.type = type;                // Sauvegarde du type entr� en param�tre
    if (type == PORTE_AVION) {
        ship.length = L_PORTE_AVION;
    }
    else if (type == DESTOYER) {
        ship.length = L_DESTOYER;
    }
    else if (type == CROISEUR) {
        ship.length = L_CROISEUR;
    }
    else if (type == SOUSMARIN) {
        ship.length = L_SOUSMARIN;
    }
    ship.life = ship.length;                             // La vie vaut le nombre de case que prend le bateau
    ship.states = malloc(ship.length * sizeof(int));     // Allocation dynamique du tableau de l'�tat de chaque partie

    for (int i = 0; i < ship.length; i++) {
        ship.states[i] = 1;                             // Initialisation de l'�tat
    }
    return ship;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui place le bateau d'un joueur al�atoirement dans la grille

void placePlayerShips(Player* player) {
    for (int shipIndex = 0; shipIndex < player->nShip; shipIndex++) {   // Pour i allant de 0 au nombre de bateaux du joueur :
        char orientation;
        int row;
        int col;
        do {
            // printf("i : %d %c\n", shipIndex, player->ships[shipIndex].type);
            if (rand() % 2 == 0) { // vertical orientation
                orientation = 'h';
                row = rand() % SIZE;
                col = rand() % (SIZE - player->ships[shipIndex].length + 1);
            }
            else { // horizontal orientation
                orientation = 'v';
                row = rand() % (SIZE - player->ships[shipIndex].length + 1);
                col = rand() % SIZE;
            }
            // printf("Coords : %c-%d-%c\n", 'a' + row, col, orientation);
        } while (!placeShip(player->tab1, &player->ships[shipIndex], (Coords) { row, col }, orientation));  // Placement du bateau
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui sauvegarde et �crit le placement de chaque bateau dans la grille du joueur

int placeShip(char** tab, Ship* ship, Coords coord, char orientation) {
    int shipPlaced = 0;                 // Boolean permettant de savoir si le bateau a bien �t� plac�
    int count = 0;                      // Initialisation du compteur � 0
    if (orientation == 'h') {
        while (count < ship->length && tab[coord.row][coord.col + count] == ' ') {   // Incrementation sur la colonne car horizontal
            count++;
        }
        if (count == ship->length) {                      // Si le compteur repr�sente bien la longueur du bateau horizontalement :
            ship->coord = coord;                        // Sauvegarde des coordon�es du bateau
            ship->orientation = orientation;            // Sauvegarde de l'orientation du tableau
            for (int i = 0; i < ship->length; i++) {
                tab[coord.row][coord.col + i] = ship->type;  // Ecriture de la lettre du type dans les cases correspondantes au placement 
            }
            shipPlaced = 1;    // Bateau plac�
        }
    }
    else if (orientation == 'v') {
        while (count < ship->length && tab[coord.row + count][coord.col] == ' ') {  // Incrementation sur la colonne car vertical
            count++;
        }
        if (count == ship->length) {
            ship->coord = coord;
            ship->orientation = orientation;
            for (int i = 0; i < ship->length; i++) {
                tab[coord.row + i][coord.col] = ship->type;
            }
            shipPlaced = 1;
        }
    }
    return shipPlaced;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui cr�� un tableau dynamique

char** newTab() {
    char** tab = malloc(SIZE * sizeof(char*));
    for (int row = 0; row < SIZE; row++) {
        tab[row] = malloc(SIZE * sizeof(char));
    }
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            tab[row][col] = ' ';            // Le tableau est compl�tement vide initialement
        }
    }
    return tab;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


// Fonction qui traite l'attaque d'un joueur sur une cible | 'a' vaut 97 transtyp� en int on initialisera � cette valeur par facilit�

void attack(Player* attacker, Player* target, Coords coord) {
    printf("Enter attack's coordinates : ");
    char row;
    getchar();
    scanf("%c %d", &row, &coord.col);
    coord.row = row - 'a';                 // Initialisation de la ligne des coordonnées

    printf("Attack at %c%d : ", 'a' + coord.row, coord.col);
    if (target->tab1[coord.row][coord.col] != ' ') {              // Si la case est non-vide
        attacker->tab2[coord.row][coord.col] = 'T';             // Touché
        int Index = getCorespondingShipIndex(target, coord);
        attackSucceed(&target->ships[Index]); // Sauvegarde de la perte de vie           
        printf("Touched\n");
        printf("The ship number %d is under attack !\n", Index);
        if (target->ships[Index].life == 0)
        {
            printf("The ship number %d is destroyed\n", Index);
        }
    }
    else {
        attacker->tab2[coord.row][coord.col] = 'E';
        printf("Missed\n");                                    // Raté
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui compare les coordonn�es entr�es en param�tre et renvoie le bool�en correspondant

int sameCoords(Coords coord1, Coords coord2) {
    // printf("%c%d %c%d\n", 'a' + coord1.row, coord1.col, 'a' + coord2.row, coord2.col);
    return (coord1.col == coord2.col && coord1.row == coord2.row);
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

//  Fonction qui renvoie l'index du bateau touch�

int getCorespondingShipIndex(Player* target, Coords coord) {
    int index = 0;
    while (!belongToShip(&target->ships[index], coord)) {
        if (index > 9)
        {
            printf("No ship here.\n");
            break;
        }
        index++;
    }
    return index;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui prend en compte la r�duction de vie du bateau et change l'�tat de la partie concern�e

void attackSucceed(Ship* ship) {
    // ship->states[shipPartIndex] = 0;
    if (ship->life > 0) {
        ship->life--;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui permet de d�tecter si la case concern�e appartient � un bateau

int belongToShip(Ship* ship, Coords coord) {
    // printf("type : %c, length : %d", ship->type, ship->length);
    // printf(", orientation : %c, coord : %c%d\n",ship->orientation, 'a' + ship->coord.row, ship->coord.col);

    int belong = 0;                  // Booléen sur l'appartenance

    // Deux cas à traiter : vertical et horizontal
    if (ship->orientation == 'v') {
        for (int i = 0; i < ship->length; i++) {
            if (sameCoords(coord, (Coords) { ship->coord.row + i, ship->coord.col })) {
                // attackSucceed(ship, i);          
                belong = 1;
            }
        }
    }
    else if (ship->orientation == 'h') {
        for (int i = 0; i < ship->length; i++) {
            if (sameCoords(coord, (Coords) { ship->coord.row, ship->coord.col + i })) {
                //  attackSucceed(ship, i);
                belong = 1;
            }
        }
    }
    return belong;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



void reveal(Player* attacker, Player* target, Coords coord) {

    int effacer[4][4];
    char row;

    // Blindage pour éviter les bugs sur extremum
    do {
        printf("Your flash will reveal a 4*4 zone, enter the up-left coordinate of the square that you want to reveal.\n");
        getchar();
        scanf("%c %d", &row, &coord.col);
        coord.row = row;
        if (coord.row == 'o' || coord.row == 'n' || coord.row == 'm' || coord.col == 14 || coord.col == 13 || coord.col == 12)
        {
            printf("Wrong coordinates.\n");
        }
    } while (coord.row == 'o' || coord.row == 'n' || coord.row == 'm' || coord.col == 14 || coord.col == 13 || coord.col == 12);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (attacker->tab2[coord.row - 'a' + i][coord.col + j] != 'E' && attacker->tab2[coord.row - 'a' + i][coord.col + j] != 'T')
            {
                attacker->tab2[coord.row - 'a' + i][coord.col + j] = target->tab1[coord.row - 'a' + i][coord.col + j];
                effacer[i][j] = 1;        // Boolean sur l'effacement
            }
        }
    }

    display(attacker);
    Sleep(3000);
    system("cls");

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (effacer[i][j] == 1)
            {
                attacker->tab2[coord.row - 'a' + i][coord.col + j] = ' ';
            }
        }
    }
}


/////////////////////////////////////////////////////

void step(Ship* ship, Player* player, char dir)
{
    char s[7];
    if (ship->orientation == 'h')
    {
        if (dir == 'e' && ship->coord.col + ship->length - 1 == 14 )
        {
            printf("You can't move to this direction, you have reached the limit of the map !\n");
        }
        else if (dir == 'e')
        {
            if (player->tab1[ship->coord.row][ship->coord.col + ship->length] == 'P' || player->tab1[ship->coord.row][ship->coord.col + ship->length] == 'C' || player->tab1[ship->coord.row][ship->coord.col + ship->length] == 'D' || player->tab1[ship->coord.row][ship->coord.col + ship->length] == 'S')
            {
                printf("You can't move here, a ship is already there !\n");
            }
            else
            {
                for (int i = 0; i < ship->length; i++)
                {
                    s[i] = player->tab1[ship->coord.row][ship->coord.col + i];  // Tableau qui sauvegarde
                    player->tab1[ship->coord.row][ship->coord.col + i + 1] = s[i]; // Décalage de 1
                }
                player->tab1[ship->coord.row][ship->coord.col] = ' '; // Suppression de la première case
                ship->coord.col++; // Actualisation de la nouvelle coordonnée de construction (Avancer)
            }
        }
        else if (dir == 'o' && ship->coord.col == 0 )
        {
            printf("You can't move to this direction, you have reached the limit of the map !\n");
        }
        else if (dir == 'o')
        {
            if (player->tab1[ship->coord.row][ship->coord.col - 1] == 'P' || player->tab1[ship->coord.row][ship->coord.col - 1] == 'C' || player->tab1[ship->coord.row][ship->coord.col - 1] == 'D' || player->tab1[ship->coord.row][ship->coord.col - 1] == 'S')
            {
                printf("You can't move here, a ship is already there !\n");
            }
            else
            {
                for (int i = 0; i < ship->length; i++)
                {
                    s[i] = player->tab1[ship->coord.row][ship->coord.col + i];  // Tableau qui sauvegarde
                    player->tab1[ship->coord.row][ship->coord.col + i - 1] = s[i]; // Décalage de 1
                }

                player->tab1[ship->coord.row][ship->coord.col + ship->length - 1] = ' '; // Suppression de la dernière case
                ship->coord.col--; // Actualisation de la nouvelle coordonnée de construction (Reculer)
            }

        }
        else
        {
            printf("You have chosen the wrong direction !\n");
        }
    }

    else if (ship->orientation == 'v')
    {
        if (dir == 's' && 'a' + ship->coord.row + ship->length - 1 == 'o' )
        {
            printf("You can't move to this direction, you have reached the limit of the map !\n");
        }
        else if (dir == 's')
        {
            if (player->tab1[ship->coord.row + ship->length][ship->coord.col] == 'P' || player->tab1[ship->coord.row + ship->length][ship->coord.col] == 'C' || player->tab1[ship->coord.row + ship->length][ship->coord.col] == 'D' || player->tab1[ship->coord.row + ship->length][ship->coord.col] == 'S')
            {
                printf("You can't move here, a ship is already there !\n");
            }
            else
            {
                for (int i = 0; i < ship->length; i++)
                {
                    s[i] = player->tab1[ship->coord.row + i][ship->coord.col];  // Tableau qui sauvegarde
                    player->tab1[ship->coord.row + i + 1][ship->coord.col] = s[i]; // Décalage de 1
                }

                player->tab1[ship->coord.row][ship->coord.col] = ' '; // Suppression de la première case
                ship->coord.row++; // Actualisation de la nouvelle coordonnée de construction (Descendre)
            }

        }
        else if (dir == 'n' && 'a' + ship->coord.row == 'a' )
        {
            printf("You can't move to this direction, you have reached the limit of the map !\n");
        }
        else if (dir == 'n')
        {
            if (player->tab1[ship->coord.row - 1][ship->coord.col] == 'P' || player->tab1[ship->coord.row - 1][ship->coord.col] == 'C' || player->tab1[ship->coord.row - 1][ship->coord.col] == 'D' || player->tab1[ship->coord.row - 1][ship->coord.col] == 'S')
            {
                printf("You can't move here, a ship is already there !\n");
            }
            else
            {
                for (int i = 0; i < ship->length; i++)
                {
                    s[i] = player->tab1[ship->coord.row + i][ship->coord.col];  // Tableau qui sauvegarde
                    player->tab1[ship->coord.row + i - 1][ship->coord.col] = s[i]; // Décalage de 1
                }

                player->tab1[ship->coord.row + ship->length - 1][ship->coord.col] = ' '; // Suppression de la dernière case
                ship->coord.row--; // Actualisation de la nouvelle coordonnée de construction (Monter)
            }

        }
        else
        {
            printf("You have chosen the wrong direction !\n");
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



void move(Player *target, Coords coord){
    char d = 'x';
    coord.row = 'x';
    coord.col = -1;
    while (d != 'n' && d != 's' && d != 'e' && d != 'o'  )
    {
        printf("Type the letter of the direction.\n");
        scanf("%s", &d);
        if (d != 'n' && d != 's' && d != 'e' && d != 'o' )
        {
            printf("Wrong direction.\n");
        }
    }
    int Index = 10;
    while (Index > 9 || (coord.row > 'a' + 'o' || coord.row < 'a' + 'a') && (coord.col < 0 || coord.col > 14) ){
        printf("Type one of the coordinates of the ship that you want to move.\n");
        char row;
        getchar();
        scanf("%c %d", &row, &coord.col);
        coord.row = row - 'a';
        if ( (coord.row > 'a' + 'o' || coord.row < 'a' + 'a') && (coord.col < 0 || coord.col > 14) )
        {
            printf("Wrong coordinates.\n");
        }
        Index = getCorespondingShipIndex(target, coord);
    }
    
    step(&target->ships[Index], target, d);
}



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui g�re le tour de l'IA

void IAattack(Player* attacker, Player* target, Coords coord) {

    char row = rand() % SIZE;
    coord.col = rand() % SIZE;
    coord.row = row;

    printf("Your opponent attacks at %c%d : ", 'a' + coord.row, coord.col);
    if (target->tab1[coord.row][coord.col] != ' ') {              // Si la case est non-vide
        target->tab1[coord.row][coord.col] = 'T';             // Touché
        printf("Touched\n");
        int Index = getCorespondingShipIndex(target, coord);
        printf("Your ship number %d is under attack !\n", Index);
        attackSucceed(&target->ships[Index]); // Sauvegarde de la perte de vie 
        if (target->ships[Index].life == 0)
        {
            printf("Your ship number %d is destroyed\n", Index);
        }
    }
    else {
        target->tab1[coord.row][coord.col] = 'E';
        printf("Missed\n");                                    // Raté
    }

}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

void play()
{

    Player p;
    Player player1 = newPlayer();                                                   // Création du joueur humain (jeu solo)
    Player computer = newPlayer();                                                  // Création de l'IA en tant que joueur

    placePlayerShips(&player1);                                                     // Appel de la fonction qui place les bateaux du joueur humain
    placePlayerShips(&computer);                                                    // Appel de la fonction qui place les bateaux de l'IA
    
    char ch;
    int choice;
    bool tourIA = false;                                                            // L'humain commnce au début
    int flares = 0;                                                                  // On a le droit à 4 flares

    do {
        Sleep(1500);
        system("cls");
        display(&player1);                                                          // Afficher les bateaux du joueur humain (ceux de l'IA sont cach�s)
        if (tourIA == false)
        {   
            printf("\nAppuez sur n'importe quelle touche pour ouvrir le menu\n");
            if (!kbhit())
            {
                ch =  getch();
                if ((int) ch == 118)
                {
                    printf("Cheat mode : ON\n");
                    for (int i =0; i < SIZE; i++)
                    {
                        for (int j = 0; j < SIZE; j++)
                        {
                            player1.tab2[i][j] = computer.tab1[i][j];
                        }
                    }   
                }
            }

            printf("It's your turn to play !\n\n\n");
            printf("\t\t------- Menu -------\n\n\n");
            printf("\t\t-1- Missile\n");
            printf("\t\t-2- Move the Ship \n");
            printf("\t\t-3- Flare\n");
            printf("\t\t-4- Save the Game\n");
            printf("\t\t-5- Exit \n\n\n");
            printf("What Option you selected ? : ");
            scanf("%d", &choice);

            switch (choice) {
            case 1:
            {
                attack(&player1, &computer, (Coords) { 5, 5 });               // Appel de la fonction qui permet d'attaquer l'IA 
                printf("Your turn is over. Your opponent is attacking !\n");
                tourIA = true;                                                // C'est au tour de l'IA une fois l'attaque terminée
            }break;
            case 2:
            {
                move(&player1, (Coords) { 5, 5 });                           // Appel de la fonction qui permet de faire bouger un bateau
                tourIA = true;                                               // C'est au tour de l'IA une fois l'attaque terminée
            }break;
            case 3:
            {
                // RAPPEL ON A LE DROIT QU'A 4 FLARES
                if (flares == 4)
                {
                    printf("You don't have any more flares !\n");
                    break;
                }
                reveal(&player1, &computer, (Coords) { 5, 5 });             // Appel de la fonction qui révèle une partie des cases     
                flares++;
                tourIA = true;                                             // C'est au tour de l'IA une fois l'attaque terminée
            }
            break;
            case 4:
            {

                Save(player1, computer);                                   // Appel de la fonction qui sauvegarde la partie en cours
            }break;
            case 5:
                exit(0);                                                   // Arrête le programme
                break;
            default:
                break;
            }
        }
        else
        {
            IAattack(&computer, &player1, (Coords) { 5, 5 });              // Quand TourIA == true, on appelle la fonction IA Attack
            tourIA = false;                                               // Tour de l'humain
        }
    } while (choice != 5);                                                // Blindage du choix du menu
}


//===============================//
//=         SAUVEGARDE          =//
//===============================//


// Fonction qui sauvegarde les coordonées d'un bateaux

void saveCoord(Coords coord, FILE* fp)
{
    // Ecriture dans le fichier texte des coordonnée
    fprintf(fp, "{\n");
    fprintf(fp, "row=%d\ncol=%d\n", coord.row, coord.col);
    fprintf(fp, "}\n");
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui sauvegarde les attributs de la class Ship comprenant les coordonnées de chaque ship

void saveShip(Ship ship, FILE* fp)
{
    fprintf(fp, "{\n");
    fprintf(fp, "type=%c\nlength=%d\norientation=%c\nlife=%d\n", ship.type, ship.length, ship.orientation, ship.life);
    // 1 n bonne état sinon 0 : détruit
    fprintf(fp, "states=[");
    fprintf(fp, "%d", ship.states[0]);
    for (int j = 1; j < ship.length; ++j)
        fprintf(fp, ",%d", ship.states[j]);
    fprintf(fp, "]\n");
    //

    fprintf(fp, "coord=\n");
    saveCoord(ship.coord, fp);
    fprintf(fp, "}\n");
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Fonction qui sauvegarde cete fois les atributs de la class Player comprenant les infos de chaqu ship

void savePlayer(Player p, FILE* fp)
{
    fprintf(fp, "{\n");
    fprintf(fp, "nShip=%d\n", p.nShip);
    fprintf(fp, "ships=[\n");
    // Boucle pour récupérer les infos sur chaque Ship du joueur
    saveShip(p.ships[0], fp);
    for (int i = 1; i < p.nShip; ++i)
    {
        fprintf(fp, ",\n");
        saveShip(p.ships[i], fp);
    }
    fprintf(fp, "]\n");
    // La grille 1 est enregistré sur une seule ligne
    fprintf(fp, "tab1=[");
    fprintf(fp, "%d", p.tab1[0][0]);
    for (int i = 1; i < 15; ++i)
    {
        for (int j = 1; j < 15; ++j)
        {
            fprintf(fp, ",%d", p.tab1[i][j]);
        }
    }
    fprintf(fp, "]\n");
    //
    // La grille 2 est enregistré sur une seule ligne 
    fprintf(fp, "tab2=[");
    fprintf(fp, "%d", p.tab2[0][0]);
    for (int i = 1; i < 15; ++i)
    {
        for (int j = 1; j < 15; ++j)
        {
            fprintf(fp, ",%d", p.tab2[i][j]);
        }
    }
    fprintf(fp, "]\n");
    fprintf(fp, "}\n");
    //
}



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



// Fonction qui sauvegarde cete fois les informations completes sur la partie pour chaque joueur
void Save(Player player, Player computer)
{
    char file_name[256];
    printf("Donnez un nom a votre fichier de sauvegarde: ");
    int f = scanf("%s", file_name);

    // Ouverture du fichier
    FILE* fichier = fopen(file_name, "w");

    // Condition d'ouverture
    if (fichier == NULL)
    {
        printf("Impossible d'ouvrir le fichier de sauvegarde");
    }
    else
    {
        // On sauvegarde tout
        //if (IsTourIA == true)
        //    fprintf(fichier, "TourIA=1\n");
        //else
        //{
        //    fprintf(fichier, "TourIA=0\n");
        //}
        savePlayer(player, fichier);
        savePlayer(computer, fichier);
        fclose(fichier);
    }
}


//===============================//
//=        RESTAURATION         =//
//===============================//






