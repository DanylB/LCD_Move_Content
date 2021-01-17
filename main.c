#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define E1 PORTB |= (1<<3) // вкл чтение/запись сигнала
#define E0 PORTB &= ~(1<<3) // выкл чтение/запись сигнала

#define RS1 PORTB |= (1<<2) // Передаем данные
#define RS0 PORTB &= ~(1<<2) // Передаем команду

unsigned char horizontal=1, vertical=0;

void send_half_byte(unsigned char my_byte){
	my_byte<<=4;
	E1;
	_delay_us(50);
	PORTB&= 0b00001111; //очистить информацию на информационных входах
	PORTB|= my_byte;
	E0;
	_delay_us(40); // ждем пока МК дисплея запишет данные
}

void send_byte(unsigned char my_byte, unsigned char mode){

	mode==0?(RS0):(RS1);
	
	unsigned char high_char = my_byte>>4;
	
	send_half_byte(high_char);
	send_half_byte(my_byte);
}

void send_data(unsigned char my_byte){
	send_byte(my_byte, 1); // 1 - передаем данные
}

void set_pos(unsigned char x,unsigned char y){
	char adress;
	adress=(0x40*y+x)|0b10000000;
	send_byte(adress, 0); // 0 - передаем команду
}

void LCD_ini(){
	/* Инициализация по Даташиту */
	
	_delay_ms(15);
	send_half_byte(0b00000011);
	_delay_ms(4);
	send_half_byte(0b00000011);
	_delay_us(100);
	send_half_byte(0b00000011);
	_delay_ms(1);
	send_half_byte(0b00000010);
	_delay_ms(1);
	
	send_byte(0b00101000,0); // вкл 4 биный режим, 2 линии (N=1)
	_delay_ms(1);
	send_byte(0b00001100,0); // вкл Дисплей(D=1), выкл курсор(C,B=0)
	_delay_ms(1);
	send_byte(0b00000110,0); // текст/курсор  ltr (I/D=1) / rtl(I/D=0)
	_delay_ms(1);

}

void str_out(char my_str[]){
	wchar_t n;
	
	for(n=0; my_str[n]!='\0'; n++)
	send_data(my_str[n]);
}

ISR(INT0_vect){
send_byte(0b00000001,0); // Clear Screen
_delay_ms(50);

vertical+=1;
set_pos(horizontal,vertical);
str_out("Hello");
}

ISR(INT1_vect){
	send_byte(0b00000001,0); // Clear Screen
	_delay_ms(10);
	
	if(horizontal>=10) horizontal=1;
	else horizontal+=2; 
	
	set_pos(horizontal,vertical);
	str_out("Hello");
	
}

int main(void)
{
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRD = 0x00;
	PORTD = 0xFF;
	
	GICR = 0b11000000; // Подкл. внешние прерывания
	MCUSR = 0b00001010;
	sei();

	LCD_ini();
	
	set_pos(horizontal,vertical);
	str_out("Hello");
	
	while(1){
		
	}
}

