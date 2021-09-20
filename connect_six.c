/******************************************************************************
* Author: Chanaka Ganewattha
* Date: 2013
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* I BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
******************************************************************************/

short board[19][19]={
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                           {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                         };


const short openStone=2,myStone=1,opponentStone=3;


struct weightAndCoordinates
{
        short x,y;
        double weight;
        short boardScore;
}cellSpec[10],candidate,temp;

struct moves
{
        short x,y;
}aiMoves[2],opponentMoves[2],tempMoves[2];

//this variable is used to limit how much moves we are testing to find the move with the best chances.
const short weightLim=5;
short weightGenerator(short checkingColor,short otherColor);


short dirLib;
const short openCellWeight=2;
int myStoneWeight[5]={4096,2048,1024,512,256};
const double degreeWeight[5]={0.99999818138,1.0,1.00000181862,1.00000363725,1.00000726562};

double hmoveEvaluation(short checkingColor,short otherColor,short x,short y);
double evaluationDirectional(short checkingColor,short otherColor,short x,short y,short dx,short dy);

void printBoard();

//functions in threat counter
short threatCounter(short board[19][19],short checkingColor,short otherColor);
short cellSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy);


//functions in boardScoring.c class file
short boardScoringFunc(short x,short y,short board[19][19],short checkingColor,short otherColor);
short foursSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy);
short threesSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy);
short twoesSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy);

//offensive moves generator function
short offensiveMoveGenerator(short checkingColor,short otherColor);
void placeStones();

short turns;

//game over functions
void gameWon();
short winMarker(short x,short y,short dx,short dy);

short threatDetector(short checkingColor,short otherColor);
short threats(short x,short y,short checkingColor,short otherColor,short dx,short dy);

void threatBlock(short dx,short dy,short x,short y);
void immediateWinBlocker();

void firstMove();

//functions relating to UART Communication
void writeAIMoves();
void readAndPlaceStones();

const short boardScoreBound=7;

short flag=0;

void main()
{
        short myWeight;
        short opponentWeight;

        //to store my cells with maximum advantage and opponent cells with max advantage to the opponent
        struct moves tmpMyMoves[2],tmpOppMoves[2];
        
        UART1_Init(4800);
        Delay_ms(200);

        //writeAIMoves();
        /*
        boardScoringFunc(9,9,board,3,1);
        weightGenerator(3,1);
        offensiveMoveGenerator(myStone,opponentStone);
        offensiveMoveGenerator(myStone,opponentStone);
        */

        firstMove();

        while(1)
        {
                turns=0;
                flag=0;
                readAndplaceStones();

                if(threatDetector(myStone,opponentStone))
                gameWon();

                if(threatDetector(opponentStone,myStone))
                immediateWinBlocker();

                //offensive moves and weights
                myWeight=offensiveMoveGenerator(myStone,opponentStone);
                tmpMyMoves[0].x=tempMoves[0].x;tmpMyMoves[0].y=tempMoves[0].y;//first move
                tmpMyMoves[1].x=tempMoves[1].x;tmpMyMoves[1].y=tempMoves[1].y;//second move
                
                //start commenting from here up to the exit of the preceding if else condition to play totally offensive
                //defensive moves and weights
                opponentWeight=offensiveMoveGenerator(opponentStone,myStone);
                tmpOppMoves[0].x=tempMoves[0].x;tmpOppMoves[0].y=tempMoves[0].y;//first move
                tmpOppMoves[1].x=tempMoves[1].x;tmpOppMoves[1].y=tempMoves[1].y;//second move

                //if(myWeight>opponentWeight||myWeight>32)
                if(myWeight>opponentWeight)
                {
                        //first move
                        if(turns<2)
                        {
                                board[tmpMyMoves[0].x][tmpMyMoves[0].y]=myStone;

                                //to store the move in the aiMoves structure
                                aiMoves[turns].x=tmpMyMoves[0].x;
                                aiMoves[turns].y=tmpMyMoves[0].y;

                                turns++;
                        }

                        //second move
                        if(turns<2)
                        {
                                board[tmpMyMoves[1].x][tmpMyMoves[1].y]=myStone;

                                //to store the move in the aiMoves structure
                                aiMoves[turns].x=tmpMyMoves[1].x;
                                aiMoves[turns].y=tmpMyMoves[1].y;

                                turns++;
                        }
                }
                else
                {
                        //first move
                        if(turns<2)
                        {
                                board[tmpOppMoves[0].x][tmpOppMoves[0].y]=myStone;

                                //to store the move in the aiMoves structure
                                aiMoves[turns].x=tmpOppMoves[0].x;
                                aiMoves[turns].y=tmpOppMoves[0].y;

                                turns++;
                        }

                        //second move
                        if(turns<2)
                        {
                                board[tmpOppMoves[1].x][tmpOppMoves[1].y]=myStone;

                                //to store the move in the aiMoves structure
                                aiMoves[turns].x=tmpOppMoves[1].x;
                                aiMoves[turns].y=tmpOppMoves[1].y;

                                turns++;
                        }
                }

                
                if(turns<2)
                {
                        board[tempMoves[0].x][tempMoves[0].y]=myStone;

                        //to store the move in the aiMoves structure
                        aiMoves[turns].x=tempMoves[0].x;
                        aiMoves[turns].y=tempMoves[0].y;

                        turns++;
                }

                if(turns<2)
                {
                        board[tempMoves[1].x][tempMoves[1].y]=myStone;

                        //to store the move in the aiMoves structure
                        aiMoves[turns].x=tempMoves[1].x;
                        aiMoves[turns].y=tempMoves[1].y;

                        turns++;
                }
                
                
                //printf("\nThe Moves played by the A.I.\n%c %i\t%c %i",aiMoves[0].y+'a',aiMoves[0].x+1,aiMoves[1].y+'a',aiMoves[1].x+1);

                writeAIMoves();
                
        }
}

//this function dynamically makes a list with highest weights and arrange them in descending order.
short weightGenerator(short checkingColor,short otherColor)
{
        short i,j,k;
        short noOfCells=0;

        //to resetting the struct array
        struct weightAndCoordinates temp;
        temp.x=0;
        temp.y=0;
        temp.boardScore=0;
        temp.weight=0;

        //resetting the cell weights in cell spec struct
        for(i=0;i<weightLim;i++)
        //cellSpec[i].weight=0;
        cellSpec[i]=temp;

        for(i=0;i<19;i++)
        {
                for(j=0;j<19;j++)
                {
                        if(board[i][j]!=openStone)
                        continue;

                        candidate.weight=hmoveEvaluation(checkingColor,otherColor,i,j);
                        candidate.x=i;
                        candidate.y=j;

                        for(k=0;k<weightLim;k++)
                        {
                                if(candidate.weight<cellSpec[k].weight)
                                continue;

                                temp=cellSpec[k];
                                cellSpec[k]=candidate;
                                candidate=temp;

                                noOfCells++;
                        }
                }
        }

        return noOfCells;
}



//checking color is the color the hmoveEvaluation score is given for, that is
//the total cell weight at a point is given for.
double hmoveEvaluation(short checkingColor,short otherColor,short x,short y)
{
        double eval=0;
        dirLib=4;


        //eval=horizontalDirectional+verticalDirectional+diagonalDirectional45+diagonalDirectional135
        eval=evaluationDirectional(checkingColor,otherColor,x,y,0,1)+evaluationDirectional(checkingColor,otherColor,x,y,1,0)+evaluationDirectional(checkingColor,otherColor,x,y,1,1)+evaluationDirectional(checkingColor,otherColor,x,y,-1,1);

        return degreeWeight[dirLib]*eval;
}
//this function is called by hmoveEvaluation()
//checkingColor is the color the evaluationDirectional value is given for
double evaluationDirectional(short checkingColor,short otherColor,short x,short y,short dx,short dy)
{
        short k;
        //adding count
        short count=0;

        double evalDirectional=1;
        
        //to check for directional liberty
        for(k=-5;k<6;k++)
        {
                if((x+k*dx>18||y+k*dy>18||x+k*dx<0||y+k*dy<0||(board[x+k*dx][y+k*dy]==otherColor)))
                {

                        dirLib--;
                        break;

                }

        }


        for(k=1;k<6;k++)//for right/up
        {
                //01,10,11,1-1
                if(x+k*dx<0||y+k*dy<0||x+k*dx>18||y+k*dy>18||board[x+k*dx][y+k*dy]==otherColor)//inserted new x+k*dx<0||y+k*dy<0||
                {
                        break;
                }

                if(board[x+k*dx][y+k*dy]==openStone)
                evalDirectional*=openCellWeight;

                else if(board[x+k*dx][y+k*dy]==checkingColor)
                evalDirectional*=myStoneWeight[k-1];

                count++;
        }

        for(k=1;k<6;k++)//for left/down
        {
                if(x-k*dx<0||y-k*dy<0||x-k*dx>18||y-k*dy>18||board[x-k*dx][y-k*dy]==otherColor)//edited inserted new x-k*dx>18||y-k*dy>18
                {
                        break;
                }

                if(board[x-k*dx][y-k*dy]==openStone)
                evalDirectional*=openCellWeight;

                else if(board[x-k*dx][y-k*dy]==checkingColor)
                evalDirectional*=myStoneWeight[k-1];

                count++;
        }

        //if the direction is not free, skip the cell weight addition
        if(count>=5)
        return evalDirectional;

        return 1;
}




//this functions generates the best move to be played
short offensiveMoveGenerator(short checkingColor,short otherColor)
{
        short i,j;
        short cellSpecArrayLimit=0;
        short noOfThreats=0;

        short max=-1,val;

        //resetting flag
        flag=0;

        cellSpecArrayLimit=weightLim;
        weightGenerator(checkingColor,otherColor);

        //place the first move on the board with the highes weight value
        board[cellSpec[0].x][cellSpec[0].y]=checkingColor;

        cellSpec[0].boardScore=boardScoringFunc(cellSpec[0].x,cellSpec[0].y,board,checkingColor,otherColor);

        for(i=1;i<cellSpecArrayLimit;i++)
        {
                board[cellSpec[i].x][cellSpec[i].y]=checkingColor;
                cellSpec[i].boardScore=boardScoringFunc(cellSpec[i].x,cellSpec[i].y,board,checkingColor,otherColor);
                board[cellSpec[i].x][cellSpec[i].y]=openStone;

                /*
                if(flag==1)
                {
                       //remove the first move placed on the board
                       board[cellSpec[0].x][cellSpec[0].y]=openStone;
                
                       //to store the first move
                       tempMoves[0].x=cellSpec[0].x;
                       tempMoves[0].y=cellSpec[0].y;

                       //store the second move and its weight before checking the cells with highest weight value
                       max=cellSpec[1].boardScore;
                       tempMoves[1].x=cellSpec[i].x;
                       tempMoves[1].y=cellSpec[i].y;
                       
                       return 1;
                 }
                 */
        }

        //remove the first stone placed on the board
        board[cellSpec[0].x][cellSpec[0].y]=openStone;

        //to store the first move
        tempMoves[0].x=cellSpec[0].x;
        tempMoves[0].y=cellSpec[0].y;

        //store the second move and its weight before checking the cells with highest weight value
        max=cellSpec[1].boardScore;
        tempMoves[1].x=cellSpec[1].x;
        tempMoves[1].y=cellSpec[1].y;



        //to find the best move with the best weight value
        for(i=2;i<cellSpecArrayLimit;i++)
        {
                val=cellSpec[i].boardScore;

                if(max<val)
                {
                        max=val;
                        tempMoves[1].x=cellSpec[i].x;
                        tempMoves[1].y=cellSpec[i].y;

                }
        }

        return max;
}





void firstMove()
{
        char c;

        //UART1_Write_Text("Enter the color you want to play");
        //UART1_Write_Text("\n\n\tPress W for white or B for black \n");
scan:   while(!UART1_Data_Ready());
        c=UART1_Read();
        

        if(c=='b'||c=='B')
        {
                short n=0;
                short firstCoordinates[4];

                while(n<4)
                {
                       while(!UART1_Data_Ready());
                       firstCoordinates[n++]=UART1_Read();
                }
                //read and store the stones played by the referee
                aiMoves[0].x=(firstCoordinates[0]-48)*10+firstCoordinates[1]-49;
                aiMoves[0].y=(firstCoordinates[2]-48)*10+firstCoordinates[3]-49;

                board[aiMoves[0].x][aiMoves[0].y]=myStone;

                readAndPlaceStones();

                //make a tactically advantageous pattern taking first player advantage
                offensiveMoveGenerator(myStone,opponentStone);
                board[tempMoves[0].x][tempMoves[0].y]=myStone;
                board[tempMoves[1].x][tempMoves[1].y]=myStone;
                
                //to store the move in the aiMoves structure
                aiMoves[0].x=tempMoves[0].x;
                aiMoves[0].y=tempMoves[0].y;
                //to store the second move
                aiMoves[1].x=tempMoves[1].x;
                aiMoves[1].y=tempMoves[1].y;

                writeAIMoves();

        }

        else if(c=='w'||c=='W')
        {
                char tmp[4];
                short n=0;

                //UART1_Write_Text(" AI is playing white ");

                //to input the first opponent stone
                while(n<4)
                {
                          while(!UART1_Data_Ready());
                          tmp[n++]=UART1_Read();
                 }


                opponentMoves[0].x=(tmp[0]-48)*10+tmp[1]-49,opponentMoves[0].y=(tmp[2]-48)*10+tmp[3]-49;
                opponentMoves[1].x=opponentMoves[0].y;
                opponentMoves[1].x=opponentMoves[0].y;
                //placing the opponentStone on the board

                board[opponentMoves[0].x][opponentMoves[0].y]=opponentStone;

                //prshortf(" $$$$ %i %i ###  %i %i\n",opponentMoves[0].x,opponentMoves[0].y,opponentMoves[1].x,opponentMoves[1].y);

                offensiveMoveGenerator(opponentStone,myStone);
                board[tempMoves[0].x][tempMoves[0].y]=myStone;
                board[tempMoves[1].x][tempMoves[1].y]=myStone;

                //to store the move in the aiMoves structure
                aiMoves[0].x=tempMoves[0].x;
                aiMoves[0].y=tempMoves[0].y;
                //to store the second move
                aiMoves[1].x=tempMoves[1].x;
                aiMoves[1].y=tempMoves[1].y;
                
                //printf("\nThe Moves played by the A.I.\n%c %i\t%c %i",aiMoves[0].y+'a',aiMoves[0].x+1,aiMoves[1].y+'a',aiMoves[1].x+1);
                writeAIMoves();
                //printBoard();
        }
}



//this function takes the win
void gameWon()
{
        short x,y;
        for(x=0;x<19;x++)
        for(y=0;y<19;y++)
        if(winMarker(x,y,0,1)==1||winMarker(x,y,1,0)==1||winMarker(x,y,1,1)==1||winMarker(x,y,-1,1)==1)
        return;

}

short winMarker(short x,short y,short dx,short dy)
{
        short k,n=0;

        for(k=0;k<6;k++)
        {
                if(board[x+k*dx][y+k*dy]==opponentStone||x+k*dx>18||y+k*dy>18||x+k*dx<0||y+k*dy<0)
                return 0;

                if(board[x+k*dx][y+k*dy]==myStone)
                n++;
                
                if(board[x+k*dx][y+k*dy]==openStone)
                {
                      aiMoves[1].x=x+k*dx;
                      aiMoves[1].y=y+k*dy;
                }
        }

        if(k==6&&n>=4)
        {
                for(k=0;k<6;k++)
                {
                        if(board[x+k*dx][y+k*dy]==openStone)
                        {
                                board[x+k*dx][y+k*dy]=myStone;
                                aiMoves[turns].x=x+k*dx;
                                aiMoves[turns].y=y+k*dy;
                                turns++;
                        }
                }
                writeAIMoves();
                return 1;
        }
        return 0;
}


//the function which blocks opponents immediate winnings
void immediateWinBlocker()
{
        short x,y;

        //horizonatal direction
        for(x=0;x<19;x++)
        {
                for(y=0;y<19;y++)
                {
                        if(turns>1)
                        return;

                        threatBlock(0,1,x,y);
                }
        }

        //vertical direction
        for(x=0;x<19;x++)
        {
                for(y=0;y<19;y++)
                {
                        if(turns>1)
                        return;

                        threatBlock(1,0,x,y);
                }
        }

        //45 diagonal direction
        for(x=0;x<19;x++)
        {
                for(y=0;y<19;y++)
                {
                        if(turns>1)
                        return;

                        threatBlock(1,1,x,y);
                }
        }

        //135 diagonal direction
        //vertical direction
        for(x=0;x<19;x++)
        {
                for(y=0;y<19;y++)
                {
                        if(turns>1)
                        return;

                        threatBlock(-1,1,x,y);
                }
        }
}


void threatBlock(short dx,short dy,short x,short y)
{
        short k;
        short count=0;
        double val,max=0;

        for(k=0;k<6;k++)
        {
                if(x+k*dx>18||x+k*dx<0||y+k*dy>18||y+k*dy<0||board[x+k*dx][y+k*dy]==myStone)
                {
                        return;
                }

                if(board[x+k*dx][y+k*dy]==opponentStone)
                count++;
        }

        if(count>=4)
        {
                for(k=0;k<6;k++)
                {
                        if(board[x+k*dx][y+k*dy]!=openStone)
                        continue;
                        
                        val=hmoveEvaluation(opponentStone,myStone,x+k*dx,y+k*dy);

                        if(max<val)
                        {
                                max=val;
                                aiMoves[turns].x=x+k*dx;
                                aiMoves[turns].y=y+k*dy;

                        }
                }

                board[aiMoves[turns].x][aiMoves[turns].y]=myStone;
                turns++;

                return;
        }
        return;
}


//four threats scoring system
//this function gives an idea of how much advantageous the board is for the stone considered by the score
short boardScoringFunc(short x,short y,short board[19][19],short checkingColor,short otherColor)
{
        short i,j,foursCnt=0;

        short totalScore=0;

        short tempBoard[19][19];
        for(i=0;i<19;i++)
        for(j=0;j<19;j++)
        tempBoard[i][j]=board[i][j];

        //clearing status flag
        flag=0;

        //the scoring is done for maximum possible free cells (here 49 free cells max) due to time constraint

        //for fours
        for(i=-boardScoreBound;i<=boardScoreBound;i++)
        {
                for(j=-boardScoreBound;j<=boardScoreBound;j++)
                {
                        if(foursSearch(tempBoard,checkingColor,otherColor,x+i,y+j,0,1)==1)
                        totalScore+=11;//original 9
                        if(foursSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,0)==1)
                        totalScore+=11;
                        if(foursSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,1)==1)
                        totalScore+=11;
                        if(foursSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,-1)==1)
                        totalScore+=11;
                }

        }

        //flag is used to play the moves which will make a immediate win in the next round.
        // that is to extend two live threes or one live three + one dead three combination pattern
        if(totalScore>32)
        {
               flag=1;
        }

        //for threes
        for(i=-boardScoreBound;i<=boardScoreBound;i++)
        {
                for(j=-boardScoreBound;j<=boardScoreBound;j++)
                {
                        if(threesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,0,1)==1)
                        totalScore+=7;
                        if(threesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,0)==1)
                        totalScore+=7;
                        if(threesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,1)==1)
                        totalScore+=7;
                        if(threesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,-1)==1)
                        totalScore+=7;
                }

        }

        //for twoes
        for(i=-boardScoreBound;i<=boardScoreBound;i++)
        {
                for(j=-boardScoreBound;j<=boardScoreBound;j++)
                {
                        if(twoesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,0,1)==1)
                        totalScore+=3;//original 5
                        if(twoesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,0)==1)
                        totalScore+=3;
                        if(twoesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,1)==1)
                        totalScore+=3;
                        if(twoesSearch(tempBoard,checkingColor,otherColor,x+i,y+j,1,-1)==1)
                        totalScore+=3;
                }

        }

        return totalScore;
}



//************************************************************************************************************
short foursSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy)
{
        short k,count=0;
        short threatCell=0;

        for(k=0;k<6;k++)
        {
                if(tempBoard[x+k*dx][y+k*dy]==otherColor||x+k*dx>18||y+k*dy>18||x+k*dx<0||y+k*dy<0)
                return 0;

                if(tempBoard[x+k*dx][y+k*dy]==checkingColor)
                count++;

                if(tempBoard[x+k*dx][y+k*dy]==openStone)
                threatCell=k;
        }

        if(k==6&&count>=4)
        {
                tempBoard[x+threatCell*dx][y+threatCell*dy]=otherColor;

                return 1;
        }

        return 0;
}



//************************************************************************************************************


//three threats scoring system
short threesSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy)
{
        short k,count=0;
        short threatCell=0;

        for(k=0;k<6;k++)
        {
                if(tempBoard[x+k*dx][y+k*dy]==otherColor||x+k*dx>18||y+k*dy>18||x+k*dx<0||y+k*dy<0)
                return 0;

                if(tempBoard[x+k*dx][y+k*dy]==checkingColor)
                count++;

                if(tempBoard[x+k*dx][y+k*dy]==openStone)
                threatCell=k;
        }


        if(k==6&&count==3)
        {
                tempBoard[x+threatCell*dx][y+threatCell*dy]=otherColor;

                return 1;
        }

        return 0;
}

//two threats scoring system
short twoesSearch(short tempBoard[19][19],short checkingColor,short otherColor,short x,short y,short dx,short dy)
{
        short k,count=0;
        short threatCell=0;

        for(k=0;k<6;k++)
        {
                if(tempBoard[x+k*dx][y+k*dy]==otherColor||x+k*dx>18||y+k*dy>18||x+k*dx<0||y+k*dy<0)
                return 0;

                if(tempBoard[x+k*dx][y+k*dy]==checkingColor)
                count++;

                if(tempBoard[x+k*dx][y+k*dy]==openStone)
                threatCell=k;
        }


        if(k==6&&count==2)
        {
                tempBoard[x+threatCell*dx][y+threatCell*dy]=otherColor;

                return 1;
        }

        return 0;
}

//returns 1 if threats are found on the board else 0
short threatDetector(short checkingColor,short otherColor)
{
        short x,y;
        for(x=0;x<19;x++)
        {
                for(y=0;y<19;y++)
                {
                        if(threats(x,y,checkingColor,otherColor,0,1)==1||threats(x,y,checkingColor,otherColor,1,0)==1||threats(x,y,checkingColor,otherColor,1,1)==1||threats(x,y,checkingColor,otherColor,-1,1)==1)
                        return 1;
                }
        }
        return 0;
}

short threats(short x,short y,short checkingColor,short otherColor,short dx,short dy)
{
        short k,n=0;
        for(k=0;k<6;k++)
        {
                if(board[x+k*dx][y+k*dy]==otherColor||x+k*dx>18||y+k*dy>18||x+k*dx<0||y+k*dy<0)
                break;

                else if(board[x+k*dx][y+k*dy]==checkingColor)
                n++;
        }

        if(k==6&&n>=4)
        {
                return 1;
        }

        return 0;
}

void readAndPlaceStones()
{
        char tmp[8];
        short n=0;
        //UART1_Write_Text(" Enter your moves ");
        while(n<8)
        {
                          while(!UART1_Data_Ready());
                          tmp[n++]=UART1_Read();
        }

        //first move
        opponentMoves[0].x=(tmp[0]-48)*10+tmp[1]-49,opponentMoves[0].y=(tmp[2]-48)*10+tmp[3]-49;
        //second move
        opponentMoves[1].x=(tmp[4]-48)*10+tmp[5]-49,opponentMoves[1].y=(tmp[6]-48)*10+tmp[7]-49;

        board[opponentMoves[0].x][opponentMoves[0].y]=opponentStone;
        board[opponentMoves[1].x][opponentMoves[1].y]=opponentStone;

}

void writeAIMoves()
{
        //first coordinate
        char x[8];
        short n=0;

       x[n++]=(aiMoves[0].x+1)>=10?1+48:0+48;
       x[n++]=(aiMoves[0].x+1)>=10?aiMoves[0].x+1-10+48:aiMoves[0].x+1+48;
       x[n++]=(aiMoves[0].y+1)>=10?1+48:0+48;
       x[n++]=(aiMoves[0].y+1)>=10?aiMoves[0].y+1-10+48:aiMoves[0].y+1+48;

       x[n++]=(aiMoves[1].x+1)>=10?1+48:0+48;
       x[n++]=(aiMoves[1].x+1)>=10?aiMoves[1].x+1-10+48:aiMoves[1].x+1+48;
       x[n++]=(aiMoves[1].y+1)>=10?1+48:0+48;
       x[n++]=(aiMoves[1].y+1)>=10?aiMoves[1].y+1-10+48:aiMoves[1].y+1+48;

       n=0;
       while(n<8)
       {
                 UART1_Write(x[n]);
                 Delay_ms(50);
                 n++;
       }
}