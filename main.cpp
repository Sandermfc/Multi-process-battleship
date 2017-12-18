#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>

using namespace std;

struct cell
{
    int cellType;
    bool hit;
    cell(){};
    cell(int cellType, bool hit)
    {
        this->cellType = cellType;
        this->hit = hit;
    }
};

vector<int> shipLengths({2,3,3,4,5});

void randomizeBoard(cell (&board)[10][10])
{

    for(int i=0; i<shipLengths.size(); ++i)
    {
        bool fits;
        do
        {
            fits = false;
            int x=rand()%10;
            int y=rand()%10;
            int orientation = rand()%4;
            switch(orientation)
            {
                case 0:
                    //up
                    
                    break;
                case 1:
                    //right
                    break;
                case 2:
                    //down
                    break;
                case 3:
                    //left
                    break;
                default:
                    cout<<"wut how are u here"<<endl;
            }
            cout<<x<<" "<<y<<" "<<orientation<<endl;
        }while(fits);
        
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
        cell board[10][10];
        randomizeBoard(board);
        // TODO: setup board
        
		char c[2];
		read(p1[0], &c, 2); // read coordinate
        
        // TODO: maybe add half a turn to randomize who starts
        printf(c);
        while(gameNotDone)
        {
            // TODO:process coordinate on our board
            // TODO:c = h or m or hs
            write(p2[1], "hs", 2); // write hit or miss
            while(c == "h" || c == "hs")
            {
                read(p1[0], &c, 2); // read a coordinate
                // TODO:process coordinate on our board
                // TODO:c = h or m or hs
                write(p2[1], "hs", 2); //send hit or miss
            }
            // TODO: get user input (coordinate)
            write(p2[1], "A4", 2); //send a coordinate
            
            read(p1[0], &c, 2);    //read hit or miss
            while(c == "h" || c == "hs")
            {
                // TODO: get user input (coordinate to send)
                write(p2[1], "B2", 2);  // write a coordinate
                read(p1[0], &c, 2);     // read hit or miss
            }
            
            printf(c);
            read(p1[0], &c, 2);     // read coordinate
            printf(c);
            break;
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
            int board[10][10] = {0};
            char c[2];
            //cout<<"in child2"<<endl;
            //TODO: maybe add half of the shit to randomize who starts
            //TODO: computer picks a coordinate
            
			write(p1[1], "B7", 2); // write coordinate
            
            while(gameNotDone)
            {
                read(p2[0], &c, 2); // read hit or miss
                while(c == "h" || c == "hs")
                {
                    // TODO: process board, pick new coordinate to send
                    write(p1[1], &c, 2); // write coordinate
                    read(p2[0], &c, 2); //read hit or miss
                }
                    
                read(p2[0], &c, 2); // read coordinate
                // TODO: process coordinate
                // TODO: c = hit or miss or wtv
                write(p1[1], "h", 2);    //write hit or miss
                while(c == "h" || c == "hs")
                {
                    read(p2[0], &c, 2);  // read coordinate
                    //TODO: process coordinate
                    write(p1[1], "h", 2);  // write hit or miss
                }
                
                printf(c);
                //TODO: figure out what coordinate to send
                write(p1[1], "B2", 2);     // write coordinate
                printf(c);
                break;
            }
            printf("2 done");
            return 0;
		}
		else
		{
			// in parent process
			//cout<<"Created child2 process: "<<child2<<endl;
		}
    }
	return 0;
}
