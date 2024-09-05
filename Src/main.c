
#include "../Inc/main.h"
//led port
volatile int a =0 ; 
volatile int b = 0; 
//led pin
int main ()
{
	//  enable clock
	RCC_AHB1_EN_R |= GPIOA_en ;
	//set pin5 to output mode
	GPIOA_MODER_R |= (1U<< 10);
	GPIOA_MODER_R &= ~(1U<< 11);

	while (1)
	{
	//Set pin 	1
//		GPIOA_ODR_R |= LED_pin;
		// toggle pin
		/* GPIOA_ODR_R ^= LED_pin; */

		/* GPIOA_ODR_R &= ~LED_pin; */
		/* for ( int i=0;i<1000000;i++) */
    /* { */
      /* a++; */ 
    /* } */
    GPIOA_ODR_R |= LED_pin ;
	  /* b=a ; */ 
  }
	return 0;
}
