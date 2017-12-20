#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <string.h> //strcmp
#include <vector>
#include <cmath> //abs
#include <sys/wait.h>
#include <signal.h>

using namespace std;

/*
 * contient l'informtation nécessaire pour la sauvegarde de l'état des cases
 */
struct cell
{
    int cellType;
    bool hit;
    bool usedSquare;
    cell(){}
    cell(int cellType, bool hit, bool usedSquare = false)
    {
        this->cellType = cellType;
        this->hit = hit;
        this->usedSquare = usedSquare;
    }
};

// Contient les lengths des bateaux
// Est également utilisé pour déterminé un gagnant
vector<int> shipLengths({2,3,3,4,5});

/*
 * Retourne la grandeur du bateau si le bateau peut etre placer (assert en dehors de cette fonction)
 * Si placeBoat est donné, place le bateau sur la grille
 */
int coordToCoord(vector<vector<cell> >&board, char coord1[], char coord2[], int placeBoat = -1)
{
    if(coord1[0] == coord2[0]) // Les lettres sont identiques
    {
        // convertir les valeurs ascii en valeur (0 - 9)
        int c1 = int(coord1[1]) - 48;
        int c2 = int(coord2[1]) - 48;
        if(c1 < 10 && c1 >= 0 && c2 < 10 && c2 >= 0) // assert entre 0 et 9
        {
            int otherCoord = int(coord1[0]) - int('A'); // convertir l'autre coordonné (la lettre) en valeur (0-9)
            for(int i=min(c1,c2); i<=max(c1,c2); ++i)   // itère sur les cell entre les deux coordonnnés
            {
                if(board[i][otherCoord].cellType != 0)
                {
                    return -1;
                }
                if(placeBoat != -1)
                {
                    board[i][otherCoord].cellType = placeBoat;
                }
            }
            return abs(c1 - c2) + 1; //retourne la grandeur inclusive du bateau (par exemple: A0 a A2 a une grandeur de 3)
        }
    }
    else if(coord1[1] == coord2[1]) // Les chiffres sont identiques
    {
        // Convertir les valeurs ascii en valeur (0-9)
        int c1 = int(coord1[0]) - int('A');
        int c2 = int(coord2[0]) - int('A');
        if(c1 < 10 && c1 >= 0 && c2 < 10 && c2 >= 0) // assert valeur entre (0-9)
        {
            int otherCoord = int(coord1[1]) - int('0'); // convertir l'autre coordonné (le chiffre) en valeur (0-9)
            for(int i=min(c1,c2); i<=max(c1,c2); ++i)  //itère sur les cell entre les deux coordonnées
            {
                if(board[otherCoord][i].cellType != 0)
                {
                    return -1;
                }
                if(placeBoat != -1)
                {
                    board[otherCoord][i].cellType = placeBoat;
                }
            }
            return abs(c1 - c2) + 1; //retourne la grandeur inclusive du bateau (par exemple: A0 a A2 a une grandeur de 3)
        }
    }
    return -1; // le bateau ne peut pas etre placé
}

//Affiche le tableau qui contient vos bateaux
void affichage(vector<vector<cell> > &board)
{
    printf("     A   B   C   D   E   F   G   H   I   J \n");
    for(int i = 0; i < 10; i++)
    {
        printf("   -----------------------------------------\n");
        printf("%d  ", (i));
        
        for(int j = 0; j < 10; j++)
        {
            //Insertion des valeurs contenues dans le tableau du user
            //Si la case contient un bateau qui à été frappé, on met un X
            //Sinon, on met 0
            if(board[i][j].hit )
            {
                printf("| X ");
            }
            else
            {
                printf("| %d ", board[i][j].cellType);
            }
        }
        printf("\n");
    }
}

// affiche le tableau qui contient vos essaie de tire
void affichageOtherBoard(vector<vector<cell> > &board)
{
    printf("     A   B   C   D   E   F   G   H   I   J \n");
    for(int i = 0; i < 10; i++)
    {
        printf("   -----------------------------------------\n");
        printf("%d  ", (i));
        
        for(int j = 0; j < 10; j++)
        {
            //Insertion des valeurs contenues dans le tableau du user
            //Si la case contient un bateau qui à été frappé, on met un X
            //Si la case à été frappé, mais il ne contient pas de bateau, on met un M
            //Sinon, on met 0
            if(board[i][j].hit )
            {
                printf("| X ");
            }
            else if(board[i][j].usedSquare)
            {
                printf("| M ");
            }
            else
            {
                printf("| %d ", board[i][j].cellType);
            }
        }
        printf("\n");
    }
}
//Contient le texte représentant un hit ou miss
char hitOrMiss[3];

//Prend une coordonnee et observe si cette position dans le tableau contient un bateau
//Prend comme parametre le tableau, et une coordonnee
//Modifie hitOrMiss pour représenter si la coordonnee est un hit or miss
void processCoordinate(vector<vector<cell> >&board, char coordinate[])
{
    //Convertit la coordonnee a son index colonne/ligne respectif
    int column = int(coordinate[0]) - 65;
    int line   = int(coordinate[1]) - 48;

    // Vérifier si c'est un emplacement valid
    if(!(line >= 0 && line < 10 && column >= 0 && column < 10))
    {
        hitOrMiss[0] = '?';
        hitOrMiss[1] = '\0';
        return;
    }
    //La coordonnee contient un bateau
    if(board[line][column].cellType != 0)
    {
        //La case est donc frappe
        board[line][column].hit = true;
        //On decremente le int de la grandeur du bateau
        shipLengths[board[line][column].cellType - 1] -= 1;
        //Si le bateau qui est frappe a une grandeur de 0, il est coule
        if(shipLengths[board[line][column].cellType - 1] == 0)
        {
            hitOrMiss[0] = 'h';
            hitOrMiss[1] = 's';
            hitOrMiss[2] = '\0';
        }
        else
        {
            hitOrMiss[0] = 'h';
            hitOrMiss[1] = '\0';
        }
        board[line][column].cellType = 0; //Pour que l'on ne puisse pas frapper un cell deux fois   
    }
    else 
    {
        //la coordonnee ne contient pas de bateau
        hitOrMiss[0] = 'm';
        hitOrMiss[1] = '\0';
    }
}

/*
 * Fonction utilisé par l'ordinateur.
 * Place des bateaux aléatoirement sur la grille.
 */
void randomizeBoard(vector<vector<cell> >&board)
{
    for(int i=0; i<shipLengths.size(); ++i)
    {
        bool fits;
        do
        {
            fits = false;
            // Emplacement de début aléatoire
            int x=rand()%10;
            int y=rand()%10;
            int orientation = rand()%2;
            
            char coord1[2] = {char(x+int('A')), char(y+int('0'))};
            
            // Orientation aléatoire
            switch(orientation)
            {
                case 0:
                {
                    //en bas
                    char coord2[2] = {char(x+int('A')), char(y+int('0')+shipLengths[i]-1)};
                    if(coordToCoord(board, coord1, coord2) >= 0)
                    {
                        // place le bateaux de coord1 a coord2
                        coordToCoord(board, coord1, coord2, i+1); // place le avec le flag i+1 (chaque bateau a un différent flag)
                        fits=true;
                    }
                    break;
                }
                case 1:
                {
                    
                    // a droite
                    char coord2[2] = {char(x+int('A')+shipLengths[i]-1), char(y+int('0'))};
                    if(coordToCoord(board, coord1, coord2) >= 0)
                    {
                        // place le bateaux de coord1 a coord2
                        coordToCoord(board, coord1, coord2, i+1); // place le avec le flag i+1 (chaque bateau a un différent flag)
                        fits=true;
                    }
                    break;
                }
            }
        }while(!fits);
    }
}

/*
 * Affichage de frapper / non frappé
 */
void processHitOrMiss(char c[])
{
    if(strcmp(hitOrMiss, "?") == 0)
    {
        cout<<"Coordonnee Invalide"<<endl;
    }
    if(strcmp(hitOrMiss, "m") == 0)
    {
        cout<<"Rate."<<endl;
    }
    else if(strcmp(hitOrMiss, "h") == 0)
    {
        cout<<"Touche."<<endl;
    }
    else if(strcmp(hitOrMiss, "hs") == 0)
    {
        cout<<"Touche Coule!"<<endl;
    }
}

/*
 * Utilisé par l'ordinateur pour choisir une coordonné aléatoirement.
 * L'ordinateur essaie aussis de chainé des touche.
 * Si prevHit contient une valeur, essaie de frapper autour de celui-ci
 */
void randomCoordinate(char (&prevHit)[3])
{
    if(strlen(prevHit) == 2)
    {
        int randomOrientation = rand()%4;
        switch(randomOrientation)
        {
            case 0:
            {
                // haut
                if(prevHit[1]+1 <= 58) // 10
                {
                    prevHit[1]++;
                    break;
                }
            }
            case 1:
            {
                // a droite
                if(prevHit[0]+1 <= 74) // J
                {
                    prevHit[0]++;
                    break;
                }
            }
            case 2:
            {
                // en bas
                if(prevHit[1]-1 >= 48) // 0
                {
                    prevHit[1]--;
                    break;
                }
            }
            case 3:
            {
                // a gauche
                if(prevHit[0]-1 >= 65) // A
                {
                    prevHit[0]--;
                    break;
                }
            }
        }
    }
    else // prevHit ne contient pas une valeur, choisis aléatoirement.
    {
        prevHit[0] = char(rand()%10 + int('A'));
        prevHit[1] = char(rand()%10 + int('0'));
        prevHit[2] = '\0';
    }
}

// Vérifie si le user veut quitter le jeu.
void ifIsKill(char (&c)[3])
{
    if(strcmp(c, "q") == 0)
    {
        exit(0);
    }
}

//Obtient des coordonnées du user, vérifie si ceux-ci sont correctes et puis place les bateaux à ces locations
//Prend comme paramètre le tableau du joueur
void placeUserBoat(vector<vector<cell> > &board)
{
    //Itère pour chaque bateau
    for(int i=0; i<shipLengths.size(); ++i)
    {
        bool fits;
        char coord1[3];
        char coord2[3];
        
        //Itère jusqu'à ce qu'on place le bateau actuel
        do
        {
            //Demande à l'utilisateur pour des coordonnées pour l'insertion du bateau
            fits = false;
            printf("Donner des coordonnees initiales pour le bateau %d (EX: A1):\n", shipLengths[i]);
            cin >> coord1;
            coord1[2] = '\0';
            ifIsKill(coord1);
            printf("\nDonner les coordonnees finales pour placer bateau %d:\n", shipLengths[i]);
            cin >> coord2;
            coord2[2] = '\0';
            ifIsKill(coord2);
            //Si la longueur des coordonnées retournée est la même que la longueur du bateau, on le place
            if(coordToCoord(board, coord1, coord2) == shipLengths[i])
            {
                coordToCoord(board, coord1, coord2, (i + 1));
                fits = true;
                affichage(board);
            }
            //Sinon, on envoie un message d'erreur
            else if(coordToCoord(board, coord1, coord2) == -1)
            {
                printf("\nNe peut pas etre place a ces coordonnees\n");
            }
            else
            {
                printf("\nCoordonnees Invalide\n");
            }
        
        }while(!fits);
    }
}


/*
 * Traitement pour la deuxieme grille (vos tire sur l'adversaire)
 */
void processSentCoordinate(vector<vector<cell> > &otherBoard, char (&c)[3], char (&hm)[3])
{
    //Convertit la coordonnee a son index respectif
    int column = int(c[0]) - 65;
    int line   = int(c[1]) - 48;
    // assert entre (0-9)
    if(!(line >= 0 && line < 10 && column >= 0 && column < 10))
    {
        return;
    }
    if(strcmp(hm, "h") == 0 || strcmp(hm, "hs") == 0)
        otherBoard[line][column].hit = true;
    otherBoard[line][column].usedSquare = true;
}

int main()
{
    srand(time(NULL)); // seed pour rand()
    printf("Appuyer sur la touche 'q' pour quitter le programme.\n");
    // Un processus va lire de son propre pipe et lire de l'autre.
	int p1[2]; //pipe pour le processus 1
	pipe(p1);
    int p2[2]; //pipe pour le processus 2
	pipe(p2);
	
    pid_t ppid = getpid(); // pid du pere
    pid_t child1;
	pid_t child2;
	child1 = fork();
	if(child1 == 0)
	{
        // Dans le processus fils1
        int numberOfSankBoats = 0; // quand cette valeur tape 5, on gagne
        vector<vector<cell> > board(10, vector<cell>(10, cell(0,0)));      // contient nos bateaux
        vector<vector<cell> > otherBoard(10, vector<cell>(10, cell(0,0))); // Contient nos essaie de tire
        randomizeBoard(board); // Remplie la grille de bateaux
        char c[3];  // Se remplie des lectures de l'adversaires
        char prevHit[3]; // contient la coordonné du frappe précédent (s'il a eu lieu)
		read(p1[0], &c, 3); // Lis une coordonné de l'adversaire
        while(true)
        {
            processCoordinate(board, c); // update hitOrMiss pour contenir "m", "h", ou "hs"
            processHitOrMiss(hitOrMiss); // print de la valeur
            write(p2[1], &hitOrMiss, 3); // Envoie frapper ou non vers l'adversaire
            while(strcmp(hitOrMiss, "h") == 0 || strcmp(hitOrMiss, "hs") == 0 || strcmp(hitOrMiss, "?") == 0)
            {
                read(p1[0], &c, 3); // Lis une coordonné de l'adversaire
                processCoordinate(board, c);
                processHitOrMiss(hitOrMiss);
                write(p2[1], &hitOrMiss, 3); // Envoie frapper ou non vers l'adversaire
            }
            randomCoordinate(prevHit); // Reçoie une coordonné de l'ordinateur
            write(p2[1], &prevHit, 3); // Envoie une coordonné
            read(p1[0], &c, 3);    // Lire si frappé ou non
            while(strcmp(c, "h") == 0 || strcmp(c, "hs") == 0)
            {
                if(strcmp(c, "hs") == 0) // compter le nombre de frappé couler
                {
                    numberOfSankBoats++;
                    if(numberOfSankBoats == 5)
                    {
                        printf("L'ordinateur gagne!\n");
                        affichageOtherBoard(otherBoard);
                        affichage(board);
                        exit(0);
                    }
                }
                randomCoordinate(prevHit); // Reçoie une coordonné de l'ordinateur
                write(p2[1], &prevHit, 3);  // Envoie une coordonné
                read(p1[0], &c, 3);     // Lire si frappé ou non
            }
            prevHit[0] = '\0';
            read(p1[0], &c, 3);     // Lis une coordonné de l'adversaire
        }
    }
	else
    {
        // Dans le processus père
		
        child2 = fork();
		if(child2 == 0)
		{
            // Dans le processus fils2
            int numberOfSankBoats = 0;
            vector<vector<cell> > board(10, vector<cell>(10, cell(0,0)));
            vector<vector<cell> > otherBoard(10, vector<cell>(10, cell(0,0)));
            char c[3];
            char hm[3]; //Stocke les valeurs de hit ou miss
            placeUserBoat(board); // Pour placer les bateaux de l'utilisateur
            printf("Choisir une coordonnee a tirer.");
            string userInput;
            cin>>userInput; // Lecture d'un coordonnée à tirer
            strncpy(c, userInput.c_str(), sizeof(c));
            ifIsKill(c); // Vérifie si on veut quitter le programme
            write(p1[1], &c, 3); // écrire la coordonnée dans le tube
            while(true)
            {
                read(p2[0], &hm, 3); // lecture de hit or miss
                processSentCoordinate(otherBoard, c, hm); //Mise à jour de la deuxième grille qui contient les essais de tire
                affichageOtherBoard(otherBoard);
                affichage(board);
                while(strcmp(hm, "h") == 0 || strcmp(hm, "hs") == 0 || strcmp(hm, "?") == 0)
                {
                    if(strcmp(hm, "hs") == 0)
                    {
                        numberOfSankBoats++;
                        if(numberOfSankBoats == 5)
                        {
                            printf("L'utilisateur gagne!\n");
                            affichageOtherBoard(otherBoard);
                            affichage(board);
                            exit(0);
                        }
                    }
                    printf("Choisir une coordonnee a tirer.");
                    cin>>userInput;
                    strncpy(c, userInput.c_str(), sizeof(c));
                    ifIsKill(c);
                    write(p1[1], &c, 3); // écrire la coordonnée dans le tube
                    read(p2[0], &hm, 3); // lecture de hit or miss
                    processSentCoordinate(otherBoard, c, hm);
                    affichageOtherBoard(otherBoard);
                    affichage(board);
                }
                read(p2[0], &c, 3); 
                cout<<endl<<"L'opposant a envoye: "<<c<<endl;
                processCoordinate(board, c); 
                write(p1[1], &hitOrMiss, 3); 
                while(strcmp(hitOrMiss, "h") == 0 || strcmp(hitOrMiss, "hs") == 0)
                {
                    read(p2[0], &c, 3);  
                    cout<<endl<<"L'opposant a envoye: "<<c<<endl;
                    processCoordinate(board, c);
                    write(p1[1], &hitOrMiss, 3);
                }
                affichageOtherBoard(otherBoard);
                affichage(board);
                printf("Choisir une coordonnee a tirer.");
                cin>>userInput;
                strncpy(c, userInput.c_str(), sizeof(c));
                ifIsKill(c);
                write(p1[1], &c, 3);
            }
            return 0;
		}
		else
		{
            // Dans le processus père

            int status = 0;
            wait(&status); // attend pour un processus fils à terminer
            kill(child1, SIGKILL); // tuer le processus child1
            kill(child2, SIGKILL); // tuer le processus child2
		}
    }
	return 0; // Terminer le processus père
}
