/*-------------------------------------------------------------------------------------------------
					Technika Mikroprocesorowa 2 - Projekt
					Projekt - Kontroler 
					autor: Michał Teterycz
					wersja: 31.12.2020r.
---------------------------------------------------------------------------------------------------*/
					
#include "MKL05Z4.h"
#include "ADC.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pit.h"
#include "uart.h"
#include "tsi.h"

//------ PRZYCISKI ------
#define BUTTON_1_POS 5	//PTA5
#define BUTTON_2_POS 6	//PTA6
#define BUTTON_3_POS 7	//PTA7

//delay dla przyciskow

void delay_ms( int n) {	//nie uzywam aby szybciej wysylane byly dane do PC / laptopa; mam swiadomosc o dragniu stykow, ale tak szybciej po prostu dziala
volatile int i;
volatile int j;
for( i = 0 ; i < n; i++)
for(j = 0; j < 3500; j++) {}
}

float adc_volt_coeff = ((float)(((float)2.91) / 4095) );			// Współczynnik korekcji wyniku, w stosunku do napięcia referencyjnego przetwornika
uint8_t lewy_ok=0;
uint8_t prawy_ok=0;
uint16_t temp;
float	wynik_lewy;
float	wynik_prawy;
float predkosc;

typedef enum {
	lewy = 3, //PTA8
	prawy = 2 //PTA9
}Pomiar;

 void PIT_IRQHandler()
{
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;		// Skasuj flage zadania przerwania
}

//------------------ ADC ---------------------
Pomiar adc = lewy;

void ADC0_IRQHandler()
{	
	temp = ADC0->R[0];										// Odczyt danej i skasowanie flagi COCO
	if(!lewy_ok && adc == lewy){													// Sprawdź, czy wynik skonsumowany przez pętlę główną
		wynik_lewy=temp;
		adc = prawy;
		lewy_ok = 1;
	} else if(!prawy_ok && adc == prawy){
		wynik_prawy=temp;
		adc = lewy;
		prawy_ok = 1;
	} else {}	
		ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(adc); // Wyzwolenie programowe przetwornika ADC0 w kanale 4 lub 5; kanal lewy to 4 prawy to 5
}
int main (void)
{
//------ PRZYCISKI ------
SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; /* Enable clóck fór GPIO A */
PORTA->PCR[BUTTON_1_POS] |= PORT_PCR_MUX(1); /* Set Pin 1 MUX as GPIO */	
PORTA->PCR[BUTTON_2_POS] |= PORT_PCR_MUX(1);	
PORTA->PCR[BUTTON_3_POS] |= PORT_PCR_MUX(1);
// Enable pull-up resitór ón Pin 1,2,3
PORTA->PCR[BUTTON_1_POS] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
PORTA->PCR[BUTTON_2_POS] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
PORTA->PCR[BUTTON_3_POS] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

//----- TSI Slider -----
TSI_Init (); /* initialize slider */
int x;	//zmienna w ktorej bedzie zapisywana wartosc Slidera
	
	uint8_t	kal_error;
	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	char display2[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	char display3[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};	//do predkosci
	LCD1602_Init();		 					// Inicjalizacja wyświetlacza LCD
	LCD1602_Backlight(TRUE);
	LCD1602_Print("---");				// Ekran kontrolny - nie zniknie, jeśli dalsza część programu nie działa
	LCD1602_ClearAll();
	
	UART_Init(9600);												/* initialize UART */ 
	PIT_Init();
	
	kal_error=ADC_Init();				// Inicjalizacja i kalibracja przetwornika A/C
	if(kal_error)
	{
		while(1);									// Klaibracja się nie powiodła
	}
	
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(lewy);		// Pierwsze wyzwolenie przetwornika ADC0 w kanale 4 i odblokowanie przerwania

	while(1)
	for(int i=0;i<40;i++){	//petla do wyswietlacza LCD, aby nie zacinalo az tak gry
	{
		//----- TSI Slider -----
		x=TSI_ReadSlider();
		sprintf(display,"a,%d,",x);
		UART_Println(display);
		
		//Odczyt danych z potencjometrów i przycisków
		if(lewy_ok)
		{
			wynik_lewy = wynik_lewy*adc_volt_coeff;		// Dostosowanie wyniku do zakresu napięciowego
			sprintf(display,"%.2f,",wynik_lewy);
			UART_Println(display);
			lewy_ok=0;
		}
		if(prawy_ok)
		{
			wynik_prawy = wynik_prawy*adc_volt_coeff;		// Dostosowanie wyniku do zakresu napięciowego
			//__wfi();
			sprintf(display2,"%.2f,",wynik_prawy);
			
			//wyswietlenie predkosci na LCD
			if (i==39){
			predkosc=(wynik_prawy-1.75)*8;
				if(predkosc>0){
					sprintf(display3,"%.2fm/sdo przodu",predkosc);
					LCD1602_SetCursor(0,0);
					LCD1602_Print(display3);
				}
				else{
					predkosc=predkosc*(-1);
					sprintf(display3,"%.2fm/s do tylu ",predkosc);
					LCD1602_SetCursor(0,0);
					LCD1602_Print(display3);
				}
				}
			}
			UART_Println(display2);
			prawy_ok=0;
			
			//------ PRZYCISKI ------
			if( ( PTA->PDIR & (1<<BUTTON_1_POS) ) ==0 ){ /* Test if buttón pressed */
			sprintf(display,"1\r\n\r");	//przycisk 1, musi byc \r\n\r bo inaczej Unity nie przyjmuje danych
			UART_Println(display);
			}

			else if( ( PTA->PDIR & (1<<BUTTON_2_POS) ) ==0 ){ /* Test if buttón pressed */
			sprintf(display,"2\r\n\r"); //przycisk 2
			UART_Println(display);
			}

			else if( ( PTA->PDIR & (1<<BUTTON_3_POS) ) ==0 ){ /* Test if buttón pressed */
			sprintf(display,"3\r\n\r"); //przycisk 3
			UART_Println(display);
			}
			else {
			sprintf(display,"0\r\n\r"); //zaden nie jest wcisniety
			UART_Println(display);
			}
			}	
		}

	}

