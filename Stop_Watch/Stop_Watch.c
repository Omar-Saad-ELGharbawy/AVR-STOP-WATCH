#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

enum timer_bits //enum contains location of enable PORTA pins of each 7 segment
{
	SEC1, SEC2, MIN1, MIN2, HOUR1, HOUR2
};

unsigned char sec1 =0, sec2 =0, min1 =0, min2 =0, hour1 =0, hour2 =0; //glopal variables to set value of each timer 7 segment

ISR(INT0_vect)  //Reset stop watch at interrupt 0 response
{
	TCNT1=0;
	sec1=0;
	sec2 =0;
	min1 =0;
	min2 =0;
	hour1 =0;
	hour2 =0;
}

ISR(INT1_vect)  //Pause stop watch by stopping Timer2
{
	TCCR1B&= 0xF8;
}

ISR(INT2_vect) //Resume stop watch by re activating Timer2
{
	TCCR1B |= (1<<CS11) | (1<<CS10);
}

ISR(TIMER1_COMPA_vect) //Timer1 interrupt each second(increment stop watch values)
{
	sec1++;
	if (sec1 == 10)
	{
		sec1=0;
		sec2++;
		if(sec2 == 6)
		{
			sec2=0;
			min1++;
			if (min1 == 10)
			{
				min1=0;
				min2++;
				if (min2 == 6)
				{
					min2=0;
					hour1++;
					if (hour1 == 5)
					{
						hour1=0;
						hour2++;
						if (hour2 == 2)
						{
							hour2=0;
						}
					}
				}
			}
		}
	}
}

void  INT0_Init(void)
{
	DDRD &= (~(1<<PD2));  //Configure PD2/INT0 as External interrupt input pin
	PORTD|= (1<<PD2);  // enable internal pull up resistor
	//	PIND&=~(1<<PD2);  //Initialize INT0(pull up resistor) as OFF

	MCUCR|=(1<<ISC01);  //Adjust interrupt sense control to falling edge
	GICR  |= (1<<INT0);  //Enable Interrupt flag
}

void  INT1_Init(void)
{
	DDRD&=~(1<INT1);  //Configure PD3/INT1 as input interrupt pin

	MCUCR |= (1<<ISC11) | (1<<ISC10); //Adjust Interrupt Sense Control to the rising edge

	GICR  |= (1<<INT1);   // Open Interrupt Flag of INT1
}

void INT2_Init(void)
{
	DDRB&=~(1<<PB2);  //Configure PB2/INT2 as input interrupt pin
	PORTB|=(1<<PB2);  // enable internal pull up resistor
	MCUCSR&=~(1<<ISC2);  //Trigger INT2 by falling edge
	GICR|=(1<<INT2);   //Enable INT2 flag
}

void TIMER1_Init_Compare_Mode_A(void)
{
	TCNT1=0;

	TCCR1A |= (1<<FOC1A) | (1<<FOC1B);  // set FOC1A to close PWM and

	TCCR1B |= (1<<WGM12);               //set WGM12 to select compare mode

	TCCR1B |= (1<<CS11) | (1<<CS10);    //select 64 PRE_SCALER
	OCR1A=15625;  //interrupt ever 15625 counts (1 Second)

	//	TCCR1B |= (1<<CS12) | (1<<CS10);    //select 1024 PRE_SCALER
	//	OCR1A = 976;                        //interrupt ever 976 counts (1 Second)

	TIMSK |= (1<<OCIE1A);  //Enable interrupt for Timer1_A
}

int main(void)
{
	DDRC|=0x0F; 		//Configure pins PC0,PC1,PC2,PC3 as output pins (Decoder for common anode 7 segment)
//	PORTC|=0;           //Initialize pins as TURN ON

	DDRA|=0x3F; 		//Configure pins PA0,PA1,PA2,PA3,PA4,PA5 as output pins for 7 segment enable
	PORTA|=0x3F; 		//Initialize pins as TURN ON

	INT0_Init();
	INT1_Init();
	INT2_Init();
	TIMER1_Init_Compare_Mode_A();

	SREG |=(1<<7); //Enable all interrupts by setting i_bit in SREG

	for(;;)
	{
		PORTA = (PORTA & 0xC0) | (1<<SEC1);
		PORTC = (PORTC & 0xF0) | (sec1 & 0x0F);
		_delay_us(50);
		PORTA = (PORTA & 0xC0) | (1<<SEC2);
		PORTC = (PORTC & 0xF0) | (sec2 & 0x0F);
		_delay_us(50);
		PORTA = (PORTA & 0xC0) | (1<<MIN1);
		PORTC = (PORTC & 0xF0) | (min1 & 0x0F);
		_delay_us(50);
		PORTA = (PORTA & 0xC0) | (1<<MIN2);
		PORTC = (PORTC & 0xF0) | (min2 & 0x0F);
		_delay_us(50);
		PORTA = (PORTA & 0xC0) | (1<<HOUR1);
		PORTC = (PORTC & 0xF0) | (hour1 & 0x0F);
		_delay_us(50);
		PORTA = (PORTA & 0xC0) | (1<<HOUR2);
		PORTC = (PORTC & 0xF0) | (hour2 & 0x0F);
		_delay_us(50);
	}
}
