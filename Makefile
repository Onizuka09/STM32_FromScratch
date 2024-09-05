SRC_DIR=./Src
INC_DIR=./Inc
BUILD_DIR=./Build
CC=arm-none-eabi-gcc
MARCH=cortex-m4
CFLAGS= -c -mcpu=$(MARCH) -nostdlib -mthumb -std=gnu11 -Wall -O0 -I${INC_DIR}
LDFLAGS= -mcpu=$(MARCH) -nostdlib -mthumb  -T linker.ld -Wl,-Map=final.map

all: final.elf

${BUILD_DIR}/main.o:${SRC_DIR}/main.c ${INC_DIR}
	$(CC) $(CFLAGS) $< -o $@ 

${BUILD_DIR}/startup.o: ${SRC_DIR}/startup.c 
		$(CC) $(CFLAGS) -o $@ $^


	
final.elf: ${BUILD_DIR}/main.o ${BUILD_DIR}/startup.o
	$(CC) $(LDFLAGS) -o $@ $^
	
flash:final.elf
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program final.elf verify reset exit"


clean:
	rm -rf ${BUILD_DIR}/*.o *.elf final.map


