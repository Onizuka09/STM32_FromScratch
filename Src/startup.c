#include "../Inc/IRQ_Handlers_Prototypes.h"
#include <stdint.h>
#define STACK_END 0x20000000
#define STACK_POINTER STACK_END
// stm32 F4 NUCLEO

// bound of each section 
extern uint32_t _etext; 
extern uint32_t _sdata; 
extern uint32_t _edata; 
extern uint32_t _sbss; 
extern uint32_t _ebss;


// main prototype
int main(void);

// callback functions declarations
void Reset_Handler(void);
// init vector table
uint32_t vector_table[] __attribute__((section(".isr_vector"))) = {
    STACK_POINTER,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)MemManage_Handler,
    (uint32_t)BusFault_Handler,
    (uint32_t)UsageFault_Handler,
    0, // Rerved
    (uint32_t)SVCall_Handler,
    (uint32_t)DebugMonitor_Handler,
    0,
    (uint32_t)PendSV_Handler,
    (uint32_t)Systick_Handler,
    (uint32_t)WWDG_IRQHandler,
    (uint32_t)PVD_IRQHandler,
    (uint32_t)TAMP_STAMP_IRQHandler,
    (uint32_t)RTC_WKUP_IRQHandler,
    (uint32_t)FLASH_IRQHandler,
    (uint32_t)RCC_IRQHandler,
    (uint32_t)EXTI0_IRQHandler,
    (uint32_t)EXTI1_IRQHandler,
    (uint32_t)EXTI2_IRQHandler,
    (uint32_t)EXTI3_IRQHandler,
    (uint32_t)EXTI4_IRQHandler,
    (uint32_t)DMA1_Stream0_IRQHandler,
    (uint32_t)DMA1_Stream1_IRQHandler,
    (uint32_t)DMA1_Stream2_IRQHandler,
    (uint32_t)DMA1_Stream3_IRQHandler,
    (uint32_t)DMA1_Stream4_IRQHandler,
    (uint32_t)DMA1_Stream5_IRQHandler,
    (uint32_t)DMA1_Stream6_IRQHandler,
    (uint32_t)ADC_IRQHandler,
    (uint32_t)EXTI9_5_IRQHandler,
    (uint32_t)TIM1_BRK_TIM9_IRQHandler,
    (uint32_t)TIM1_UP_TIM10_IRQHandler,
    (uint32_t)TIM1_TRG_COM_TIM11_IRQHandler,
    (uint32_t)TIM1_CC_IRQHandler,
    (uint32_t)TIM2_IRQHandler,
    (uint32_t)TIM3_IRQHandler,
    (uint32_t)TIM4_IRQHandler,
    (uint32_t)I2C1_EV_IRQHandler,
    (uint32_t)I2C1_ER_IRQHandler,
    (uint32_t)I2C2_EV_IRQHandler,
    (uint32_t)I2C2_ER_IRQHandler,
    (uint32_t)SPI1_IRQHandler,
    (uint32_t)SPI2_IRQHandler,
    (uint32_t)USART1_IRQHandler,
    (uint32_t)USART2_IRQHandler,
    (uint32_t)EXTI15_10_IRQHandler,
    (uint32_t)EXTI17_RTC_Alarm_IRQHandler,
    (uint32_t)EXTI18_OTG_FS_WKUP_IRQHandler,
    (uint32_t)TIM8_BRK_TIM12_IRQHandler,
    (uint32_t)DMA1_Stream7_IRQHandler,
    (uint32_t)SDIO_IRQHandler,
    (uint32_t)TIM5_IRQHandler,
    (uint32_t)SPI3_IRQHandler,
    (uint32_t)DMA2_Stream0_IRQHandler,
    (uint32_t)DMA2_Stream1_IRQHandler,
    (uint32_t)DMA2_Stream2_IRQHandler,
    (uint32_t)DMA2_Stream3_IRQHandler,
    (uint32_t)DMA2_Stream4_IRQHandler,
    (uint32_t)OTG_FS_IRQHandler,
    (uint32_t)DMA2_Stream5_IRQHandler,
    (uint32_t)DMA2_Stream6_IRQHandler,
    (uint32_t)DMA2_Stream7_IRQHandler,
    (uint32_t)USART6_IRQHandler,
    (uint32_t)I2C3_EV_IRQHandler,
    (uint32_t)I2C3_ER_IRQHandler,
    (uint32_t)FPU_IRQHandler,
    (uint32_t)SPI3_IRQHandler,

};

void Reset_Handler(void) {
// copy .data section to sram
uint32_t size  = &_edata - &_sdata ; 
uint8_t *pDst = (uint8_t*)&_sdata; //flash
uint8_t *pSrc = (uint8_t*)&_etext;//sram
for (uint32_t i= 0 ; i < size ; i++ ){ 
		*pDst++=*pSrc++; 
}
// init the .bss section to Zero in SRAM 
size = &_ebss - &_sbss ; 
pDst = (uint8_t*)& _sbss; 
for (uint32_t i= 0 ; i < size ; i++ ){ 
 *pDst++ = 0 ;
}
// call main
main();

}
void Default_Handler() {
  while (1); 
} 
