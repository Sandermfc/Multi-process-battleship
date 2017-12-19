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
        int c1 = int(coord1[1]) - int('0');
        int c2 = int(coord2[1]) - int('0');
        if(c1 < 10 && c1 >= 0 && c2 < 10 && c2 >= 0)
        {
            int otherCoord = int(coord1[0]) - int('A');
            for(int i=min(c1,c2); i<max(c1,c2); ++i)
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
            return abs(c1 - c2);
        }
    }
    else if(coord1[1] == coord2[1])
    {
        //digits are the same
        int c1 = int(coord1[0]) - int('A');
        int c2 = int(coord2[0]) - int('A');
        if(c1 < 10 && c1 >= 0 && c2 < 10 && c2 >= 0)
        {
            int otherCoord = int(coord1[1]) - int('0');
            for(int i=min(c1,c2); i<max(c1,c2); ++i)
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
            return abs(c1 - c2);
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
            printf("| %d ", board[i][j].cellType);
        }
        printf("| %d ", board[i][9].cellType);
        printf("|\n");
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
            return;
        }
        else
        {
            hitOrMiss[0] = 'h';
            hitOrMiss[1] = '\0';
            return;
        }
        board[line][column].cellType = 0; //so we cant hit here again    
    }
    else 
    {
        //la coordonnee ne contient pas de bateau
        hitOrMiss[0] = 'm';
        hitOrMiss[1] = '\0';
        return;
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
        bool gameNotDone = true;
        // in child1 process
        vector<vector<cell> > board(10, vector<cell>(10, cell(0,0)));
        randomizeBoard(board);
        affichage(board);
        // TODO: setup board
        char c[3];
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
                // TODO:process coordinate on our board
                // TODO:c = h or m or hs
                processCoordinate(board, c);
                processHitOrMiss(hitOrMiss);
                write(p2[1], &hitOrMiss, 3); //send hit or miss
            }
            // TODO: get user input (coordinate)
            write(p2[1], "A4", 3); //send a coordinate
            
            read(p1[0], &c, 3);    //read hit or miss

            while(strcmp(c, "h") == 0 || strcmp(c, "hs") == 0)
            {
                // TODO: get user input (coordinate to send)
                write(p2[1], "B2", 3);  // write a coordinate
                read(p1[0], &c, 3);     // read hit or miss
            }
            
            read(p1[0], &c, 3);     // read coordinate
        }
        printf("done1");
		return 0; //end the process at the end
    }
	else
    {
        // in parent process
		//cout<<"Created child1 process: "<<child1<<endl;
		
        child2 = fork();
		if(child2 == 0)
		{
            bool gameNotDone = true;
            vector<vector<cell> > board(10, vector<cell>(10, cell(0,0)));
            char c[3];
            randomizeBoard(board); //TODO: remove this placeholder and let the player actually pick coords
            //TODO: maybe add half of the thing to randomize who starts
            //affichage(board);
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
                    processCoordinate(board, c); // Gives hitOrMiss it's value
                    write(p1[1], &hitOrMiss, 3); // write hit or miss
                }
                
                //TODO: figure out what coordinate to send
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
