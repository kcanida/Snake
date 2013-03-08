/*
 * EE357: Fall 2011 Final Project
 * Kellie Canida and Anthony Chung
 */


#include "support_common.h" /* include peripheral declarations and more */
#include "usc_support.h"
#include "lcd128x64.h"
#include "fonts.h"
#include "i2c.h"
#include <string.h>
#include <stdlib.h>//include standard library to use rand function
#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT)
/* Standard IO is only possible if Console or UART support is enabled. */
#include <stdio.h>
#endif


void init_gpio()
{

	// Init. input buttons that are connected to PortTA
	MCF_GPIO_PTAPAR = 0
	    | MCF_GPIO_PTAPAR_ICOC0_GPIO
	    | MCF_GPIO_PTAPAR_ICOC1_GPIO;
	/* Set Data Direction to all input */
	MCF_GPIO_DDRTA = 0;
	/**
    //Enable 4 LED signals as GPIO 
	
    MCF_GPIO_PTCPAR = 0
        | MCF_GPIO_PTCPAR_DTIN3_GPIO
        | MCF_GPIO_PTCPAR_DTIN2_GPIO
        | MCF_GPIO_PTCPAR_DTIN1_GPIO
        | MCF_GPIO_PTCPAR_DTIN0_GPIO;

    //Enable signals as digital outputs 
    MCF_GPIO_DDRTC = 0
        | MCF_GPIO_DDRTC_DDRTC3
        | MCF_GPIO_DDRTC_DDRTC2
        | MCF_GPIO_DDRTC_DDRTC1
        | MCF_GPIO_DDRTC_DDRTC0;

	//Enable 2 LED's on iMn FM board 
    
	MCF_GPIO_PTHPAR = 0
		| MCF_GPIO_PTHPAR_MB_D0_GPIO
		| MCF_GPIO_PTHPAR_MB_D2_GPIO;

	//MCF_GPIO_DDRTH = MCF_GPIO_DDRTH_DDRTH0 | MCF_GPIO_DDRTH_DDRTH2;
	**/
}
//function outputs a 1 if button1 is pushed and 0 if not pushed
int get_SW1_v2()
{
	int i;
	// Read the current state of the switch
	if(!(MCF_GPIO_SETTA & MCF_GPIO_SETTA_SETTA0))
	{
		cpu_pause(5000);
		if(!(MCF_GPIO_SETTA & MCF_GPIO_SETTA_SETTA0))
		{
		// Wait until button is released
		while(!(MCF_GPIO_SETTA & MCF_GPIO_SETTA_SETTA0));
		return 1;
		}
	}
	return 0;

}


//function initializes accelerometer 
void init_accel(){
    /****************************************/
	/* Setup ADC0 on M52259 Tower Board        */
    /****************************************/
    /* Setting PortAN0 as ADC functionality */
    MCF_GPIO_PANPAR |= MCF_GPIO_PANPAR_PANPAR3 
					  | MCF_GPIO_PANPAR_PANPAR2 
					  | MCF_GPIO_PANPAR_PANPAR1
					  | MCF_GPIO_PANPAR_PANPAR0;
}
//function outputs 1,-1,2, or -2 depending on direction from accelerometer
int get_dir_accel_v2(){
		//int counter = 0;
		int x=2000,y=2000;
	   /****************************************/
	    /* Setup ADC Module					    */
		/****************************************/

		/* Initialize the CRTL1 register to 0's with SMODE = Once Sequential = 000 */
	    MCF_ADC_CTRL1 = 0 | MCF_ADC_CTRL1_SMODE(0);

	    /* Setting divisor in CTRL2 register */
	    MCF_ADC_CTRL2 = MCF_ADC_CTRL2_DIV(3);

	    /* Setting Power Register appropriately - PUDELAY & clear PD0*/
	    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(4);

	    /* Set AN0 to sample channel 3 once.  Note we will only look 	*/
	    /* at the result of sample 0 even though the ADC will take  	*/
	    /* an 8 sample scan 											*/
	    MCF_ADC_ADLST1 = 0 | MCF_ADC_ADLST1_SAMPLE0(0)
							| MCF_ADC_ADLST1_SAMPLE1(1)
							| MCF_ADC_ADLST1_SAMPLE2(2)
							| MCF_ADC_ADLST1_SAMPLE3(3);

		/* Clear stop bit */
		MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0;

		while(1){
			/* Toggle the LED's */
			MCF_GPIO_PORTTC = ~MCF_GPIO_PORTTC;

			/* Clear stop bit */
			MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0;
		
			/* Set start bit */
			MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0;
		
			/* Waiting for the last "Ready" bit to be set so we don't start the next scan to early*/
			while (!(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY7));
		
			/* Toggle the LED's */
			MCF_GPIO_PORTTC = ~MCF_GPIO_PORTTC;

			x = MCF_ADC_ADRSLT(0) >> 3;
			y = MCF_ADC_ADRSLT(1) >> 3;
			//sample = MCF_ADC_ADRSLT(3) >> 3;
					//threshold values
					if(x < 1600){
						return -1;
					}
					else if(x > 2200){
						return 1;
					}
					if(y < 1600){
						return -2;
					}
					else if(y > 2200){
						return 2;
					}
		}
}

//function that initializes joy stick input 
void init_joy()
{  
   /* Enable signals as digital outputs */
    MCF_GPIO_DDRTJ = 0
        | MCF_GPIO_DDRTJ_DDRTJ0
        | MCF_GPIO_DDRTJ_DDRTJ1
        | MCF_GPIO_DDRTJ_DDRTJ6
        | MCF_GPIO_DDRTJ_DDRTJ7 | MCF_GPIO_DDRTJ_DDRTJ2;
    MCF_GPIO_PORTTJ = 0xff; 
}
//function that outputs a 1 if joy stick switch is left
int get_lt_sw_v2()
{
    int i;
    /* Read the current state of the switch */
    if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ0))
    {
        //cpu_pause(5000);
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ0))
        {
        // Wait until button is released
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ0));
        return 1;            
        }
    }
    return 0;
}

//function that outputs a 1 if joy stick switch is right
int get_rt_sw_v2()
{
    int i;
    /* Read the current state of the switch */
    if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ1))
    {
        //cpu_pause(5000);
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ1))
        {
        // Wait until button is released
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ1));
        return 1;            
        }
    }
    return 0;
}

//function that outputs a 1 if joy stick switch is down
int get_down_sw_v2()
{
    int i;
    /* Read the current state of the switch */
    if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ6))
    {
        //cpu_pause(5000);
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ6))
        {
        // Wait until button is released
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ6));
        return 1;            
        }
    }
    return 0;
}
//added a new function that outputs a 1 if switch is up
int get_up_sw_v2()
{
    int i;
    /* Read the current state of the switch */
    if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ7))
    {
        //cpu_pause(5000);
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ7))
        {
        // Wait until button is released
        if(!(MCF_GPIO_SETTJ & MCF_GPIO_SETTJ_SETTJ7));
        return 1;            
        }
    }
    return 0;
}

//function deletes tail pixels and draws new head pixels
void grphTailHead (int k, int accel, int tailDirection,int x10,int x20,int x30,int x1L,int x2L,int x3L,int y10,int y20,int y30,int y1L,int y2L,int y3L){		
			if(tailDirection==1 && accel==1){//tail moving right and head moving right(don't continue to draw if head hits wall)
						if(k==1 && x30<127){
							grphClrVertLine(y3L,y1L,x1L);
							grphVertLine(y30,y10,x10);
						}
						else if(k==2 && x30<127){
							grphClrVertLine(y3L,y1L,x2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x30<127){
							grphClrVertLine(y3L,y1L,x3L);
							grphVertLine(y30,y10,x30);
						}
					}
					else if(tailDirection==1 && accel==-1){//tail moving right and head moving left(don't continue to draw if head hits wall)
						if(k==1 && x10>0){
							grphClrVertLine(y3L,y1L,x1L);
							grphVertLine(y30,y10,x30);
						}
						else if(k==2 && x10>0){
							grphClrVertLine(y3L,y1L,x2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x10>0){
							grphClrVertLine(y3L,y1L,x3L);
							grphVertLine(y30,y10,x10);
						}
					}
					else if(tailDirection==1 && accel==2){//tail moving right and head moving up(don't continue to draw if head hits wall)
						if(k==1 && y30>0){
							grphClrVertLine(y3L,y1L,x1L);
							grphHorizLine(x10,x30,y10);
						}
						else if(k==2 && y30>0){
							grphClrVertLine(y3L,y1L,x2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y30>0){
							grphClrVertLine(y3L,y1L,x3L);
							grphHorizLine(x10,x30,y30);
						}
					}
					else if(tailDirection==1 && accel==-2){//tail moving right and head moving down(don't continue to draw if head hits wall)
						if(k==1 && y10<61){
							grphClrVertLine(y3L,y1L,x1L);
							grphHorizLine(x10,x30,y30);
						}
						else if(k==2 && y10<61){
							grphClrVertLine(y3L,y1L,x2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y10<61){
							grphClrVertLine(y3L,y1L,x3L);
							grphHorizLine(x10,x30,y10);
						}
					}
					else if(tailDirection==-1 && accel==1){//tail moving left and head moving right(don't continue to draw if head hits wall)
						if(k==1 && x30<127){
							grphClrVertLine(y3L,y1L,x3L);
							grphVertLine(y30,y10,x10);
						}
						else if(k==2 && x30<127){
							grphClrVertLine(y3L,y1L,x2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x30<127){
							grphClrVertLine(y3L,y1L,x1L);
							grphVertLine(y30,y10,x30);
						}
					}
					else if(tailDirection==-1 && accel==-1){//tail moving left and head moving left
						if(k==1 && x10>0){
							grphClrVertLine(y3L,y1L,x3L);
							grphVertLine(y30,y10,x30);
						}
						else if(k==2 && x10>0){
							grphClrVertLine(y3L,y1L,x2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x10>0){
							grphClrVertLine(y3L,y1L,x1L);
							grphVertLine(y30,y10,x10);
						}
					}
					else if(tailDirection==-1 && accel==2){//tail moving left and head moving up
						if(k==1 && y30>0){
							grphClrVertLine(y3L,y1L,x3L);
							grphHorizLine(x10,x30,y10);
						}
						else if(k==2 && y30>0){
							grphClrVertLine(y3L,y1L,x2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y30>0){
							grphClrVertLine(y3L,y1L,x1L);
							grphHorizLine(x10,x30,y30);
						}
					}
					else if(tailDirection==-1 && accel==-2){//tail moving left and head moving down
						if(k==1 && y10<61){
							grphClrVertLine(y3L,y1L,x3L);
							grphHorizLine(x10,x30,y30);
						}
						else if(k==2 && y10<61){
							grphClrVertLine(y3L,y1L,x2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y10<61){
							grphClrVertLine(y3L,y1L,x1L);
							grphHorizLine(x10,x30,y10);
						}
					}
					else if(tailDirection==2 && accel==1){//tail moving up and head moving right
						if(k==1 && x30<127){
							grphClrHorizLine(x1L,x3L,y1L);
							grphVertLine(y30,y10,x10);
						}
						else if(k==2 && x30<127){
							grphClrHorizLine(x1L,x3L,y2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x30<127){
							grphClrHorizLine(x1L,x3L,y3L);
							grphVertLine(y30,y10,x30);
						}
					}
					else if(tailDirection==2 && accel==-1){//tail moving up and head moving left
						if(k==1 && x10>0){
							grphClrHorizLine(x1L,x3L,y1L);
							grphVertLine(y30,y10,x30);
						}
						else if(k==2 && x10>0){
							grphClrHorizLine(x1L,x3L,y2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x10>0){
							grphClrHorizLine(x1L,x3L,y3L);
							grphVertLine(y30,y10,x10);
						}
					}
					else if(tailDirection==2 && accel==2){//tail moving up and head moving up
						if(k==1 && y30>0){
							grphClrHorizLine(x1L,x3L,y1L);
							grphHorizLine(x10,x30,y10);
						}
						else if(k==2 && y30>0){
							grphClrHorizLine(x1L,x3L,y2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y30>0){
							grphClrHorizLine(x1L,x3L,y3L);
							grphHorizLine(x10,x30,y30);
						}
					}
					else if(tailDirection==2 && accel==-2){//tail moving up and head moving down
						if(k==1 && y10<61){
							grphClrHorizLine(x1L,x3L,y1L);
							grphHorizLine(x10,x30,y30);
						}
						else if(k==2 && y10<61){
							grphClrHorizLine(x1L,x3L,y2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y10<61){
							grphClrHorizLine(x1L,x3L,y3L);
							grphHorizLine(x10,x30,y10);
						}
					}
					else if(tailDirection==-2 && accel==1){//tail moving down and head moving right
						if(k==1 && x30<127){
							grphClrHorizLine(x1L,x3L,y3L);
							grphVertLine(y30,y10,x10);
						}
						else if(k==2 && x30<127){
							grphClrHorizLine(x1L,x3L,y2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x30<127){
							grphClrHorizLine(x1L,x3L,y1L);
							grphVertLine(y30,y10,x30);
						}
					}
					else if(tailDirection==-2 && accel==-1){//tail moving down and head moving left
						if(k==1 && x10>0){
							grphClrHorizLine(x1L,x3L,y3L);
							grphVertLine(y30,y10,x30);
						}
						else if(k==2 && x10>0){
							grphClrHorizLine(x1L,x3L,y2L);
							grphVertLine(y30,y10,x20);
						}
						else if(k==3 && x10>0){
							grphClrHorizLine(x1L,x3L,y1L);
							grphVertLine(y30,y10,x10);
						}
					}
					else if(tailDirection==-2 && accel==2){//tail moving down and head moving up
						if(k==1 && y30>0){
							grphClrHorizLine(x1L,x3L,y3L);
							grphHorizLine(x10,x30,y10);
						}
						else if(k==2 && y30>0){
							grphClrHorizLine(x1L,x3L,y2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y30>0){
							grphClrHorizLine(x1L,x3L,y1L);
							grphHorizLine(x10,x30,y30);
						}
					}
					else if(tailDirection==-2 && accel==-2){//tail moving down and head moving down
						if(k==1 && y10<61){
							grphClrHorizLine(x1L,x3L,y3L);
							grphHorizLine(x10,x30,y30);
						}
						else if(k==2 && y10<61){
							grphClrHorizLine(x1L,x3L,y2L);
							grphHorizLine(x10,x30,y20);
						}
						else if(k==3 && y10<61){
							grphClrHorizLine(x1L,x3L,y1L);
							grphHorizLine(x10,x30,y10);
						}
					}
	
}

//function generates random x coordinate for mouse
int generatemX3(int generateX, int snakeLength,int x3[20] ){
	int i;
	int mX3;
	while(generateX==0){
		mX3 = (rand()%124) + 3;//randomly generate a number between 3 and 126 that is a multiple of 3 
		if(mX3%3==0 && mX3>=3 && mX3<127){
			generateX=1;
			for(i=1; i<snakeLength; i++){
				if(mX3==x3[i]){//mouse is on top of part of snake so set generateX=0
					generateX=0;
			}
		}
		}

	}
	return mX3;
}
//function generates random y coordinate for mouse
int generatemY1(int generateY, int snakeLength, int y1[20]){
	int i;
	int mY1;
	while(generateY==0){
		mY1=(rand()%58) + 3; //randomly generate a number between 3 and 60 that is a multiple of 3
		if(mY1%3==0 && mY1>=3 && mY1<61){
			generateY=1;
			for(i=1; i<snakeLength; i++){
				if(mY1==y1[i]){//mouse is on top of part of snake so set generateY=0
					generateY=0;
			}
		}
		}
	}
	return mY1;
}


int main(void)
{
	int counter=0;
	int	state = 1; //variable that holds the state we are in
				   //1=INITIAL
				   //2=START
				   //3=CHECK
				   //4=MOVE
	               //5=GROW
				   //6=DONE
				   //7=RESET
	int snakeLength=2; //initial length of snake is 2 units
	int score_tens = 0;//initial score tens place is 0
	int score_ones = 0;//initial score ones place is 0
	char disp_score[4];//character array of size 1 to hold score converted to ASCII
	//snake is 3x3 pixels
	int x1[20]={61,58,55};//x1 stores left pixel
	int x2[20]={62,59,56};//x2 stores middle pixel
	int x3[20]={63,60,57};//x3 store right pixel(MUST BE A MULTIPLE of 3)
	int y1[20]={30,30,30};//y1 stores bottom pixel(MUST BE A MULTIPLE of 3)
	int y2[20]={29,29,29};//y2 stores middle pixel
	int y3[20]={28,28,28};//stores top pixel
	int accel=1;//initial direction of accelerometer is to the right
				//1=right, -1=left
				//2=up, -2= down
	int accel_in=1;//variable storing result from accelerometer
	int mX1;   
	int mX2;
	int mX3;//randomly generate mX2 which can only be a multiple of 3 between(1,127)
	int mY3;
	int mY2;
	int mY1;//randomly generate mY1 which can only be a multiple of 3 between (1,61)
	int eaten = 1;//initialize eaten to 1 to indicate a mouse was eaten so we draw new mouse in the beginning
	int generateX=0;//if mX3 was successfully generated
	int generateY=0;//if mY1 was successfully generated
	int i;
	int k=1;//initialize to 1. use k to move snake in multiples of 3 pixels because we define one unit as a 3x3 pixels
	int tailDirection; //direction tail is following body in 


	init_gpio();
	i2c_init();

	init_lcd();                                         // initialize LCD display
    //init_joy();											//initialize joy stick
    init_accel();										//initialize accel controller
    uart_init(0,48000,9600);                            //initialize UART board input
    
  	while (1){
  		cpu_pause(1000/(snakeLength-1));
  		//cpu_pause(100000); //delay clock for 0.1 seconds every time we enter while loop
		if(state==1){
			//display home screen
			grphErase();// clear
			grphBox (0,0,127,61);//draw a box around screen(goes only to 61 so we have multiples of 3)
			grphText (35,10,FONT_FIFTEEN_DOT,(unsigned char *)"SNAKE");//display home screen
			grphText (36,30,FONT_FIVE_DOT,(unsigned char *)"by:  Kellie Canida");
			grphText (50,38,FONT_FIVE_DOT,(unsigned char *)"Anthony Chung");
			grphText (20,53,FONT_FIVE_DOT,(unsigned char *)"Press Button 1 to Continue");
			grphUpdate(SCRN_TOP,SCRN_BOTTOM); //refresh screen only once
			state = 2;
		}
		else if(state==2 && get_SW1_v2()){
		
  			//START state
			grphErase();
			grphBox (0,0,127,61);//draw a box around screen
			
			//draw starting snake
			
			grphHorizLine(x1[snakeLength-1], x3[0], y3[0]);//draw horizontal line for y3
			grphHorizLine(x1[snakeLength-1], x3[0], y2[0]);//draw horizontal line for y2
			grphHorizLine(x1[snakeLength-1], x3[0], y1[0]);//draw horizontal line for y1
			grphUpdate(SCRN_TOP+1,SCRN_BOTTOM+1);//update screen with box
			
  			state = 3;
  		}
		
		else if(state==3){
			//CHECK state
			//check if snake head equals any part of its body or wall
			for(i=1; i<snakeLength; i++){
				if(accel==-1){
					if((x1[0]==x3[i]+1 && y2[0]==y2[i]) || x1[0]<=0){
						state = 6;
					}
				}
				else if(accel==1){
					if((x3[0]==x1[i]-1 && y2[0]==y2[i]) || x3[0]>=127){
						state = 6;
					}
				}
				else if(accel==-2){
					if((x2[0]==x2[i] && y1[0]==y3[i]-1) || y1[0]>=61){
						state = 6;
					}
				}
				else if(accel==2){
					if((x2[0]==x2[i] && y3[0]==y1[i]+1) || y3[0]<=0){
						state = 6;
					}
					
				}
			}
			
			//generate random mouse coordinates and draw mouse if previous one was eaten
			if(eaten ==1){
				
				mX3 = generatemX3(generateX,snakeLength, x3);
				mX1=mX3-2;
				mX2=mX3-1;
				mY1 = generatemY1(generateY,snakeLength, y1);
				mY2 = mY1 -1;
				mY3 = mY1 -2;

				grphHorizLine(mX1, mX3, mY3);
				grphHorizLine(mX1, mX3, mY2);
				grphHorizLine(mX1, mX3, mY1);
				grphUpdate(1,62);
				eaten = 0;//set to 0
				generateX=0;
				generateY=0;
			}
			//Get input from accelerometer controller////
			if(get_dir_accel_v2()==1){
				accel_in==1;
			}
			else if(get_dir_accel_v2()==-1){
				accel_in==-1;
			}
			else if(get_dir_accel_v2()==2){
				accel_in==2;
			}
			else if(get_dir_accel_v2()==-2){
				accel_in==-2;
			}
			/////////////////////////////////////////////
			//Get input from joy stick/////////////////
			/**
			if(get_rt_sw_v2()){
				accel_in=1; //joy stick right = right
			}			
			if(get_lt_sw_v2()){
				accel_in=-1;//joy stick left = left
			}
			if(get_down_sw_v2()){
				accel_in=-2;//joy stick down = down
			}
			if(get_up_sw_v2()){
				accel_in=2;//joy stick down = up
			}**/
			////////////////////////////////////////////
			
			if(accel_in!=-accel){
				accel= accel_in; //move direction from accelerometer into variable accel as long as accel_in not in opposite direction
								//and x3[0] and y1[0] is a multiple of 3
			}
	
			
			if(state!=6){
				if((accel==1 && x3[0]==mX1-1 && y2[0]==mY2) || (accel==-2 && x2[0]==mX2 && y1[0]==mY3-1) || (accel==-1 && x1[0]==mX3+1 && y2[0]==mY2) || (accel==2 && x2[0]==mX2 && y3[0]==mY1+1)){
					state = 5; //GROW
				}
				else{
					state = 4; //MOVE
				}
			}
		}
		
		else if(state==4){
			//MOVE state
			//decide direction tail is moving in
			if(y3[snakeLength-1]-y3[snakeLength-2]==0){//tail is moving right/left
				if(x1[snakeLength-1]-x1[snakeLength-2]<0){//tail is moving right
					tailDirection=1;
				}
				else{//tail is moving left
					tailDirection=-1;
				}
			}
			else{//tail is moving up/down
				if(y3[snakeLength-1]-y3[snakeLength-2]<0){//tail is moving down
					tailDirection=-2;
				}
				else{//tail is moving up
					tailDirection=2;
				}
			}
			
			//shift all units of snake including the tail into element after
			for(i=snakeLength-1; i>=0; i--){
				x3[i+1]=x3[i];
				x2[i+1]=x2[i];
				x1[i+1]=x1[i];
				y3[i+1]=y3[i];
				y2[i+1]=y2[i];
				y1[i+1]=y1[i];
			}
			//calculate new head coordinates
			if(accel==-1){ //move left
				//calculate coordinates of new head
				x3[0]= x3[0]-3;
				x2[0]= x2[0]-3;
				x1[0]= x1[0]-3;
			}
			else if(accel==1){//move right
				//calculate coordinates of new head
				x3[0]= x3[0]+3;
				x2[0]= x2[0]+3;
				x1[0]= x1[0]+3;				
			}
			else if(accel==-2){//move down
				y3[0]= y3[0]+3;
				y2[0]= y2[0]+3;
				y1[0]= y1[0]+3;	

			}
			else if(accel==2){//move up
				y3[0]= y3[0]-3;
				y2[0]= y2[0]-3;
				y1[0]= y1[0]-3;

			}
			
			//clear tail and draw head
			while(k<=3){
				grphTailHead(k, accel, tailDirection, x1[0],x2[0],x3[0],x1[snakeLength],x2[snakeLength],x3[snakeLength],y1[0],y2[0],y3[0],y1[snakeLength],y2[snakeLength],y3[snakeLength]);
				grphUpdate(1,60);
				
				if(k!=3){
					cpu_pause(1000/(snakeLength-1)); //don't  pause on the last iteration because it will see cpu_pause at the top
				}
				k++;
			}

			k=1; //reset k value 

			state = 3; //go back to Check state
		}

		else if(state==5){
			//GROW state
			//shift all coordinates over and don't drop tail coordinates. move into next element space
			for(i=snakeLength-1; i>=0; i--){
				x3[i+1]=x3[i];
				x2[i+1]=x2[i];
				x1[i+1]=x1[i];
				y3[i+1]=y3[i];
				y2[i+1]=y2[i];
				y1[i+1]=y1[i];
			}
				
			//make mouse coordinate the new head coordinates
				x3[0]= mX3;
				x2[0]= mX2;
				x1[0]= mX1;
				y3[0]= mY3;
				y2[0]= mY2;
				y1[0]= mY1;
			
			//increase score and snakeLength
			if(score_ones==9){
				score_tens++;
				score_ones=0;
			}
			else{
				score_ones++;
			}
			snakeLength++;
			eaten = 1;
			state = 3;//go back to check state
		}

  		else if(state ==6){
  			//Done state
  			grphErase();
  			grphBox (0,0,127,61);//draw a box around entire screen
  			grphText (2,10,FONT_FIFTEEN_DOT,(unsigned char *)"GAME OVER");//display finish screen
  			grphText (5,35,FONT_SEVEN_DOT,(unsigned char *)"Final Score = ");
  			disp_score[0] = (score_tens+48); //convert score_tens into ASCII character
  			disp_score[1] = (score_ones+48); //convert score_ones into ASCII character
  			grphText(103,35, FONT_SEVEN_DOT,(unsigned char *)disp_score);
  			grphText (20,53,FONT_FIVE_DOT,(unsigned char *)"Press Button 1 to Restart");
  			grphUpdate(SCRN_TOP,SCRN_BOTTOM); //refresh screen
			state = 7;//stay in DONE state until user resets game
  		}
		else if(state==7 && get_SW1_v2()){
			//RESET state
			snakeLength=2;//reset initial snakeLength to 2
			score_tens = 0;//reset score tens place to 0
			score_ones = 0;//reset score ones place to 0
			accel = 1; //reset initial direction to the right
			accel_in = 1;//reset initial direction to the right
			eaten = 1; //reset eaten to 1
			generateX=0;//reset generateX to 0
			generateY=0;//reset generateY to 0
			//reset starting positing of snake to center of screen
			x1[0]= 61;
			x2[0]= 62;
			x3[0]= 63;
			x1[1]= 58;
			x2[1]= 59;
			x3[1]= 60;
			y1[0]= 30;
			y2[0]= 29;
			y3[0]= 28;
			y1[1]= 30;
			y2[1]= 29;
			y3[1]= 28;
			state = 1; //go to INITIAL state when user presses button 1
			
  		}

  	}

	for(;;) {
	   	counter++;
	}
}
