#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include <time.h>
#include "lcd.h"
#include "uart.h"

#define FREQ 16000000
#define BAUD 9600
#define HIGH 1
#define LOW 0
#define BUFFER 1024
#define BLACK 0x000001

char displayChar = 0;
char words[] = "Hello World";

int l = 0;
int c = 0;
int i = 0;
char buffer[8];

int xx = 0;//get coord
int yy = 0;

int k;
int xxx=0;
int yyy;
int circle=20;
int rightHit = 0;
int leftHit = 0;
char hit[];

int leftPaddleX, regitPaddleX;
int leftPaddleY, rightPaddleY;

int x0=2;
int x0_1=123;
int y0=26;
int y0_1=26;
int x1,y1;
int location = 0;

int paddle_v = 1;

void setup_ADC()
{
	ADMUX = (1 << REFS0);//比较电压5v
	ADCSRA = (1 << ADEN) | ( 1 << ADATE) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);//
	//DIDR0 = (1 << ADC0D); //默认0     ADC prescalar 128
	//DIDR0 = (1 << ADC3D);
	//ADMUX = (1 << MUX1) | (1 << MUX0);
	
	ADCSRB = (1 << ADTS2) | (1 << ADTS1);
}

uint16_t adc_read(uint16_t ch)
{
	ch |= 0x00;
	ADMUX = (ADMUX & 0xF8) | ch;
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	return(ADC);
}

int get_X_Coordinate()
{
	DDRC = 0x00;
	PORTC = 0x00;
	DDRC |= (1 << PORTC1) | (1 << PORTC3) | (0 << PORTC0) | (0 << PORTC2);//x
	PORTC |= (1 << PORTC3);
	
}

int get_Y_Coordinate()
{
	DDRC = 0x00;
	PORTC = 0x00;
	DDRC |= (0 << PORTC1) | (0 << PORTC3) | (1 << PORTC0) | (1 << PORTC2);//y
	PORTC |= (1 << PORTC0);
	
}

int test_input()
{
	l = 0;
	DDRC = 0x00;
	PORTC = 0x00;
	DDRC |= (1 << PORTC1) | (1 << PORTC3) | (0 << PORTC0) | (0 << PORTC2);//test_x
	PORTC |= (1 << PORTC3) | (1 << PORTC0);
	
	//printf("%d\n",adc_read(0));
	xx = adc_read(0);
	
	
	DDRC = 0x00;
	PORTC = 0x00;
	DDRC |= (0 << PORTC1) | (0 << PORTC3) | (1 << PORTC0) | (1 << PORTC2);//test_y
	PORTC |= (1 << PORTC1) | (1 << PORTC0);
	
	//printf("%d\n",adc_read(3));
	_delay_ms(100);
	yy = adc_read(3);
	
	if ((xx < 1000) & (yy < 1000))
	{
		l = 1;
		xx = (xx-175)/6.218+8;
		yy = 64-(yy-290)/8.546+3;
	}
} 

void ballMove()
{
	//srand((unsigned int)time(NULL));
	k = TCNT1 %5 -2;
	printf("%d\n",k);
	if (k == 0)
	{
		k = -1;
	}
	fillcircle(buff,63,31,3,BLACK);         //initial ball
	xxx=63;                                    //ball coord
	yyy=31;
	leftPaddleY = 26;
	rightPaddleY = 26;
	fillrect(buff,124,26,1,8,BLACK);     //initial batch
	fillrect(buff,2,26,1,8,BLACK); 
	int flag = 1;

	while (1)
	{
		interphase();
		clearrec(buff,124,rightPaddleY,1,8,BLACK);
		clearrec(buff,2,leftPaddleY,1,8,BLACK);
		clear_circle(buff,xxx,yyy,3,BLACK);
		test_input();
		x1 = xx;
		y1 = yy;
	    if ((l == 1 )& (x1<63) )//左边
	    {
			    if (y1 < leftPaddleY+4)
			    {
				    leftPaddleY--;
			    }
			    if (y1 > leftPaddleY+4)
			    {
				    leftPaddleY++;
			    }
			    if (leftPaddleY < 3)
			    {
				    leftPaddleY = 2;
			    }
			    if (leftPaddleY >52)
			    {
		    		leftPaddleY = 53;
		     	}
			    fillrect(buff,2,leftPaddleY,1,8,BLACK);
				fillrect(buff,124,rightPaddleY,1,8,BLACK);
		}
	     if ((l == 1 ) & (x1>63) )//右边
		{
				if (y1 <rightPaddleY+4)
				{
					rightPaddleY--;
				}
				if (y1 > rightPaddleY+4)
				{
					rightPaddleY++;
				}
				if (rightPaddleY < 3)
				{
					rightPaddleY = 2;
				}
				if (rightPaddleY >52)
				{
					rightPaddleY = 53;
				}
				fillrect(buff,124,rightPaddleY,1,8,BLACK);
				fillrect(buff,2,leftPaddleY,1,8,BLACK);
			}
		
		    if (l == 0)
		    {
			    fillrect(buff,2,leftPaddleY,1,8,BLACK);
		        fillrect(buff,124,rightPaddleY,1,8,BLACK);
		    }
	
		
		
		if (flag ==1)//判断运动方向 右移
		{
			xxx+=1;
			
			if ((xxx+4>=124)&(yyy>=rightPaddleY-1)&(yyy<= rightPaddleY + 11))//碰到了右边的paddle
			{
				flag = 0;
				//voice();
			}
			if (xxx+4>=128)
			{
				leftHit += 1;
				break;
			}
	
	
		}
		if (flag == 0)  //left move 左移
		{
			xxx-=1;
			
			if ((xxx-4<=2)&(yyy>=leftPaddleY-1)&(yyy<= leftPaddleY + 11))
			{
				flag = 1;
				//voice();
			}
			
			
			if (xxx-4<=0)
			{
				rightHit += 1;
				break;
			}
		}
		
	    write_buffer(buff);
		if (yyy-4 <= 0)
		{
			k = - k;
			
			//voice();
		}
		if (yyy + 4 > 59)
		{
			k = - k;
			
			//voice();
		}
	
		
	    yyy=k+yyy;
		fillcircle(buff,xxx,yyy,3,BLACK);
		 
		write_buffer(buff);
		_delay_ms(400);
		displayChar++;		
	}
	
}

void ballMove_AI()
{
	//srand((unsigned int)time(NULL));
	k = TCNT1 %5 -2;
	printf("%d\n",k);
	if (k == 0)
	{
		k = -1;
	}
	fillcircle(buff,63,31,3,BLACK);         //initial ball
	xxx=63;                                    //ball coord
	yyy=31;
	leftPaddleY = 26;
	rightPaddleY = 26;
	fillrect(buff,124,26,1,8,BLACK);     //initial batch
	fillrect(buff,2,26,1,8,BLACK);
	int flag = 1;

	while (1)
	{
		interphase();
		//paddle();
		//circle -=1;
		clearrec(buff,124,rightPaddleY,1,8,BLACK);
		clear_circle(buff,xxx,yyy,3,BLACK);
		clearrec(buff,2,leftPaddleY,1,8,BLACK);
		
		test_input();
		x1 = xx;
		y1 = yy;
		if (l == 1)
		{
			if (y1 < leftPaddleY+4)
			{
				leftPaddleY--;
			}
			if (y1 > leftPaddleY+4)
			{
				leftPaddleY++;
			}
			if (leftPaddleY < 3)
			{
				leftPaddleY = 2;
			}
			if (leftPaddleY >52)
			{
				leftPaddleY = 53;
			}
			fillrect(buff,2,leftPaddleY,1,8,BLACK);
		}
		if (l == 0)
		{
			fillrect(buff,2,leftPaddleY,1,8,BLACK);
		}
		
		
		if (flag ==1)//判断运动方向 右移
		{
			xxx+=1;
			
			if ((xxx+4>=124))
			{
				//rightHit += 1;
				i ++;
				flag = 0;
				//voice();
			}
		}
		if (flag == 0)  //left move 左移
		{
			xxx-=1;
			
			if ((xxx-4<=2)&(yyy>=leftPaddleY-1)&(yyy<= leftPaddleY + 11))
			{
				flag = 1;
				//voice();
			}
			
			if (xxx-4<=0)
			{
				rightHit += 1;
				i++;
				break;

				//flag = 1;
				//break;
			}
		}
		/*
		if (flag == 2)
		{
			return 0;
		}*/
		
		
		if (yyy-4 <= 0)
		{
			k = - k;
			
			//voice();
		}
		if (yyy+4 > 59)
		{
			k = - k;
			
			//voice();
		}
		
		yyy = k + yyy;
		
		printf("%d\n",yyy);
		
		rightPaddleY = yyy;
		fillrect(buff,124,rightPaddleY,1,8,BLACK);    
		
		fillcircle(buff,xxx,yyy,3,BLACK);
		write_buffer(buff);
		_delay_ms(400);
		displayChar++;
		
	}
	
}
		
		


void ballMove_G()
{
	//srand((unsigned int)time(NULL));
	k = TCNT1 %5 -2;
	//printf("%d\n",k);
	if (k == 0)
	{
		k = -1;
	}
	fillcircle(buff,63,31,3,BLACK);         //initial ball
	xxx=63;                                    //ball coord
	yyy=31;
	leftPaddleY = 26;
	rightPaddleY = 26;
	fillrect(buff,124,26,1,8,BLACK);     //initial batch
	fillrect(buff,2,26,1,8,BLACK);
	int flag = 1;

	while (1)
	{
		interphase();
		//paddle();
		//circle -=1;
		clearrec(buff,124,rightPaddleY,1,8,BLACK);
		clear_circle(buff,xxx,yyy,3,BLACK);
		clearrec(buff,2,leftPaddleY,1,8,BLACK);
		
		DDRC = 0x00;
		PORTC = 0x00;
		DDRC |= (0 << PORTC1) | (0 << PORTC3) | (1 << PORTC0) | (1 << PORTC2);//test_y
		PORTC |= (1 << PORTC1) | (1 << PORTC0);
		_delay_ms(100);
		location = adc_read(4);
		printf("%d\n",location);
		//改成模拟量变化

		if (l == 1)
		{
			if (location < 280)//310
			{
				leftPaddleY--;
			}
			if (location > 300)//320
			{
				leftPaddleY++;
			}
			if (leftPaddleY < 3)
			{
				leftPaddleY = 2;
			}
			if (leftPaddleY >52)
			{
				leftPaddleY = 53;
			}
			fillrect(buff,2,leftPaddleY,1,8,BLACK);
		}
		if (l == 0)
		{
			fillrect(buff,2,leftPaddleY,1,8,BLACK);
		}
		
		
		
		
		if ((yyy + 4 > 59)|(yyy-4 <= 0))
		{
			k = -k;
			//voice();
		}
		
		if (flag ==1)//判断运动方向 右移
		{
			xxx+=1;
			
			if ((xxx+4>=124))
			{
				//rightHit += 1;
				i ++;
				flag = 0;
				//voice();
			}
		}
		if (flag == 0)  //left move 左移
		{
			xxx-=1;
			
			if ((xxx-4<=2)&((yyy>=leftPaddleY-1)&(yyy<= leftPaddleY +11)))
			{
				flag = 1;
				//voice();
			}
			
			if (xxx-4<=0)
			{
				rightHit += 1;
				i++;
				break;
			}
		}
		/*
		if (flag == 2)
		{
			return 0;
		}*/
		
		yyy=k+yyy;
		
		rightPaddleY = yyy;
		fillrect(buff,124,rightPaddleY,1,8,BLACK);    
		
		fillcircle(buff,xxx,yyy,3,BLACK);
		write_buffer(buff);
		_delay_ms(400);
		displayChar++;
		
	}
	
}


int interphase(void)         //every time display the full screen
{
	drawrect(buff,0,0,127,63,BLACK);    // square

	drawline(buff,63,3,63,6,BLACK);	// mid-dash-line
	drawline(buff,63,11,63,14,BLACK);
	drawline(buff,63,19,63,22,BLACK);
	drawline(buff,63,27,63,30,BLACK);
	drawline(buff,63,35,63,38,BLACK);
	drawline(buff,63,43,63,46,BLACK);
	drawline(buff,63,51,63,54,BLACK);
	drawline(buff,63,59,63,62,BLACK);
	//drawrect(buff,70,1,4,5,BLACK);
	//drawrect(buff,52,1,4,5,BLACK);      //init_score
	itoa(leftHit,hit,10);             //from chat to int
	drawstring(buff,51,0,hit);
	itoa(rightHit,hit,10);
	drawstring(buff,66,0,hit);
	//fillrect(buff,2,26,2,10,BLACK);
	//fillrect(buff,123,26,2,10,BLACK);   //init_batch
}

void judgemode()
{
	
	char line1[]="1 V 1";
	char line2[]="1 V PC";
	char line3[]="1 V PCG";
	drawstring(buff,9,2,line1);
	drawstring(buff,9,4,line2);
	drawstring(buff,9,6,line3);     //choose mode
	drawrect(buff,8,14,25,10,BLACK);
	drawrect(buff,8,30,31,10,BLACK);
	drawrect(buff,8,46,36,10,BLACK);//mode square
	test_input();
	if(l==1)
	{
		if (xx>8 && xx<33 && yy>14 && yy<24)
		{
			mode1();
		}
		else if (xx>8 && xx<39 && yy>14 && yy<40)
		{
			mode2();
		}
		else if (xx>8 && xx<44 && yy>14 && yy<56)
		{
			mode3();
		}
	}
}








void mode1()
{
	clear_buffer(buff);
	interphase();
	
	rightHit = 0;
	leftHit = 0;
	
	
	while((rightHit<=5)&(leftHit<=5))
	{
		ballMove();
		flash();
	}
	
	flash();
	
	
	leftHit = 0;
	rightHit = 0;
	l = 0;
}
void mode2()
{
	clear_buffer(buff);
	interphase();
	
	rightHit = 0;
	leftHit = 0;
	
	while((rightHit<=5)&(leftHit<=5))
	{
		ballMove_AI();
		flash();
	}
	
	flash();
	
	leftHit = 0;
	rightHit = 0;
	l = 0;
}
void mode3()
{
	clear_buffer(buff);
	interphase();
	
	rightHit = 0;
	leftHit = 0;
	
	while((rightHit<=5)&(leftHit<=5))
	{
		ballMove_G();
		flash();
	}
	
	flash();
	
	leftHit = 0;
	rightHit = 0;
	l = 0;
}

void voice()
{
	PORTB |= (1 << PORTB1);
	_delay_ms(2000);
	PORTB &= ~(1 << PORTB1);
}

void flash()//int j???
{
	int j;
	PORTB |= (1 << PORTB1);    //make some noise

	drawstring(buff,29,3,"You Win");
	
	for (j=0;j<3;j++)
	{
		PORTB |= (1 << PORTB0);               //display RED winner and background
		_delay_ms(2000);
		OCR1A = 130;
		PORTB &= ~(1 << PORTB0);
		
		PORTB |= (1 << PORTB2);         //  GREEN
		_delay_ms(2000);
		OCR1A = 70;
		PORTB &= ~(1 << PORTB2);
		
		PORTD |= (1 << PORTD7);        //  BLUE
		_delay_ms(2000);
		OCR1A = 130;
		PORTD &= ~(1 << PORTD7);
	}
	
	
	PORTB &= ~(1 << PORTB1);
	clear_buffer(buff);
}

int main(void)
{
	setup_ADC();
	
	lcd_init();
	
	//setting up the gpio for backlight
	DDRD |= 0x80;
	PORTD &= ~0x80;
	PORTD |= 0x00;
	
	DDRB |= 0x05;
	PORTB &= ~0x05;
	PORTB |= 0x00;
	
	uart_init();
	
	//lcd initialisation
	lcd_command(CMD_DISPLAY_ON);
	lcd_set_brightness(0x18);
	write_buffer(buff);
	_delay_ms(300);
	clear_buffer(buff);
	
	//DDRD |= (1 << PORTD6);
	TCCR1A = (1 << WGM11) | (1 << COM1A0);  // | (1 << COM0A0);
	OCR1A = 70;
	TCCR1B = (1 << CS12);                   //TCNT1 >= OCR1A
	

	
	while (1)
	{
		//interphase();
		//judgemode();
		drawstring(buff,22,0,"Welcome to Pong");
		//drawstring(buff,20,4,"Press to continue");
		setpixel(buff,0,0,BLACK);
		setpixel(buff,0,63,BLACK);
		setpixel(buff,127,0,BLACK);
		setpixel(buff,127,63,BLACK);
		
		test_input();
		//ballMove();
		if (l == 1)
		{	
			//mode1();
			
			judgemode();
		}
	
		//_delay_ms(2500);
		
		write_buffer(buff);
		_delay_ms(100);
		displayChar++;
	}
}
