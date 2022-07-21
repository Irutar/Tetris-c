#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "primlib.h"
#include "pieces.inl"


#define BLOCK_SIZE 20 //square size
#define GAME_AREA_WIDTH 10 //number of blocks that fit in one row
#define GAME_AREA_HEIGHT 20 //number of blocks that fit in one column
#define MOVING_PIECE_COLOR YELLOW
#define ROTATION_BLOCK_COLOR RED
#define STATIC_PIECE_COLOR BLUE
#define TEXT_COLOR WHITE
#define BLOCK_FRAME_COLOR GREEN
#define TIME 100000 //in microseconds
#define X_LEFT_UPPER_CORNER 100
#define Y_LEFT_UPPER_CORNER 70
#define FRAME_WIDTH 5
#define NEXT_PIECE_AREA_SHIFT_LEFT 11
#define NEXT_PIECE_AREA_SHIFT_RIGHT 28
#define TEXT_HEIGHT 10
#define END_OF_GAME_TEXT screenWidth()-1)/2-100
#define REMOVE_ROWS_TO_WIN 20
#define TURN 10

#if (GAME_AREA_WIDTH<10 || GAME_AREA_WIDTH>20 || GAME_AREA_HEIGHT<10 || GAME_AREA_HEIGHT>20)
	#error: "the wrong dimension of the game's area"
#endif /*Condition for dimentions of the game area*/

int is_Manipulation_Possible(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3], int x, int y, char piece[4][4], char move);
void Draw_Game_State(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3],char Next_Piece[4][4]);
void Remove_Row(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3]);
void Display_Current_Result();
void Insert_Piece(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3],int x, int y, char Generated_Piece[4][4], enum color colour);
int Play(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3], int x, int y, char Which_Piece[4][4][4]);
void You_Win();
void You_Lose();

int Removed_Rows;

int main ()
{
	int column,row;
	int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3];
	int Actual_Piece,Next_Piece;

	if(initGraph()==0)
		printf("Failed to initialize graphics library");

	Removed_Rows=0;

	srand(time(NULL));

	for(column=0;column<GAME_AREA_WIDTH;column++)
		for(row=0;row<GAME_AREA_HEIGHT;row++)
		{
			Game_Area [column][row][0]= X_LEFT_UPPER_CORNER+column*BLOCK_SIZE;
			Game_Area [column][row][1]= Y_LEFT_UPPER_CORNER+row*BLOCK_SIZE;
			Game_Area [column][row][2]= 0;
		}
		Actual_Piece=rand()%7;
	while(1)
	{
		Next_Piece=(int)rand()%7;

		Draw_Game_State(Game_Area,pieces[Next_Piece][0]);	
		updateScreen();

		if(is_Manipulation_Possible(Game_Area,(GAME_AREA_WIDTH/2)-1,0,pieces[Actual_Piece][0],'0')==0)
		{
				You_Lose();
				break;
		}
		Insert_Piece(Game_Area,(GAME_AREA_WIDTH/2)-1,0,pieces[Actual_Piece][0],MOVING_PIECE_COLOR);
		updateScreen();

		if(Play(Game_Area,(GAME_AREA_WIDTH/2)-1,0,pieces[Actual_Piece])==0)
		{
			You_Win();
			break;
		}

		Remove_Row(Game_Area);

		Actual_Piece=Next_Piece;
	}
	updateScreen();
	return 0;

}

void Draw_Game_State(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3], char Next_Piece[4][4])
{
	int column,row;

	filledRect(0,0,screenWidth()-1,screenHeight()-1,BLACK);

	textout(0,TEXT_HEIGHT, "Use arrows to move, and space to rotate the piece.",TEXT_COLOR);
	textout(0,2*TEXT_HEIGHT,"Press Esc to quit",TEXT_COLOR);

	filledRect(X_LEFT_UPPER_CORNER-FRAME_WIDTH,Y_LEFT_UPPER_CORNER,X_LEFT_UPPER_CORNER-1,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE),TEXT_COLOR);

	filledRect(X_LEFT_UPPER_CORNER-FRAME_WIDTH,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE),
		X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+FRAME_WIDTH,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE)+FRAME_WIDTH,TEXT_COLOR);

	filledRect(X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE),Y_LEFT_UPPER_CORNER,
		X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+FRAME_WIDTH,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE),TEXT_COLOR);

	/*rect(X_LEFT_UPPER_CORNER-FRAME_WIDTH,Y_LEFT_UPPER_CORNER,X_LEFT_UPPER_CORNER-1,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE),BLOCK_FRAME_COLOR);

	rect(X_LEFT_UPPER_CORNER-FRAME_WIDTH,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE),
		X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+FRAME_WIDTH,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE)+FRAME_WIDTH,BLOCK_FRAME_COLOR);

	rect(X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE),Y_LEFT_UPPER_CORNER,
		X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+FRAME_WIDTH,Y_LEFT_UPPER_CORNER+(GAME_AREA_HEIGHT*BLOCK_SIZE),BLOCK_FRAME_COLOR);*/

	for(column=0;column<GAME_AREA_WIDTH;column++)
		for(row=0;row<GAME_AREA_HEIGHT;row++)
		{
			filledRect(Game_Area[column][row][0]+1,Game_Area[column][row][1]+1,Game_Area[column][row][0]+BLOCK_SIZE-2,Game_Area[column][row][1]+BLOCK_SIZE-2,Game_Area[column][row][2]);
			rect(Game_Area[column][row][0]+1,Game_Area[column][row][1]+1,Game_Area[column][row][0]+BLOCK_SIZE-2,Game_Area[column][row][1]+BLOCK_SIZE-2,Game_Area[column][row][2]);
		}

	textout(X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_LEFT,Y_LEFT_UPPER_CORNER,"Next piece:",TEXT_COLOR);
	for(column=0;column<4;column++)
		for(row=0;row<4;row++)
			if(Next_Piece[column][row]!=0)
			{
				filledRect(Game_Area[column][row][0]+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_LEFT, Game_Area[column][row][1]+NEXT_PIECE_AREA_SHIFT_LEFT,
					Game_Area[column][row][0]+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_RIGHT,Game_Area[column][row][1]+NEXT_PIECE_AREA_SHIFT_RIGHT, MOVING_PIECE_COLOR);
				rect(Game_Area[column][row][0]+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_LEFT, Game_Area[column][row][1]+NEXT_PIECE_AREA_SHIFT_LEFT,
					Game_Area[column][row][0]+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_RIGHT,Game_Area[column][row][1]+NEXT_PIECE_AREA_SHIFT_RIGHT, BLOCK_FRAME_COLOR);
			}
	Display_Current_Result();
}

void Display_Current_Result()
{
	char result[40];
	char to_win[40];
	int rows_left=REMOVE_ROWS_TO_WIN-Removed_Rows;
	sprintf(result,"Rows removed: %d", Removed_Rows);
	sprintf(to_win,"Rows left to win: %d",rows_left);
	textout(X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_LEFT,Y_LEFT_UPPER_CORNER*2+NEXT_PIECE_AREA_SHIFT_RIGHT,result,TEXT_COLOR);
	textout(X_LEFT_UPPER_CORNER+(GAME_AREA_WIDTH*BLOCK_SIZE)+NEXT_PIECE_AREA_SHIFT_LEFT,Y_LEFT_UPPER_CORNER*2+NEXT_PIECE_AREA_SHIFT_RIGHT+TEXT_HEIGHT,to_win,TEXT_COLOR);
}

int is_Manipulation_Possible(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3], int x, int y, char piece[4][4], char move)
{
	int column,row;

	switch(move)
	{
		case('l'): x--; break;

		case('r'): x++; break;

		case('d'): y++; break;

		case('0'): break;
	};

	if(x<0 || x>=GAME_AREA_WIDTH || y>=GAME_AREA_HEIGHT) return 0;

	for(column=0;column<4;column++)
		for(row=0;row<4;row++)
		{
			if(piece[column][row]!=0 && (x+column>=GAME_AREA_WIDTH || y+row>=GAME_AREA_HEIGHT))
				return 0;
			if(piece[column][row]!=0 && Game_Area[x+column][y+row][2]!=0)
				return 0;
		}
	return 1;	
}

void Insert_Piece(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3],int x, int y, char Generated_Piece[4][4], enum color colour)
{
	int column,row;
	int keep_colour=colour;
	for(column=0;column<4;column++)
		for(row=0;row<4;row++)
		{
			if(Generated_Piece[column][row]!=0)
			{	
				if(colour==BLACK)	
					colour=BLACK;
				else if(Generated_Piece[column][row]==2)
				colour=ROTATION_BLOCK_COLOR;
				filledRect(Game_Area[x+column][y+row][0]+1,Game_Area[x+column][y+row][1],Game_Area[x+column][y+row][0]+BLOCK_SIZE-2,Game_Area[x+column][y+row][1]+BLOCK_SIZE-2,colour);
				colour=keep_colour;
				if(colour!=BLACK)
				rect(Game_Area[x+column][y+row][0]+1,Game_Area[x+column][y+row][1],Game_Area[x+column][y+row][0]+BLOCK_SIZE-2,Game_Area[x+column][y+row][1]+BLOCK_SIZE-2,BLOCK_FRAME_COLOR);
				colour=keep_colour;
			}		
		}
}

int Play(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3], int x, int y, char Which_Piece[4][4][4])
{
	int column,row,turn;
	int rotation=0;

	while(1)
	{
		if((REMOVE_ROWS_TO_WIN-Removed_Rows)==0)
			return 0;
		for(turn=0;turn<TURN;turn++)
		{
			if(isKeyDown(SDLK_ESCAPE)==1) exit(1);

			switch(pollkey())
			{
				case(SDLK_DOWN):
					while(is_Manipulation_Possible(Game_Area,x,y, Which_Piece[rotation],'d')==1)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						y++;
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
						updateScreen();
					}
					goto Save;

				case(SDLK_RIGHT):
					while (is_Manipulation_Possible(Game_Area,x,y,Which_Piece[rotation],'r')==1 && isKeyDown(SDLK_RIGHT)==1)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						x++;
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
						updateScreen();
						turn++;
						if(turn==TURN)
						{
							Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
							if(is_Manipulation_Possible(Game_Area,x,y+1,Which_Piece[rotation],'r')==0)
								break;
							else
							y++;
							turn=0;
							continue;
						}
						usleep(TIME);
					}break;

				case(SDLK_LEFT):
					while (is_Manipulation_Possible(Game_Area,x,y,Which_Piece[rotation],'l')==1 && isKeyDown(SDLK_LEFT)==1)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						x--;
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
						updateScreen();
						turn++;
					if(turn==TURN)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						if(is_Manipulation_Possible(Game_Area,x,y+1,Which_Piece[rotation],'l')==0)
							break;
						else
						y++;
						turn=0;
						continue;
					}
					usleep(TIME);
					}break;

				case(SDLK_SPACE):
				while(isKeyDown(SDLK_SPACE)==1)
				{
					if (is_Manipulation_Possible(Game_Area,x,y,Which_Piece[(rotation+1)%4],'0')==1 && isKeyDown(SDLK_SPACE)==1)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						rotation++; rotation%=4;
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
						updateScreen();
						usleep(TIME);
					}
					else if (is_Manipulation_Possible(Game_Area,x-1,y,Which_Piece[(rotation+1)%4],'0')==1)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						rotation++; rotation%=4; x--;
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
						updateScreen();
						usleep(TIME);
					}
					else if (is_Manipulation_Possible(Game_Area,x+1,y,Which_Piece[(rotation+1)%4],'0')==1)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						rotation++; rotation%=4; x++;
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
						updateScreen();
						usleep(TIME);
					}
					turn++;
					if(turn==TURN)
					{
						Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
						if(is_Manipulation_Possible(Game_Area,x,y+1,Which_Piece[(rotation+1)%4],'0')==0 && is_Manipulation_Possible(Game_Area,x+1,y+1,Which_Piece[(rotation+1)%4],'0')==0 && is_Manipulation_Possible(Game_Area,x-1,y+1,Which_Piece[(rotation+1)%4],'0')==0)
							break;
						else
						y++;
						turn=0;
						continue;
					}
				}	break;

			}
			usleep(TIME);
		}
		if(is_Manipulation_Possible(Game_Area,x,y, Which_Piece[rotation],'d')==1)
				{
					Insert_Piece(Game_Area,x,y,Which_Piece[rotation], BLACK);
					y++;
					Insert_Piece(Game_Area,x,y,Which_Piece[rotation], MOVING_PIECE_COLOR);
					updateScreen();
				}
		else break;
	}	
	Save:
		for(column=0;column<4;column++)
			for(row=0;row<4;row++)
				if(Which_Piece[rotation][column][row]!=0)
					Game_Area[x+column][y+row][2]=STATIC_PIECE_COLOR;

	return 1;
}

void Remove_Row(int Game_Area[GAME_AREA_WIDTH][GAME_AREA_HEIGHT+1][3])
{
	int column,row,deleted_row;
	int Does_Block_Exist=1;

	for(row=0;row<GAME_AREA_HEIGHT;row++)
	{
		for(column=0;column<GAME_AREA_WIDTH;column++)
		{
			Does_Block_Exist*=Game_Area[column][row][2];
			if(Does_Block_Exist==0) break;
		}
		if(Does_Block_Exist!=0)
		{
			for(deleted_row=row;deleted_row>0;deleted_row--)
				for(column=0;column<GAME_AREA_WIDTH;column++)
					Game_Area[column][deleted_row][2]=Game_Area[column][deleted_row-1][2];

			for(column=0;column<GAME_AREA_WIDTH;column++)
				Game_Area[column][0][2]=0;

			Removed_Rows++;
		}
		Does_Block_Exist=1;
	}
}

void You_Win()
{
	filledRect(0,0,screenWidth()-1,screenHeight()-1,BLACK);
	textout((END_OF_GAME_TEXT,(screenHeight()-1)/2,"End of the game. You Won.",TEXT_COLOR); 
	updateScreen();
	sleep(3);
}

void You_Lose()
{
	char result[40];
	filledRect(0,0,screenWidth()-1,screenHeight()-1,BLACK);
	sprintf(result,"End of the game. You removed %d rows",Removed_Rows);
	textout((END_OF_GAME_TEXT,(screenHeight()-1)/2,result,TEXT_COLOR);
	updateScreen();
	sleep(3);
}
