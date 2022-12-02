/*
 * slave.c
 *
 * Created: 24-Nov-22 8:21:42 AM
 *  Author: nasser
 */ 


#include <avr/io.h>
#define F_CPU 8000000UL
#include "ADC_driver.h"
#include <util/delay.h>
#include "LED.h"
#include "spi_driver.h"
#include "macros.h"
#include "std_macros.h"
#include <avr/interrupt.h>
#include "timer.h"

volatile unsigned char temp_set=100;
volatile unsigned char air_flag=0;
volatile unsigned short temp_sensor=0;


int main(void)
{
	LED_vInit('D',cond);
    LED_vInit('D',tv);
	LED_vInit('D',room1);
	LED_vInit('D',room2);
	LED_vInit('D',room3);
	LED_vInit('D',room4);
	LED_vInit('C',0);
	SPI_SlaveInit();
	ADC_vinit();
	
	unsigned char recive=0xff,send=0xff,guest_flag=0,admin_flag=0;
	
while(1)
{
	timer_CTC_init_interrupt();		

	recive=SPI_SlaveReceivechar(send);
	if (recive=='g')
	{
		guest_flag=1;
		admin_flag=0;
	}
	else if (recive=='a')
	{
		admin_flag=1;
		guest_flag=0;
	}
	
	/******************** start guest operations **************************/
		while(guest_flag==1)
		{
		  g_options:
			   recive=SPI_SlaveReceivechar(send);// select room
			// *********  Room1 ********************
			if (recive==1)
			{
				/**** status of room ****/
				if (READ_BIT(PORTD,room1)==1)
				{
					send='T';
				}
				else if (READ_BIT(PORTD,room1)==0)
				{
					send='F';
				}
				recive=SPI_SlaveReceivechar(send);
				
				while(1)
				{
					recive=SPI_SlaveReceivechar(send);
					if (recive=='1')
					{
						LED_vTurnOn('D',room1);
						break;
					}
					else if(recive=='2')
					{
						LED_vTurnOff('D',room1);
						break;
					}
					else if(recive=='0')
					{
						break;
					}
				}
				goto g_options;
				}//end if room1
        							
        		// *********  Room2 ********************
        		if (recive==2)
        		{
	        		/**** status of room ****/
	        		if (READ_BIT(PORTD,room2)==1)
	        		{
		        		send='T';
	        		}
	        		else if (READ_BIT(PORTD,room2)==0)
	        		{
		        		send='F';
	        		}
	        		recive=SPI_SlaveReceivechar(send);
	        							
	        		while(1)
	        		{
		        		recive=SPI_SlaveReceivechar(send);
		        		if (recive=='1')
		        		{
			        		LED_vTurnOn('D',room2);
			        		break;
		        		}
		        		else if(recive=='2')
		        		{
			        		LED_vTurnOff('D',room2);
			        		break;
		        		}
		        		else if(recive=='0')
		        		{
			        		break;
		        		}
	        		}
	        		goto g_options;
        		}//end if room2
        							
        		// *********  Room3 ********************
        		if (recive==3)
        		{
	        		/**** status of room ****/
	        		if (READ_BIT(PORTD,room3)==1)
	        		{
		        		send='T';
	        		}
	        		else if (READ_BIT(PORTD,room3)==0)
	        		{
		        		send='F';
	        		}
	        		recive=SPI_SlaveReceivechar(send);
	        							
	        		while(1)
	        		{
		        		recive=SPI_SlaveReceivechar(send);
		        		if (recive=='1')
		        		{
			        		LED_vTurnOn('D',room3);
			        		break;
		        		}
		        		else if(recive=='2')
		        		{
			        		LED_vTurnOff('D',room3);
			        		break;
		        		}
		        		else if(recive=='0')
		        		{
			        		break;
		        		}
	        		}
	        		goto g_options;
        		}//end if room3
				// *********  Room4 ********************
				if (recive==4)
				{
					/**** status of room ****/
					if (READ_BIT(PORTD,room4)==1)
					{
						send='T';
					}
					else if (READ_BIT(PORTD,room4)==0)
					{
						send='F';
					}
					recive=SPI_SlaveReceivechar(send);
					
					while(1)
					{
						recive=SPI_SlaveReceivechar(send);
						if (recive=='1')
						{
							LED_vTurnOn('D',room4);
							break;
						}
						else if(recive=='2')
						{
							LED_vTurnOff('D',room4);
							break;
						}
						else if(recive=='0')
						{
							break;
						}
					}
					goto g_options;
				}//end if room3
        	
    }//guest mode
	
	/****************** End guest mode *****************/
	
	/******************** start admin operations **************************/
		while(admin_flag==1)
		{
			
			/**************** a_options 1.2.3.4**********************/
		a_options:
/*
		/ *************** check air cond *************** /
		temp_sensor=(ADC_u16Read()*0.25);//check air cond
		if (temp_sensor>=temp_set && air_flag==1)
		{
			LED_vTurnOn('D',cond);
		}
		else
		{
			LED_vTurnOff('D',cond);
		}
		/ *************** End check air cond *********************** /*/
			recive=SPI_SlaveReceivechar(send);// select room
			
			// *********  Room1 ********************
			if (recive==1)
			{
				/**** status of room ****/
				if (READ_BIT(PORTD,room1)==1)
				{
					send='T';
				}
				else if (READ_BIT(PORTD,room1)==0)
				{
					send='F';
				}
				recive=SPI_SlaveReceivechar(send);
				
				while(1)
				{
					recive=SPI_SlaveReceivechar(send);
					if (recive=='1')
					{
						LED_vTurnOn('D',room1);
						break;
					}
					else if(recive=='2')
					{
						LED_vTurnOff('D',room1);
						break;
					}
					else if(recive=='0')
					{
						break;
					}
				}
			goto a_options;
			}//end if room1
							
		// *********  Room2 ********************
		if (recive==2)
		{
			/**** status of room ****/
			if (READ_BIT(PORTD,room2)==1)
			{
				send='T';
			}
			else if (READ_BIT(PORTD,room2)==0)
			{
				send='F';
			}
			recive=SPI_SlaveReceivechar(send);
			
			while(1)
			{
				recive=SPI_SlaveReceivechar(send);
				if (recive=='1')
				{
					LED_vTurnOn('D',room2);
					break;
				}
				else if(recive=='2')
				{
					LED_vTurnOff('D',room2);
					break;
				}
				else if(recive=='0')
				{
					break;
				}
			}
			goto a_options;
		}//end if room2
		
		// *********  Room3 ********************
		if (recive==3)
		{
			/**** status of room ****/
			if (READ_BIT(PORTD,room3)==1)
			{
				send='T';
			}
			else if (READ_BIT(PORTD,room3)==0)
			{
				send='F';
			}
			recive=SPI_SlaveReceivechar(send);
			
			while(1)
			{
				recive=SPI_SlaveReceivechar(send);
				if (recive=='1')
				{
					LED_vTurnOn('D',room3);
					break;
				}
				else if(recive=='2')
				{
					LED_vTurnOff('D',room3);
					break;
				}
				else if(recive=='0')
				{
					break;
				}
			}
			goto a_options;
		}//end if room3
		
		if (recive==4)
		{
		More:

			recive=SPI_SlaveReceivechar(send);
			/************* RET *********************/
			if (recive==8)
			{
				goto a_options;
			}
			// *********  Room 4 ********************
			if (recive==5)
			{
				/**** status of room ****/
				if (READ_BIT(PORTD,room4)==1)
				{
					send='T';
				}
				else if (READ_BIT(PORTD,room4)==0)
				{
					send='F';
				}
				recive=SPI_SlaveReceivechar(send);
				
				while(1)
				{
					recive=SPI_SlaveReceivechar(send);
					if (recive=='1')
					{
						LED_vTurnOn('D',room4);
						break;
					}
					else if(recive=='2')
					{
						LED_vTurnOff('D',room4);
						break;
					}
					else if(recive=='0')
					{
						break;
					}
				}
				goto More;
			}//end if room4
			/***************** TV ***********************/
			if (recive==6)
			{
				/**** status of tv ****/
				if (READ_BIT(PORTD,tv)==1)
				{
					send='T';
				}
				else if (READ_BIT(PORTD,tv)==0)
				{
					send='F';
				}
				recive=SPI_SlaveReceivechar(send);
				
				while(1)
				{
					recive=SPI_SlaveReceivechar(send);
					if (recive=='1')
					{
						LED_vTurnOn('D',tv);
						break;
					}
					else if(recive=='2')
					{
						LED_vTurnOff('D',tv);
						break;
					}
					else if(recive=='0')
					{
						break;
					}
				}
				goto More;
			}//end if tv
			/***************** AIR cond **************/
			if (recive==7)
			{
			AIR:
			recive=SPI_SlaveReceivechar(send);

				/************* RET *********************/
				if (recive=='0')
				{
					goto More;
				}
				/********* set **************************/
				if (recive=='1')
				{
					recive=SPI_SlaveReceivechar(send);
					temp_set=recive;
					goto AIR;
				}
				/************* control ******************/
				if (recive=='2')
				{
					/**** status of air cond ****/
					if (READ_BIT(PORTD,cond)==1)
					{
						send='T';
					}
					else if (READ_BIT(PORTD,cond)==0)
					{
						send='F';
					}
					recive=SPI_SlaveReceivechar(send);
					
					while(1)
					{
						temp_sensor=(ADC_u16Read()*0.25);
						
						recive=SPI_SlaveReceivechar(send);
						if (recive=='1')
						{
							air_flag=1;
							temp_sensor=(ADC_u16Read()*0.25);
							
							if (temp_sensor>=temp_set)
							{
								LED_vTurnOn('D',cond);
							}
							else
							{
								LED_vTurnOff('D',cond);
								
							}
							break;
						}
						else if(recive=='2')
						{
							LED_vTurnOff('D',cond);
							air_flag=0;
							break;
						}
						else if(recive=='0')
						{
							break;
						}
					}
				goto AIR;	
				}//end control
							
			goto More;
			}//end if air
			
		}//end if more 
		
		
	
			
		}//admin mode
	
}	
	
}//main


ISR(TIMER0_COMP_vect)
{
		/*************** check air cond ***************/
		temp_sensor=(ADC_u16Read()*0.25);//check air cond
		if ((temp_sensor>temp_set) && (air_flag==1))
		{
			LED_vTurnOn('D',cond);
		}
		else
		{
			LED_vTurnOff('D',cond);
		}
		/*************** End check air cond ***********************/
}