#include "MKL05Z4.h"
#include	"pit.h"



void PIT_Init(void)
{
	uint32_t tsv;
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;			// Włączenie sygnału zegara do modułu PIT
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;				// Włączenie modułu PIT
	//tsv=BUS_CLOCK;												// Przerwanie co 1s
	//tsv=BUS_CLOCK/10;										// Przerwanie co 100ms
	//tsv=BUS_CLOCK/2;										// Przerwanie co 500ms
	tsv=BUS_CLOCK/100;									// Przerwanie co 10ms
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(tsv);		// Załadowanie wartości startowej
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;		// Odblokowanie przerwania i wystartowanie licznika
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);	
}
