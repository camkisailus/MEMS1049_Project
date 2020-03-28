

#include <avr/io.h>
#include <avr/interrupt.h>

void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);
void ISR(INT0_vect);
void monitor_pump();
void brew_tea();
void init_ADC();
void cue_finish();

char WATER_TEMP = 0;
char TEA_STRENGTH = 0;


int main(void) {
	// Makes PORTD2 and PORTD3 input interrupt pins
	DDRD = DDRD & 11110011; 
	PORTD = DDRD | 00001100;

	// define all Port C bits as input
	DDRC = 0x00; 

	//PORTB output
	DDRB = DDRB & 00000011;

	EICRA = 1<<ISC11 | 1<<ISC10 | 1<<ISC01 | 0<<ISC00;
	EIMSK = 1<<INT1 | 1<<INT0; 
	sei();
	init_ADC();

	while(1){
		// TODO: while pressure sensor not triggered, do nothing
		// TODO: when pressure sensor triggered, go to ISR
	}
	return 0;

}

void init_ADC()
{
	// Select Vref=AVcc
	ADMUX |= (1<<REFS0);
	//set prescaller to 128 and enable ADC 
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);    

}

ISR(INT0_vect){
	// cup is placed onto pressure sensor
	monitor_pump();
}

void monitor_pump(){
	// turn some yellow LED on
	// monitor the voltage coming from thermocouple
	// when it's close to WATER_TEMP, make LED green and start pump
	// TODO: Need to figure out how to make ADC from ADC1 because ADC0 is being used by brew_tea()
	brew_tea();
}

void brew_tea(){
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; 
	ADCSRA |= (1<<ADSC); // Start conversion
	while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
	TEA_STRENGTH = ADCH;
	int delay = 0;

	if(TEA_STRENGTH<=50){
		// 1 min
		delay = 60000;

	}else if (TEA_STRENGTH<=100){
		// 2 min
		delay = 120000;

	}else if (TEA_STRENGTH<=150){
		// 3 min
		delay = 180000;

	}else if (TEA_STRENGTH<=200){
		// 4 min
		delay = 240000;

	}else{
		// 5 min
		delay = 300000;

	}
	OCR0A = 0x00; 
	TCCR0A = 1<<COM0A1 | 1<<COM0A0 | 1<<WGM01 | 1<<WGM00; 
	TCCR0B = 0<<CS02 | 1<<CS01 | 1<<CS00;

	OCR0A = 0xFF; // set full duty cycle
	PORTB = 1<<PORTB4; // turn on motor
	wait(delay, 2); // wait
	PORTB = 0<<PORTB4; // turn off motor
	OCR0A = 0x00; // clear duty cycle

	cue_finish();

}

void cue_finish(){
	// TODO: Speaker and LED to show tea is done
}

void wait(volatile int multiple, volatile char time_choice){
	while (multiple>0){
		delay_T_msec_timer0(time_choice);
		multiple--;
	}
}

void delay_T_msec_timer0(char choice){
	TCCR0A = 0x00;
	TCNT0 = 0;
	switch (choice){
		case 1:
			TCCR0B = 0b00000010;
			break;

		case 2:
			TCCR0B = 0b00000011;
			break;

		case 3:
			TCCR0B = 0b00000100;
			break;

		case 4:
			TCCR0B = 0b00000101;
			break;

		default:
			TCCR0B = 0b00000001;
			break;

	}
	while (TCNT0 < 0xFA);
	TCCR0B = 0x00;
}

