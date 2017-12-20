#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <string.h> //strcmp
#include <vector>
#include <cmath> //abs
#include <sys/wait.h>

using namespace std;

struct cell
{
    int cellType;
    bool hit;
    cell(){}
    cell(int cellType, bool hit)
    {
        this->cellType = cellType;
        this->hit = hit;
    }
};

vector<int> shipLengths({2,3,3,4,5});

// return the length of the boat if it fits, else return -1
// if placeBoat is set to a boat label, actually place the boat, if not, only return if you CAN place it there (by returning the length of the boat).
int coordToCoord(vector<vector<cell> >&board, char coord1[], char coord2[], int placeBoat = -1)
{
    if(coord1[0] == coord2[0])
    {
        //letters are the same
        int c1 = int(coord1[1]) - 48 - 1;
        int c2 = int(coord2[1]) - 48 - 1;
        if(c1 < 10 && c1 >= 0 && c2 < 10 && c2 >= 0)
        {
            int otherCoord = int(coord1[0]) - int('A');
            for(int i=min(c1,c2); i<=max(c1,c2); ++i)
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
            return abs(c1 - c2) + 1;
        }
    }
    else if(coord1[1] == coord2[1])
    {
        //digits are the same
        int c1 = int(coord1[0]) - int('A');
        int c2 = int(coord2[0]) - int('A');
        if(c1 < 10 && c1 >= 0 && c2 < 10 && c2 >= 0)
        {
            int otherCoord = int(coord1[1]) - int('0') - 1;
            for(int i=min(c1,c2); i<=max(c1,c2); ++i)
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
            return abs(c1 - c2) + 1;
        }
    }
    return -1;
}

//Affiche le tableau du processus user
void affichage(vector<vector<cell> > &board)
{
    printf("     A   B   C   D   E   F   G   H   I   J \n");
    for(int i = 0; i < 10; i++)
    {
        printf("   -----------------------------------------\n");
        if(i == 9)
        {
            printf("%d ", (i+1));
        }
        else
        {
            printf("%d  ", (i+1));
        }
        
        for(int j = 0; j < 9; j++)
        {
            //Insertion des valeurs contenues dans le tableau du user
            if(board[i][j].hit)
            {
                printf("| x ");
            }
            else
            {
                printf("| %d ", board[i][j].cellType);
            }
        }
        if(board[i][9].hit)
        {
            printf("| x ");
            printf("|\n");
        }
        else
        {
            printf("| %d ", board[i][9].cellType);
            printf("|\n");
        }
    }
}

char hitOrMiss[3];
//Prend une coordonnee et observe si cette position dans le tableau contient un bateau
//Prend comme parametre le tableau, et une coordonnee
//Retourne hit or miss
void processCoordinate(vector<vector<cell> >&board, char coordinate[]) // removed last parameter
{
    //Convertit la coordonnee a son index colonne respectif
    int column = int(coordinate[0]) - 65;
    int line   = int(coordinate[1]) - 48 - 1;

    // Check if valid choice
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
        board[line][column].cellType = 0; //so we cant hit here again    
    }
    else 
    {
        //la coordonnee ne contient pas de bateau
        hitOrMiss[0] = 'm';
        hitOrMiss[1] = '\0';
    }
}

void randomizeBoard(vector<vector<cell> >&board)
{
    for(int i=0; i<shipLengths.size(); ++i)
    {
        bool fits;
        do
        {
            fits = false;
            int x=rand()%10;
            int y=rand()%10;
            int orientation = rand()%2;
            
            char coord1[2] = {char(x+int('A')), char(y+int('0'))};
            
            switch(orientation)
            {
                case 0:
                {
                    //down
                    char coord2[2] = {char(x+int('A')), char(y+int('0')+shipLengths[i])};
                    if(coordToCoord(board, coord1, coord2) >= 0)
                    {
                        //place the ship from coord1 to coord2
                        coordToCoord(board, coord1, coord2, i+1); // with the boatNum flag, it places the boat
                        fits=true;
                    }
                    break;
                }
                case 1:
                {
                    
                    //right
                    char coord2[2] = {char(x+int('A')+shipLengths[i]), char(y+int('0'))};
                    if(coordToCoord(board, coord1, coord2) >= 0)
                    {
                        //place the ship from coord1 to coord2
                        coordToCoord(board, coord1, coord2, i+1); // with the boatNum flag, it places the boat
                        fits=true;
                    }
                    break;
                }
            }
        }while(!fits);
    }
}

void processHitOrMiss(char c[])
{
    if(strcmp(hitOrMiss, "?") == 0)
    {
        cout<<"Not a valid coordinate."<<endl;
    }
    if(strcmp(hitOrMiss, "m") == 0)
    {
        cout<<"miss."<<endl;
    }
    else if(strcmp(hitOrMiss, "h") == 0)
    {
        cout<<"hit."<<endl;
    }
    else if(strcmp(hitOrMiss, "hs") == 0)
    {
        cout<<"Hit Sink!"<<endl;
    }
}

void randomCoordinate(char (&prevHit)[3])
{
    if(strlen(prevHit) == 2)
    {
        int randomOrientation = rand()%4;
        switch(randomOrientation)
        {
            case 0:
            {
                //up
                if(prevHit[1]+1 <= 58) // 10
                {
                    prevHit[1]++;
                    break;
                }
            }
            case 1:
            {
                //right
                if(prevHit[0]+1 <= 74) // J
                {
                    prevHit[0]++;
                    break;
                }
            }
            case 2:
            {
                //down
                if(prevHit[1]-1 >= 48) // 0
                {
                    prevHit[1]--;
                    break;
                }
            }
            case 3:
            {
                //left
                if(prevHit[0]-1 >= 65) // A
                {
                    prevHit[0]--;
                    break;
                }
            }
        }
    }
    else
    {
        prevHit[0] = char(rand()%10 + int('A'));
        prevHit[1] = char(rand()%10 + int('1'));
        prevHit[2] = '\0';
    }
}


void placeUserBoat(vector<vector<cell> > &board)
{
    for(int i=0; i<shipLengths.size(); ++i)
    {
        bool fits;
        char coord1[3];
        char coord2[3];
        
        do
        {
            fits = false;
            printf("Give initial coordinates to place the boat of length %d (EX: A1):\n", shipLengths[i]);
            cin >> coord1;
            coord1[2] = '\0';
            printf("\nGive the final coordinates to place the boat %d:\n", shipLengths[i]);
            cin >> coord2;
            coord2[2] = '\0';
            
            if(coordToCoord(board, coord1, coord2) == shipLengths[i])
            {
                coordToCoord(board, coord1, coord2, (i + 1));
                fits = true;
                affichage(board);
            }
            else if(coordToCoord(board, coord1, coord2) == -1)
            {
                printf("\nDoes not fit, try new coordinates\n");
            }
            else
            {
                printf("\nInvalid coordinates\n");
            }
        
        }while(!fits);
    }
}

int main()
{
    srand(time(NULL));
    //processus pere
    
    // a process will read from its own pipe, write to the other
	int p1[2]; //pipe pour le processus 1
	pipe(p1);
    int p2[2]; //pipe pour le processus 2
	pipe(p2);
	
    pid_t child1;
	pid_t child2;
	child1 = fork();
	if(child1 == 0)
	{
        int numberOfSankBoats = 0;
        bool gameNotDone = true;
        // in child1 process
        vector<vector<cell> > board(10, vector<cell>(10, cell(0,0)));
        randomizeBoard(board);
        //affichage(board);
        char c[3];
        char prevHit[3];
		read(p1[0], &c, 3); // read coordinate
        // TODO: maybe add half a turn to randomize who starts
        while(gameNotDone)
        {
            processCoordinate(board, c); // this updates hitOrMiss to contain "m", "h", or "hs"
            processHitOrMiss(hitOrMiss);
            write(p2[1], &hitOrMiss, 3); // write hit or miss
            while(strcmp(hitOrMiss, "h") == 0 || strcmp(hitOrMiss, "hs") == 0 || strcmp(hitOrMiss, "?") == 0)
            {
                read(p1[0], &c, 3); // read a coordinate
                processCoordinate(board, c);
                processHitOrMiss(hitOrMiss);
                write(p2[1], &hitOrMiss, 3); //send hit or miss
            }
            // TODO: get computer input
            randomCoordinate(prevHit);
            write(p2[1], &prevHit, 3); //send a coordinate
            read(p1[0], &c, 3);    //read hit or miss
            while(strcmp(c, "h") == 0 || strcmp(c, "hs") == 0)
            {
                if(strcmp(c, "hs") == 0)
                {
                    numberOfSankBoats++;
                    if(numberOfSankBoats == 5)
                    {
                        printf("Computer wins!\n");
                        //TODO: endGame();
                    }
                }
                randomCoordinate(prevHit); // get computer input
                write(p2[1], &prevHit, 3);  // write a coordinate
                read(p1[0], &c, 3);     // read hit or miss
            }
            prevHit[0] = '\0';
    
            read(p1[0], &c, 3);     // read coordinate
        }
		return 0; //end the process at the end
    }
	else
    {
        // in parent process
		//cout<<"Created child1 process: "<<child1<<endl;
		
        child2 = fork();
		if(child2 == 0)
		{
            int numberOfSankBoats = 0;
            bool gameNotDone = true;
            vector<vector<cell> > board(10, vector<cell>(10, cell(0,0)));
            char c[3];
            placeUserBoat(board);
            //randomizeBoard(board); //TODO: remove this placeholder and let the player actually pick coords
            //TODO: maybe add half of the thing to randomize who starts
            //affichageEnemyBoard();
            printf("Choose a coordinate to shoot at.");
            string userInput;
            cin>>userInput;
            strncpy(c, userInput.c_str(), sizeof(c));
            write(p1[1], &c, 3); // write coordinate
            while(gameNotDone)
            {
                read(p2[0], &c, 3); // read hit or miss
                while(strcmp(c, "h") == 0 || strcmp(c, "hs") == 0 || strcmp(c, "?") == 0)
                {
                    if(strcmp(c, "hs") == 0)
                    {
                        numberOfSankBoats++;
                        if(numberOfSankBoats == 5)
                        {
                            printf("Player wins!\n");
                            //TODO: endGame();
                        }
                    }
                    printf("Choose a coordinate to shoot at.");
                    cin>>userInput;
                    strncpy(c, userInput.c_str(), sizeof(c));
                    write(p1[1], &c, 3); // write coordinate
                    read(p2[0], &c, 3); //read hit or miss
                }
                read(p2[0], &c, 3); // read coordinate
                cout<<endl<<"other guy sent: "<<c<<endl;
                processCoordinate(board, c); // Gives hitOrMiss it's value
                write(p1[1], &hitOrMiss, 3); // write hit or miss
                while(strcmp(hitOrMiss, "h") == 0 || strcmp(hitOrMiss, "hs") == 0)
                {
                    read(p2[0], &c, 3);  // read coordinate
                    cout<<endl<<"other guy sent: "<<c<<endl;
                    affichage(board);
                    processCoordinate(board, c); // Gives hitOrMiss it's value
                    write(p1[1], &hitOrMiss, 3); // write hit or miss
                }
                affichage(board);
                printf("Choose a coordinate to shoot at.");
                cin>>userInput;
                strncpy(c, userInput.c_str(), sizeof(c));
                write(p1[1], &c, 3);     // write coordinate
            }
            printf("2 done");
            return 0;
		}
		else
		{
            // in parent process
            pid_t wpid;
            int status = 0;
            while ((wpid = wait(&status)) > 0); // wait for all child processes to end
			//cout<<"Created child2 process: "<<child2<<endl;
		}
    }
	return 0;
}
