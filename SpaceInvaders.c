#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "image.h"

#define R 6 //row
#define C 7 //column

//variables
int Symbol;// 1 bold / 2 round
int matrix[R][C];
int turn=1,move;
int cursor = 0;
int depth=2;
unsigned long slider; //SW1 
unsigned long insert; //SW2
//prototypes

void gui_of_starting(void);
void printsympol(void);
void insert_sympol(const unsigned char sympol[] , int column, int row);

int check_the_connect_4(int matrix[R][C] , int Gamer);
void draw_table(void);
void printBoard(void);
int insert_matrix(int matrix[R][C], int column, int Gamer);
int possible_position(int matrix[R][C], int column);
void start_playing(void);
int AI_of_computer(int matrix[R][C], int depth) ;
int heuristic(int s[6][7]) ;
int minimax_algorithm(int matrix[R][C], int depth, int turn ) ;
void remove(int matrix[6][7], int column) ;

void Delay_10ms(unsigned long counter); // time delay in 0.01 seconds
void Delay_100ms(unsigned long counter);// time delay in 0.1 seconds




//initialization of port f
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}

int main(void){
  //intializations
	TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Nokia5110_Init();
	PortF_Init(); 
	gui_of_starting();
  printBoard();	
	start_playing();
	
	
}


void gui_of_starting(void){
	// Logo 
	Nokia5110_ClearBuffer(); 
	Nokia5110_PrintBMP(10, 35, logo1, 2);
	Nokia5110_DisplayBuffer(); 
	Nokia5110_SetCursor(2,5);
	Nokia5110_OutString("Welcome");	
	Delay_100ms(2); 
	Nokia5110_ClearBuffer();
	// mode of the game 
	Nokia5110_Clear();
	Nokia5110_SetCursor(2,1);
	Nokia5110_OutString("This is");
	Nokia5110_SetCursor(1,3);
	Nokia5110_OutString("Single Mode");
			Delay_100ms(2);
	while (1){ 
		// select symbol 
				Delay_100ms(1);
		Nokia5110_Clear();
		Nokia5110_ClearBuffer();
		Nokia5110_PrintBMP(3,20, Bold6, 2);
		Nokia5110_DisplayBuffer();
		Nokia5110_PrintBMP(3,35, Round6, 2);
		Nokia5110_DisplayBuffer();
		Nokia5110_SetCursor(0,0);
		Nokia5110_OutString("Select sympo");
		Nokia5110_SetCursor(3,2);
		Nokia5110_OutString("Press SW1");
		Nokia5110_SetCursor(3,4);
		Nokia5110_OutString("Press SW2");
				Delay_100ms(1);
		// set the value of symbol bold or round 
		while(1){
			if (!(GPIO_PORTF_DATA_R&0x10)){ 
			Symbol = 1;
			break;
			}
			else if (!(GPIO_PORTF_DATA_R&0x01)){ 
			Symbol = 2;
			break;
			}	
		}
		break;
	}
		Delay_100ms(1);
	  Nokia5110_ClearBuffer();
	  Nokia5110_DisplayBuffer();
		Nokia5110_SetCursor(0,0);
	  Nokia5110_OutString("Press :");
		Nokia5110_SetCursor(0,2);
  	Nokia5110_OutString("SW1: move");
		Nokia5110_SetCursor(0,3);
	  Nokia5110_OutString("SW2: insert");
		Nokia5110_SetCursor(0,4);
		Nokia5110_OutString("Press any to cont");
		Delay_100ms(1);
		while(1){
			if(!(GPIO_PORTF_DATA_R&0x01)||!(GPIO_PORTF_DATA_R&0x10)) break;
		}
		Delay_10ms(10);
		 
}

void start_playing(void){
	do{
		Nokia5110_ClearBuffer();	
		draw_table();
		insert_sympol(Symbol==1? Bold6:Round6, cursor, 6); // print sympol
		printBoard();
		Nokia5110_DisplayBuffer();
		if(turn==1){
					do {
						while(1){
							slider = GPIO_PORTF_DATA_R&0x10 ; 
							insert = GPIO_PORTF_DATA_R&0x01; 	
							if (slider== 0x00){ //slider pressed 
								cursor++;
								cursor= cursor%7;
									Nokia5110_ClearBuffer(); 
									draw_table();
									insert_sympol(Symbol==1? Bold6:Round6, cursor, 6);
									printBoard();
									Nokia5110_DisplayBuffer(); //
								Delay_10ms(20);
								}
								if(insert == 0x00 ) {//insert pressed
									break;							
							}
					}
						move = cursor;
					}
					while(possible_position(matrix, move) == 0);
						
			}
			else{
					do {
							move = AI_of_computer(matrix, depth);
					}while(possible_position(matrix, move) == 0);
			}
			
			
			insert_matrix(matrix, move, turn);
			printBoard();
			Nokia5110_DisplayBuffer();    
			Delay_10ms(10);              
			turn = 3-turn ;

	}while( check_the_connect_4(matrix,3-turn) == 0 );
	Nokia5110_ClearBuffer();
	Nokia5110_Clear();
	Nokia5110_SetCursor(2,2);	
  if(check_the_connect_4(matrix,3-turn) == 1 ){ 
		Nokia5110_OutString("You Win");
	}
	else if(check_the_connect_4(matrix,3-turn) == 2 ) {
		Nokia5110_OutString("You Lose");
		
		
	}
		else if(check_the_connect_4(matrix,3-turn) == 3 ) {
			
				Nokia5110_OutString("Game over");
					
    }
			Delay_10ms(100);
}	
			

//if return 0=not finished , 1=me , 2=computer , -1=game stopped
int check_the_connect_4(int matrix[R][C],int Gamer) {
    int i, j;
    //check horizontals
    for(i=0; i<R; i++)
        for(j=0; j<=3; j++){
            if(matrix[i][j]== Gamer && matrix[i][j+1]== Gamer && matrix[i][j+2]== Gamer && matrix[i][j+3]== Gamer)
                return Gamer;
        }
    //check verticals
    for(i=0; i<=2; i++)
        for(j=0; j<C; j++){
            if(matrix[i][j]== Gamer && matrix[i+1][j]== Gamer && matrix[i+2][j]== Gamer && matrix[i+3][j]== Gamer )
                return Gamer;
        }
    //check diagonals (\)
    for(i=(R-1); i>=3; i--)
        for(j=0; j<=3; j++){
            if(matrix[i][j]== Gamer && matrix[i-1][j+1]== Gamer && matrix[i-2][j+2]== Gamer && matrix[i-3][j+3]== Gamer)
                return Gamer;
        }
    //check diagonals (/)
    for(i=0; i<=2; i++)
        for(j=0; j<=3; j++){
            if(matrix[i][j]== Gamer && matrix[i+1][j+1]== Gamer && matrix[i+2][j+2]== Gamer && matrix[i+3][j+3]== Gamer)
                return Gamer;
        }
    //check the game is not finished // there is at least one empty place
    for(i=0; i<7; i++){
        if(matrix[5][i]==0)
            return 0; }
		
     //the matrix is full (game over / stoped)
						return 3;
}

void insert_sympol(const unsigned char sympol[] , int column,int row ){
		Nokia5110_PrintBMP(((84/C)*column)+3 , 47-(6*row) , sympol ,0);


}

void draw_table(void){
	int i ,j; 		
       	for(i=12;i<=84;i+=12){  // 84 width of the nokia 5110 screen	
						for(j=47;j>12;j-=6){ //47 length of nokia 5110 screen	
								Nokia5110_PrintBMP(i,j,Line,0);
		}
	}				
}



void printBoard(void){
	int i=0 ;
		while (i<6 ){//row
			int j=0 ;
			while(j<7){//column
				//print cursor 
				if (matrix[i][j]==1){
					insert_sympol(Symbol==1? Bold6:Round6, j, i); // print sympol
				}
				else if(matrix[i][j]==2){
					insert_sympol(Symbol==1? Round6:Bold6, j, i); // print sympol
				}
					j++;
			}
			i++;
		}
}

int possible_position(int matrix[R][C], int column){
    int i,no_of_rows =0;
				for(i = 0; i< R; i++) {
						if(matrix[i][column] != 0)
								no_of_rows++;
						}
    if(no_of_rows == 6)
        return 0;//no possible position
    else
        return 1;//possible position
		
}

int insert_matrix(int matrix[R][C], int column, int Gamer) {
    int i;
    for(i = 0; i < R; i++) {
        if(matrix[i][column] == 0) {
            matrix[i][column] = Gamer;
            return i+1;//return no of row inserted
        }
    }
		return -1;//not inserted
}



int heuristic(int s[6][7]) {
    int result = 0;
    int i, j;
 
    //check horizontals
    for(i=0; i<6; i++)
        for(j=0; j<=3; j++){
            if(s[i][j]!= 2 && s[i][j+1]!= 2 && s[i][j+2]!= 2 && s[i][j+3]!= 2)
                result++;
            if(s[i][j]!= 1 && s[i][j+1]!= 1 && s[i][j+2]!= 1 && s[i][j+3]!= 1)
                result--;
        }
 
    //check verticals
    for(i=0; i<=3; i++)
        for(j=0; j<7; j++){
            if(s[i][j]!= 2 && s[i+1][j]!= 2 && s[i+2][j]!= 2 && s[i+3][j]!= 2 )
                result++;
            if(s[i][j]!= 1 && s[i+1][j]!= 1 && s[i+2][j]!= 1 && s[i+3][j]!= 1 )
                result--;
        }
 
    return result;
}

int minimax_algorithm(int matrix[R][C], int depth, int turn ) {
	
    int column,best_value=-5000,n,check,j;
	
		if(depth==0){
			if(turn==1)	
						return heuristic(matrix);
			else 
						return -heuristic(matrix);
	}
		check=check_the_connect_4(matrix,3-turn);
		
   if(check) {
        if(check==3)
            return 0;
        if(check==turn)
            return 5000;
        else
            return -5000;
    }
    
    for(column=0; column < 7; column++)     
        if(matrix[5][column]==0) {   
            insert_matrix(matrix, column, turn);
            n = minimax_algorithm(matrix, depth-1, 3-turn);
            if(turn==1) {
                if ( -n > best_value ) best_value = -n;
            }
						else { //turn==2
                if ( -n > best_value ) best_value = -n;
            }
						
         //   remove from the matrix that is not needed
    for (j=R-1; j>=0; j--){
        if (matrix[j][column] != 0) {
            matrix[j][column] = 0;
            break;
										}
								}
		

        }
    return best_value;
}

int AI_of_computer(int matrix[R][C], int depth) {
    int col, move, n, val = -5000-1,i;
    for(col=0; col < 7; col++)
        if(matrix[5][col]==0) {
            insert_matrix(matrix, col, 2);
            n = minimax_algorithm(matrix, depth, 1);
            if ( -n > val ) {
                val = -n;
                move = col;
            }
						    //   remove from the matrix that is not needed
            for (i=R-1; i>=0; i--){
        if (matrix[i][col] != 0) {
            matrix[i][col] = 0;
            break;
        }
    }
        }
 
    return move;
}



void Delay_10ms(unsigned long counter){
	unsigned long volatile time1;
  while(counter>0){
    time1 = 72724;  // 0.01sec at 80 MHz
    while(time1){

				time1--;
    }
    counter--;
 }
}

void Delay_100ms(unsigned long counter){unsigned long volatile time1;
  while(counter>0){
    time1 = 727240;  // 0.1sec at 80 MHz
    while(time1){
	  	time1--;
    }
    counter--;
  }
}