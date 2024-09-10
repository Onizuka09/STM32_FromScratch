# Article 1: Development environment from Scratch for an STM32
Have you ever wondered when you are woking on an Arduino project how does everything works, why sometimes it takes so long to flash or compile the code..., Or when you work an ST project on the CUBE-IDE and you hit run or Debug what is happening on the backend, Or you are just fed up with using a bloated IDE<br>

well, let's figure out to program an STM32 and set up its environment from scratch.<br> 

I am using a `STM32F4RET6-NCLEO` board, it has a CORTEX-M4 micro-processor and 00 FLASH and 00 SRAM.<br>
This article will explore how to set up a development environment from scratch, starting with a bare `main.c` file as our main application (a blinky sketch).<br>
A development should contains these elements : 
- `code editor`: a piece of software to write your code. 
- `toolchain` : tools needed to compile, assemble ...
-`Startup file`: describes the Request handler that the MCU calls when an IT happens, describes the VT and implements the `Reset_handler`. 
- `linker script`:  describes you memory layout and how to set up different sections<br>
In this article i will focus how to setup an Development environment from scratch without the need of an IDE.<br> 
I will assume that you are using linux "ubuntu" as your operating system.<br> 
### An IDE VS Code Editor

### Embedded system code structure 
if you worked on the STM32-Cube IDE, you would find this structure :
`Src dir`: contains all the source files `.c` (`main.c`) 
`Includes dir`: contains all the headers `.h`
`Debug dir`: contains the build output  
`Startup dir`: contains the start up file 
a `.ld` file extension: linker script 

There are other directories like `driver` and `core`, i am not going to focus on them for the moment.<br>
i have already created the simple main.c file: a blinky application ( we will explore it later ).
Now How to create a linker script and a startup file for an embedded target. 
#### linker script 

The linker script is a piece of code that describes the memory layout of your project
It describes the physical memory size (Flash and SRAM), also it describes the sections present in your code.<br>

##### 1- First let's understand the memory architecture of an embedded system:
* `Flash`       :   this memory contains the binary code you uploaded and it's going to run (volatile). 
* `SRAM`        :   this is the ram that would hold your data at runtime (non-volatile)

###### - The anatomy of `flash` and `SRAM`:<br>
**- Let's first start with flash:**<br>
As said before the `flash` holds the application binary code, it is divided into sections: 
- `.text` section   : contains the application code. 
- `.data` section   : contains the (global/static) initialized data 
- `.rodata` section : contains the constant data. 
- `.bss` section    : contains the (global/static) uninitialized data. 

**- The `SRAM` (static memory):**<br>
The `SRAM` holds the data at runtime: 
- `.data` section   : same as the `flash`, the `.data` of the `flash` are copied to `.data` section of `SRAM` at boot-time.   
- `.bss` section    : same as the `flash`, the `.bss` of the `flash` are copied and initialized to `0` to `.bss` section of `SRAM` at boot-time.   
- `stack` section   : contains the local data that are statically allocated and function calls at runtime. 
- `heap` section    : contains dynamic allocated memory regions, at runtime. 
###### - Other memory regions: 
* `Peripherals`   :   a memory area, describes where the MCU peripherals are mapped, it is used to control the peripherals through registers.   
<!--- TODO: Look and understand more about the other types of memory in Embedded systems --->


##### 2- The memory mapping in ST: 
In a PC you have in the mother-board, CPU, DISK and MEMORY stick, each one in dedicated area and connected through each other via buses.<br>
In an STM32 (STM32F4) there is a `4Gb` of addressable memory space, this memory area is divided into sections, each section has a defined boundary address space (start and end), these sections represents the different memory component of an embedded target.<br> 
This space is not a physical memory block but rather a way of organizing how different types of physical memory and peripherals are accessed by the processor.<br>
This information is present in the `Memory mapping section in the DATASHEET`. 
TODO: Insert Picture <br>

This means to access `SRAM` or `FLASH`, they are mapped to a specific location in this memory space.<br> 
These mappings are not standard and changes from MCU to MCU, for that we need to specify the `location` of each region, it's `size` and it's `contents` (`sections`).<br>
this is where the `linker` comes to play. 
##### 3- linker and linker script
The `linker` is responsible for assigning memory addresses to `.text` and `.data` sections of a program, ensuring that they are correctly placed in the appropriate memory regions as defined by the microcontroller’s (MCU's) memory map. This process is guided by a `linker script`.

A `linker script` is a text file that details how different sections of an `object file` should be combined to create the final output file. It specifies unique absolute addresses for these sections based on the address information provided within the script.

- Key aspects of a linker script include:

* Memory Layout             :   Describes how memory is organized and assigns addresses to different sections.
* Code and Data Addresses   :   Defines where code and data should be placed in memory and their sizes.

`Linker scripts` are written using GNU linker command language and have the `.ld` file extension.

During the linking phase, you must supply the linker script to the linker using the `-T` option.
The linker performs additional roles, which will be discussed later.

##### 4- How to write a linker script: 
writing a linker script is very easy. 

- This is a basic template how a linker script should appear. 
```ld 
ENTRY (_symbol_name_)

MEMORY {
memory-name (att) :ORIGIN = adr,LENGTH = size
}
SECTIONS {
    .section_name:{
     *(.section_name)     
    }>(VMA) AT> (LMA)
}
```
###### **- ENTRY():** 
This command sets the `entry-point address` information in the header `.elf` file (final executable) to `_symbol_name_`.<br>
Usually the `_symbol_name_` refers to the first function to be executed by the CPU, in ST this function is called `Reset_Handler`.<br>
- let's replace the `ENTRY` function with our suitable Handler: 
```ld 
ENTRY(Reset_Handler)
```
###### **- MEMORY section**:
This command allows you to describe the different `memories` present in the target and their `start address` and `size information`.<br>
This information also helps the `linker` to calculate total code and memory consumed so far and throw error if data, code, heap or stack areas cannot fit into the available sizes<br>
```ld
memory-name (perm) :ORIGIN = adr,LENGTH = size
``` 
- `memory-name` :   defines a name of the memory region which will be later referenced by other parts of the `linker script`.
- `adr`         :   defines origin address, start of the memory region
- `size`        :   defines length of information 
- `att`         :   defines the attribute list of the memory region (actions performed Read/write access ...). 

- Let's write the `MEMORY` command based on the `STM32F4RET6`:<br> 
This board has: 
- 512K of `Flash`
- 96K of `SRAM` 

if you have other `SRAM` regions you can merge them together or declare them separately. 
```ld
MEMORY{
  FLASH (rx) :ORIGIN =0x08000000,LENGTH =512K
  SRAM (rwx) :ORIGIN =0x20000000,LENGTH =96K 
}
```
- `RX`: in flash means that, its a Read only memory region and contains executable code. 
- `W`: means its readable and writable. 
this ensures that code resides in a protected memory region and that data can be modified as needed.

###### **- SECTIONS section:** 

This section is responsible for creating the output sections present in the final executable. by merging all of the input section.<br>
lets say we have `.c` file, let's compile it using `-c` to obtain `.o` file representing the binary format. 
```bash
gcc -c main.c  -o main.o 
```
Notice here, we did not command the `gcc` to `link`, we just `compiled` and `translated` the `.c` file into `binary` format.<br>
> we will explore this in a later chapter. 

Imagin now we have multiple `.o` files. Each `binary` file has section like `.text`, `.data`. <br>
In the `SECTIONS` we telling the linker to combine all of the `.text` section together, etc.. <br>
```ld
SECTIONS{
/* here we are instructing the linker to merge all the .text in a section  called .text*/
    .text{ 
        *(.text)
        /* onther way is to specify the the files we want to get the .text section from */
        main.o file1.o // ...    
    }
}
```
we can do that for all the other sections.<br>
```ld
SECTIONS
{
.text : {
	*(.text) 
	*(.rodata)	
}
.data : {
	*(.data)
}
.bss : {
    *(.bss)
    }
}
```
>Note
i just added the `.rodata` under the `.text` sections 

We can control also the oder the section in the final `elf` (who shows up first), this is important, will see that later. 

- now how to control the placement of a section in a memory region, for example set the `.text` section to be in  FLASH memory region ?<br>
we can do that using keywords:<br> 
`VMA` :virtual memory address (static memory), it could be another `SRAM` you defined (`SRAM`, `SRAM2`).<br>
`LMA`: load memory address  (physical memory), `FLASH` or an external Flash memory <br>
`AT` command. <br>
using these variations, we can instruct the linker where to put the different sections, (`FLASH`, `SRAM`, `EXternal FLASH`) it depends on your application.<br>
**- To instruct the linker to put the section in `LMA`**
```ld
.section_name{

}>  FLASH
```
**- To instruct the linker relocate the section in from `LMA` to `VMA`**
```ld
.section_name{

}> SRAM AT>  FLASH
```
**- To instruct the linker to put the section in `VMA`**
```ld
.section_name{

}>  SRAM
```
we obtain this code now: 
```ld
SECTIONS 
{
  /* text section */ 
  .text : {
    *(.text)
    *(.rodata)
  }>FLASH
  /*  data section  */   
  .data : {
    *(.data)
  }> SRAM AT> FLASH
  /* uninitialized data */ 
  .bss : {
  *(.bss) 
  }> SRAM
}
```
in linker script there are other utilities we need in the next section used in the `SECTIONS`
- **`.` (dot) operator**: 

the `.` operator is counter that keeps record of used memory.<br> 
for example we can measure the end of the `.text` section 
```ld 
.text{ 
/* specifies the beginning of the address */ 
    . ;
    *(.text) 
    *(.rodata)
/* store the end of .text section in the symbol end_of_text */
    end_of_text = . ;
}
```
- **ALIGN(4):**
this command ensures that memory region is a aligned by `4` bytes.<br>
the memory address values is mutiple of `4` (`0x08000000`,`0x08000004`, `0x08000008`).<br> 
Why aligning an address.<br>
The CPU (`CORTEX-M4`)32 bit register, meaning the processor access the memory in chunks of `4` bytes, when data is `ALIGN(4)` the CPU can fetch the data in one memory cycle, increasing efficiency.<br> 
lest's take `end_of_text` is a variable containing the end text data region address. that address coud be `0x08000082`(not aligned).<br>
If the next section, say `.data`, starts immediately at `0x08000083`, this would create an `alignment issue`. Since `0x08000083` is not a multiple of `4`, the CPU would need extra steps to access the data efficiently, leading to performance degradation
```ld
.text{ 
/* specifies the beginning of the address */ 
    . ;
    *(.text) 
    *(.rodata)
/* store the end of .text section in the symbol end_of_text */
    . = ALIGN(4) ;
    end_of_text = . ;
}
```
##### 5- How to compile a linker script: 

```bash 
arm-none-gcc -nostdlib -T stm32_ls.ld *.o -o prog.elf
```

- `-nostdlib`: specifies that we are not using the `glibc` the C standard libary. ( gcc by default assumes you are using `libc` )
- `-Map`: creates a file with a detailed overview of the memory layout, including the sizes and locations of all sections and symbols. 
##### 6- Conclusion: 
the `linker script` is an essential file to describe how you want to control your memory.
In the next Section we will see what is `start up` file and how to write one. 
#### startup file 


### Elf file 



### ToilChains 
A toolChain is collection of binaries which allows to:
- compiler  : compiles your code (`C/C++,..`) to an assembly code.  
- assembler : transforms the assembly code to object files `.o` (binary format)
- linker    : takes all the object files combines them, to obtain the final executable `.elf`. 
- Build systems: automates the building process of the final executable `make, camke, ...`
- Libraries: combination of function you use a cross your application (`math.h`)
- Debuggers :    allows you to inspect your code at runtime, step by step. 
- Programmer    : a utility that uploads the final executable on your target MCU. 
- emulator      : a tool that emulates you hardware used for testing purposes     

For our application, we will be using `arm-gcc`(compiler, assembler and linker), for the momement we don't need a library, 
as the build system, i am using `make`, the debugger is `GDB` and for the programmer i am using the open-source tool `open-ocd`



#### 1- arm-gcc: 
this is a cross-compiler that compiles the code on host machine (you machine ie: `x86_64`) for tagret architechure `arm`.<br>
the arm-gcc also come with these utilities:
- dissect different sections 
- disassemble 
- extract symbol and size information 
- convert executable to other formats, bin,hex 
- Provides C standard libraries 
###### Installation
`Toolchains` for arm: 
- `arm-gcc` : GNU `toolchain` (Free and open-source)
- `armcc` from ARM, (ships with `KEIL`, code restricted, requires licensing)
linux: 
`sudo apt install gcc-arm-none-eabi `
or 
```bash 
wget "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2"
tar -jxf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
rm gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
export PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin:$PATH"
```
You  can put the export command in the `.bashrc` file

###### usage
```bash
arm-none-eabi-gcc file.c -o file
```
- `-o` means specifies the output file name    
- `-E`: stop after the pre-processing stage, output file format `.i` 
- `-c`: tells the compiler to compile and assemble the `C` file but not link, output file format (`.o) 
- `-S` stop after the compilation stage, do not assemble, output file format `.S`
- `-march=[name]`: this specifies the architecture that the assembler will use, (each arch has it's specific assembly language)  
`-mcpu=[name]`:  specifies the name of the target arm processor, GCC will use this name to derive the target architecture (`march`) and the ARM processor type for which to tune for performance (`mtune`) 
- `-mthumb` or `-marm` by default it's set to `-marm`: select between generating code that executes in `ARM` and `Thumb` states  
>**Note**
>- **Use `-mthumb`** if you need smaller code size and are working in an environment where memory is limited, and performance isn't the highest priority. (tells the assembler to use 16-bit long instructions) .
>- **Use `-marm`** if you need maximum performance, particularly for tasks that require heavy computation, and memory size isn't a primary concern.(tells the assembler to use 32-bit long instructions) .
```bash
arm-none-eabi-gcc -c main.c -mthumb -mcpu=cortex-m4 -o main.o
```
**Other flags:**
`-OO:` no optimization
`-std=gnu11` specifies what C standard to use

#### 2- Build system (make) 
As i have said before, make is build system that automates the compilation process of your project 
<!---- TODO  ----->
##### TODO
todo 

#### 3- Libraries: 
we will understand the diff between the static library and dynamic library, create a static library and include it.  
##### TODO
todo 
<!---- TODO  ----->


#### 4- flashing 
how to use the open-ocd to flash the compiled code 
##### TODO
todo 
<!---- TODO  ----->

#### 5- Debugging 
will understand how GDB works and how to use it

<!---- TODO  ----->
##### TODO
todo 

#### 6- Emulators 
how QMU works and how to use it 
we will look also to renode 
##### TODO
todo 

# conclusion 









