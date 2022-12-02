/*
 * master.c
 *
 * Created: 23-Nov-22 3:25:55 PM
 *  Author: nasser
 */ 


#include <avr/io.h>
#include "LCD.h"
#include "keypad_driver.h"
#define F_CPU 8000000UL
#include <util/delay.h>
#include "EEPROM_driver.h"
#include "macros.h"
#include "spi_driver.h"
#include <avr/interrupt.h>
#include "timer.h"
#include "LED.h"

volatile unsigned int count_time=0;
volatile unsigned int count_time_flag=1;
volatile unsigned int count_time_admin=0;
int main(void)
{	
unsigned char admin[3]={0xff,0xff,0xff,0xff};
unsigned char guest[3]={0xff,0xff,0xff,0xff};
unsigned char i=0;//counter
unsigned char x=0xff;//scan from keypad
unsigned char send,recive;
unsigned char admin_mode=0;
unsigned char guest_mode=0;
unsigned char temp_1digit=0;
unsigned char temp_2digit=0;
unsigned char temp_set=0;
unsigned char temp_sensor=0;
signed char no_try=max_tries;
signed char no_try_ad=max_tries;
signed char block_time=block_time_max;
	
	LCD_vInit();
	keypad_vInit();
	SPI_MasterInit();
	LED_vInit('C',admin_led);
	LED_vInit('C',guest_led);
	LED_vInit('C',block_led);
	

while(1)
{

	/*********************** if still Block time  *****************/ 
	if (EEPROM_read(block_address)==0x00)
	  {
			EEPROM_write(block_address,0xff);
			LED_vTurnOn('C',block_led);
	
			timer_CTC_init_interrupt();
	
			LCD_clearscreen();
			LCD_vSend_string("Block Mode");
 			LCD_movecursor(2,1);
			LCD_vSend_string("Wait ");
			block_time=EEPROM_read(block_address_data);
			while (block_time>0)
			{
				EEPROM_write(block_address_data,block_time);
				LCD_movecursor(2,6);
				LCD_vSend_cmd(0x0c);
				LCD_vSend_char(block_time/10+48);
				LCD_vSend_char(block_time%10+48);
				LCD_vSend_string(" second");
				if (count_time>=100)
				{
					block_time--;
					count_time=0;
				}
		
			}
	
			no_try=max_tries;
			no_try_ad=max_tries;
			block_time=block_time_max;
	
	  } 
/******************************** END block time ***********************/
		LCD_clearscreen();
		LCD_movecursor(1,5);
		LCD_vSend_string("Hello");
		LCD_movecursor(2,2);
		LCD_vSend_string("Welcome Home");
		_delay_ms(500);
		LCD_clearscreen();
		LCD_vSend_cmd(0x0e);
		
/*****************       login first time   **************************/
	if (EEPROM_read(STATUS_ADDRESS)==0xff) 
		{
			LCD_vSend_string("set password");
			LCD_movecursor(2,1);
			LCD_vSend_string("for Admin :");
			for (i=0;i<4;i++)
			{
				do
				{
					x=keypad_u8check_press();
				
				} while (x==0xff);
				LCD_vSend_char(x);
				_delay_ms(250);
				LCD_movecursor(2,12+i);
				LCD_vSend_char('*');
				EEPROM_write(admin_pass_ADDRESS1+i,x);
					
			}
			LCD_clearscreen();
			LCD_vSend_string("pass saved");
			_delay_ms(400);
			LCD_clearscreen();
			LCD_vSend_string("set password");
			LCD_movecursor(2,1);
			LCD_vSend_string("for Guest :");
			for (i=0;i<4;i++)
			{
				x=0xff;
				do
				{
					x=keypad_u8check_press();
				
				} while (x==0xff);
				LCD_vSend_char(x);
				_delay_ms(250);
				LCD_movecursor(2,12+i);
				LCD_vSend_char('*');
				EEPROM_write(guest_pass_ADDRESS1+i,x);
			
			}
		LCD_clearscreen();
		LCD_vSend_string("pass saved");
		_delay_ms(400);
		EEPROM_write(STATUS_ADDRESS,0x01);
	}
/***********************       End log in ****************************/
		
/****************  select mode *****************************************/
 lable1:
	count_time_flag=1;
	if (EEPROM_read(STATUS_ADDRESS)==0x01)
		{
			LCD_clearscreen();
			LCD_vSend_string("select mode");
			LCD_movecursor(2,1);
			LCD_vSend_string("0:Admin  1:Guest");
		}
		x=0xff;
		do 
		{
			x=keypad_u8check_press();
		} while (x==0xff);
		_delay_ms(250);
		LCD_clearscreen();
/*********************  check password for admin  ***********************/
		if (x=='0') 
		{
			LCD_vSend_string("check pass");
			LCD_movecursor(2,12);
		
			for (i=0;i<4;i++)
			{
				do
				{
					admin[i]=keypad_u8check_press();
				
				} while (admin[i]==0xff);
				LCD_vSend_char(admin[i]);
				_delay_ms(250);
				LCD_movecursor(2,12+i);
				LCD_vSend_char('*');
			}		
			if (EEPROM_read(admin_pass_ADDRESS1)==admin[0] && EEPROM_read(admin_pass_ADDRESS2)==admin[1] && EEPROM_read(admin_pass_ADDRESS3)==admin[2] && EEPROM_read(admin_pass_ADDRESS4)==admin[3])
				{
					
				LCD_clearscreen();
				LCD_vSend_string("correct pass");
				_delay_ms(400);
				LED_vTurnOn('C',admin_led);
				admin_mode=1;
				/******** start time out for admin *********/
				timer_CTC_init_interrupt();
					
				}
			
			else
			{
				LCD_clearscreen();
				LCD_vSend_string("wrong pass");
				_delay_ms(200);
				if (no_try_ad>0)
				{
					no_try_ad--;
					LCD_movecursor(2,1);
					LCD_vSend_string("left tries ");
					LCD_vSend_char(no_try_ad+49);
					_delay_ms(500);
					goto lable1;
				}
				else if(no_try_ad<=0)
				{
					timer_CTC_init_interrupt();
					LCD_clearscreen();
					LCD_vSend_string("Block Mode");
					LCD_movecursor(2,1);
					LCD_vSend_string("Wait ");
					EEPROM_write(block_address,0x00);
					while (block_time>0)
					{
						LED_vTurnOn('C',block_led);
						EEPROM_write(block_address_data,block_time);
						LCD_movecursor(2,6);
						LCD_vSend_cmd(0x0c);
						LCD_vSend_char(block_time/10+48);
						LCD_vSend_char(block_time%10+48);
						LCD_vSend_string(" second");
						if (count_time>=100)
						{
							block_time--;
							count_time=0;
						}
					}
				    LCD_vSend_cmd(0x0e);
					no_try_ad=max_tries;
					block_time=block_time_max;
					EEPROM_write(block_address,0xff);
					goto lable1;
				}
			}
				
		}
/*******************  check password for guest  ***********************/
		else if (x=='1')
		{
			LCD_vSend_string("check pass");
			LCD_movecursor(2,12);
		
			for (i=0;i<4;i++)
			{
				do
				{
					guest[i]=keypad_u8check_press();
				
				} while (guest[i]==0xff);
				LCD_vSend_char(guest[i]);
				_delay_ms(250);
				LCD_movecursor(2,12+i);
				LCD_vSend_char('*');
			}
			if (EEPROM_read(guest_pass_ADDRESS1)==guest[0] && EEPROM_read(guest_pass_ADDRESS2)==guest[1] && EEPROM_read(guest_pass_ADDRESS3)==guest[2] && EEPROM_read(guest_pass_ADDRESS4)==guest[3])
			{
				LCD_clearscreen();
				LCD_vSend_string("correct pass");
				_delay_ms(400);
				LED_vTurnOn('C',guest_led);
				guest_mode=1;
				/******** start time out for guest *********/
				timer_CTC_init_interrupt();
				
			}
			else
			{
				LCD_clearscreen();
				LCD_vSend_string("wrong pass");
				_delay_ms(200);
				if (no_try>0)
				{
					no_try--;
					LCD_movecursor(2,1);
					LCD_vSend_string("left tries ");
					LCD_vSend_char(no_try+49);
					_delay_ms(500);
					goto lable1;
				}
				else if(no_try<=0)
				{
					timer_CTC_init_interrupt();
					LCD_clearscreen();
					LCD_vSend_string("Block Mode");
					LCD_movecursor(2,1);
					LCD_vSend_string("Wait ");
					EEPROM_write(block_address,0x00);
					while (block_time>0)
					{
						LED_vTurnOn('C',block_led);
						EEPROM_write(block_address_data,block_time);
						LCD_movecursor(2,6);
						LCD_vSend_cmd(0x0c);
						LCD_vSend_char(block_time/10+48);
						LCD_vSend_char(block_time%10+48);
						LCD_vSend_string(" second");
						if (count_time>=100)
						{
							block_time--;
							count_time=0;
						}
					}
					LCD_vSend_cmd(0x0e);
					no_try=max_tries;
					block_time=block_time_max;
					EEPROM_write(block_address,0xff);
					goto lable1;
				}
			}
			
		}
		else
		{
			LCD_vSend_string("wrong choice");
			LCD_movecursor(2,1);
			_delay_ms(300);
			goto lable1;
		}
/*********************** End check ********************/

/*************** guest mode operation **************/
	if (guest_mode==1)
	{
		count_time_flag=2;
		admin_mode=0;
		while(count_time_flag==2)
		{
		
			send='g';
			recive=SPI_MasterTransmitchar(send);
		g_options:
			LCD_clearscreen();
			LCD_vSend_string("1:Room1  2:Room2");
			LCD_movecursor(2,1);
			LCD_vSend_string("3:Room3  4:Room4");
			

			x=0xff;
			do 
			{
				x=keypad_u8check_press();
				
				if (count_time_flag==guest_time_flag) // **** check session time out
				{
					guest_mode=0;
					count_time_flag=1;
					LCD_clearscreen();
					LCD_vSend_string("session time out");
					_delay_ms(500);
					LED_vTurnOff('C',guest_led);
					break;
					
				}
			} while (x==0xff);
			_delay_ms(150);
			LCD_clearscreen();
			
			/**************** Room 1 select ******************/
			if (x=='1')
			{
				send=1;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room1 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					
				}
				goto g_options;
			}//end if room1

			/**************** Room 2 select ******************/
			if (x=='2')
			{
				send=2;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room2 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					
				}
				goto g_options;
			}//end if room2
			/**************** Room 3 select ******************/
			if (x=='3')
			{
				send=3;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room3 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					
				}
				goto g_options;
			}//end if room3
		
			/**************** Room 4 select ******************/
			if (x=='4')
			{
				send=4;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room4 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					
				}
				goto g_options;
			}//end if room4

	else if (x!=0xff)
	{
		LCD_vSend_string("wrong choice");
		_delay_ms(250);
		goto g_options;
	}
	
	
	
	  }//while  count time flag=2
	  
		
	goto lable1;	
		
		}//guest mode
	
	
/************************ End guest mode operation *************************************************/
/*************************** Admin mode operations ********************/		

	if (admin_mode==1)
		{
			count_time_flag=3;
			guest_mode =0;
			while(count_time_flag==3)
			{
				
				send='a';
				recive=SPI_MasterTransmitchar(send);
			
		/************************a_options(1.2.3.4)***********************************/	
		a_options:
			LCD_clearscreen();
			LCD_vSend_string("1:Room1  2:Room2");
			LCD_movecursor(2,1);
			LCD_vSend_string("3:Room3  4:More");
			
			x=0xff;
			do
			{
				x=keypad_u8check_press();
				
				if (count_time_flag==admin_time_flag) // **** check session time out
				{
					admin_mode=0;
					count_time_flag=1;
					LCD_clearscreen();
					LCD_vSend_string("session time out");
					_delay_ms(500);
					LED_vTurnOff('C',admin_led);
					break;
					
				}
			} while (x==0xff);
			_delay_ms(150);
			LCD_clearscreen();
			
			/**************** Room 1 select ******************/
			if (x=='1')
			{
				send=1;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room1 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					
				}
			goto a_options;	
			}//end if room1
			
			/**************** Room 2 select ******************/
			if (x=='2')
			{
				send=2;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room2 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					
				}
			goto a_options;		
			}//end if room2
			
			/**************** Room 3 select ******************/
			if (x=='3')
			{
				send=3;
				recive=SPI_MasterTransmitchar(send);
				LCD_vSend_string("Room3 :S");
				/******* room status****/
				recive=SPI_MasterTransmitchar(send);
				if (recive == 'T')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("ON");
				}
				else if (recive == 'F')
				{
					LCD_movecursor(1,10);
					LCD_vSend_string("OFF");
				}
				LCD_movecursor(2,1);
				LCD_vSend_string("1-ON 2-OFF 0-RET");
				while(1)
				{
					x=0xff;
					do
					{
						x=keypad_u8check_press();
					} while (x==0xff);
					_delay_ms(150);
					
					if (x=='0')
					{
						send=SPI_MasterTransmitchar(x);
						break;
					}
					else if (x=='1')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					else if (x=='2')
					{
						send=SPI_MasterTransmitchar(x);
						
						break;
					}
					
				}
				goto a_options;
			}//end if room3
			
			/************************* More selsct **********************/
		if (x=='4')//more 
			{
				send=4;
				recive=SPI_MasterTransmitchar(send);
			More:
				LCD_clearscreen();
				LCD_vSend_string("5-Room4  6-TV");
				LCD_movecursor(2,1);
				LCD_vSend_string("7-Air cond 8-RET");
				
				
				x=0xff;
				do
				{
					x=keypad_u8check_press();
					
					if (count_time_flag==admin_time_flag) // **** check session time out
					{
						admin_mode=0;
						count_time_flag=1;
						LCD_clearscreen();
						LCD_vSend_string("session time out");
						_delay_ms(500);
						LED_vTurnOff('C',admin_led);
						break;
					}
				} while (x==0xff);
				_delay_ms(150);
				LCD_clearscreen();
				/************** RET ********************/
				if (x=='8')
				{
					send=8;
					recive=SPI_MasterTransmitchar(send);
					goto a_options;
				}
				/**************** Room 4 select ******************/
				if (x=='5')
				{
					send=5;
					recive=SPI_MasterTransmitchar(send);
					LCD_vSend_string("Room4 :S");
					/******* room status****/
					recive=SPI_MasterTransmitchar(send);
					if (recive == 'T')
					{
						LCD_movecursor(1,10);
						LCD_vSend_string("ON");
					}
					else if (recive == 'F')
					{
						LCD_movecursor(1,10);
						LCD_vSend_string("OFF");
					}
					LCD_movecursor(2,1);
					LCD_vSend_string("1-ON 2-OFF 0-RET");
					while(1)
					{
						x=0xff;
						do
						{
							x=keypad_u8check_press();
						} while (x==0xff);
						_delay_ms(150);
						
						if (x=='0')
						{
							send=SPI_MasterTransmitchar(x);
							break;
						}
						else if (x=='1')
						{
							send=SPI_MasterTransmitchar(x);
							
							break;
						}
						else if (x=='2')
						{
							send=SPI_MasterTransmitchar(x);
							
							break;
						}
						
					}
					goto More;
				}//end if room4
				
				/**************** TV select ******************/
				if (x=='6')
				{
					send=6;
					recive=SPI_MasterTransmitchar(send);
					LCD_vSend_string("TV :S");
					/******* TV status****/
					recive=SPI_MasterTransmitchar(send);
					if (recive == 'T')
					{
						LCD_movecursor(1,7);
						LCD_vSend_string("ON ");
					}
					else if (recive == 'F')
					{
						LCD_movecursor(1,7);
						LCD_vSend_string("OFF");
					}
					LCD_movecursor(2,1);
					LCD_vSend_string("1-ON 2-OFF 0-RET");
					while(1)
					{
						x=0xff;
						do
						{
							x=keypad_u8check_press();
						} while (x==0xff);
						_delay_ms(150);
						
						if (x=='0')
						{
							send=SPI_MasterTransmitchar(x);
							break;
						}
						else if (x=='1')
						{
							send=SPI_MasterTransmitchar(x);
							
							break;
						}
						else if (x=='2')
						{
							send=SPI_MasterTransmitchar(x);
							
							break;
						}
						
					}
					goto More;
				}//end if tv
				/******************** AIR select ******************/
			if (x=='7')
				{
					
					send=7;
					recive=SPI_MasterTransmitchar(send);
				AIR:
					LCD_clearscreen();
					LCD_vSend_string("1:set Air Temp");
					LCD_movecursor(2,1);
					LCD_vSend_string("2:Control 0:RET");
						x=0xff;
						do
						{
							x=keypad_u8check_press();
						} while (x==0xff);
						_delay_ms(150);
						/****** ret ********************/
						if (x=='0')
						{
							send='0';
							recive=SPI_MasterTransmitchar(send);
							goto More;
						}
						/****** set temp **********************/
						if (x=='1')//set temp
						{
							send='1';
							recive=SPI_MasterTransmitchar(send);
							LCD_clearscreen();
							LCD_vSend_string("Temperature= --");
							//scan first digit
							x=0xff;
							do
							{
								x=keypad_u8check_press();
							} while (x==0xff);
							_delay_ms(250);
							temp_1digit=x-48;
							LCD_movecursor(1,14);
							LCD_vSend_char(x);
							
							//scan second digit
							x=0xff;
							do
							{
								x=keypad_u8check_press();
							} while (x==0xff);
							_delay_ms(150);
							temp_2digit=x-48;
							LCD_movecursor(1,15);
							LCD_vSend_char(x);
							_delay_ms(500);
							temp_set= (temp_1digit*10)+temp_2digit;
							
							send=temp_set;
							recive=SPI_MasterTransmitchar(send);
							
							LCD_clearscreen();
							LCD_vSend_string("Temp Set");
							_delay_ms(500);
							LCD_clearscreen();
							goto AIR;
						}//end if set temp
						
						/************ control ***********/
						if (x=='2')//control
						{
							send='2';
							recive=SPI_MasterTransmitchar(send);
							LCD_clearscreen();
							LCD_vSend_string("AIR cond :S");
							/******* AIR status****/
							recive=SPI_MasterTransmitchar(send);
							if (recive == 'T')
							{
								LCD_movecursor(1,13);
								LCD_vSend_string("ON ");
							}
							else if (recive == 'F')
							{
								LCD_movecursor(1,13);
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-ON 2-OFF 0-RET");
							while(1)
							{
								x=0xff;
								do
								{
									x=keypad_u8check_press();
								} while (x==0xff);
								_delay_ms(150);
								
								if (x=='0')
								{
									send=SPI_MasterTransmitchar(x);
									break;
								}
								else if (x=='1')
								{
									send=SPI_MasterTransmitchar(x);
									
									break;
								}
								else if (x=='2')
								{
									send=SPI_MasterTransmitchar(x);
									
									break;
								}
								
							}
							goto AIR;
						}//end if control
				
				goto More;
				}//end if air 

			else if (x!=0xff)
			{
				LCD_vSend_string("wrong choice");
				_delay_ms(250);
				goto More;
			}

			}//end if more
				
		else if (x!=0xff)
		{
			LCD_vSend_string("wrong choice");
			_delay_ms(250);
			goto a_options;
		}
		
		
		}//while  count time flag=3
			
			
			
	}//end if admin mode
		
	
/************************ End admin mode operation ********************/			

}
}//main

ISR(TIMER0_COMP_vect)
{
	count_time++;//for block time and guest time 
	count_time_admin++;// for admin time
	
	if (count_time >= guest_time_max*100)
		{
			count_time=0;
			count_time_flag= guest_time_flag;
			LED_vTurnOff('C',guest_led);
		}
	if (count_time_admin >= admin_time_max*100)
	{
	
	count_time_admin=0;
	count_time_flag=admin_time_flag;
	
	}
}