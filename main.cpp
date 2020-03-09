

#include <avr/io.h>
#include <avr/interrupt.h>

void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);
void ISR(INT0_vect);
void read_poteniometers();
void monitor_pump();

int WATER_TEMP = 0;
int TEA_STRENGTH = 0;


int main(void) {
	// Makes PORTD2 and PORTD3 input interrupt pins
	DDRD = DDRD & 11110011; 
	PORTD = DDRD | 00001100;

	// define all Port C bits as input
	DDRC = 0x00; 

	// clear Power Reduction ADC bit (0) in PRR register
	PRR = 0x00; 

	// Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2‐0 of ADCSRA = ADPS2‐ADPS0 = 111)
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;

	// select Analog Reference voltage to be AVcc (bits 7‐6 of ADMUX = 01)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; 

	EICRA = 1<<ISC11 | 1<<ISC10 | 1<<ISC01 | 0<<ISC00;
	EIMSK = 1<<INT1 | 1<<INT0; 
	sei();

	while(1){


	}



	return 0;

}

ISR(INT0_vect){
	// cup is placed onto pressure sensor
	read_poteniometers();
}

void read_poteniometers(){
	ADCSRA |= (1<<ADSC); // Start conversion
	while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
	WATER_TEMP = ADCH;
	monitor_pump();


}

void monitor_pump(){
	// turn some yellow LED on
	// monitor the voltage coming from thermocouple
	// when it's close to WATER_TEMP, make LED green and start pump

	brew_tea();
}

void brew_tea(){
	OCR0A = 0x00; // Load $00 into OCR0 to set initial duty cycle to 0 (motor off)
	TCCR0A = 1<<COM0A1 | 1<<COM0A0 | 1<<WGM01 | 1<<WGM00; // Set non‐inverting
		//mode on OC0A pin (COMA1:0 = 10; Fast PWM (WGM1:0 bits = bits 1:0 = 11) (Note
		//that we are not affecting OC0B because COMB0:1 bits stay at default = 00)
	TCCR0B = 0<<CS02 | 1<<CS01 | 1<<CS00; // Set base PWM frequency (CS02:0 ‐ bits
		//2‐0 = 011 for prescaler of 64, for approximately 1kHz base frequency)
		// PWM is now running on selected pin at selected base frequency. Duty cycle is
		//set by loading/changing value in OCR0A register.

}

void wait(int multiple){
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

