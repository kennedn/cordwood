//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//  Revised: 15-Feb-2013


// Access from ARM Running Linux

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof((x)[0]))

void setup_io();


#define LATCH_PIN 22
#define CLOCK_PIN 9
#define DATA_PIN 10

void pulse_pin(uint32_t pin) {
  GPIO_SET  = 1 << pin;
  usleep(1);
  GPIO_CLR = 1 << pin;
  usleep(1);
}

void send_byte(uint8_t bits) {
  for (int i=0; i < 8; i++) {
    // Select GPIO_SET or GPIO_CLR register based on bits value
    volatile unsigned *reg = (bits & 1 << i) >> i ? (gpio+7) : (gpio+10); 
    *reg = 1 << DATA_PIN;
    pulse_pin(CLOCK_PIN);
  }
  pulse_pin(LATCH_PIN);
}

int main(int argc, char **argv)
{
  // Set up gpi pointer for direct register access
  setup_io();
  // Disable buffering for stdout
  setbuf(stdout, NULL);
  
  INP_GPIO(CLOCK_PIN);
  OUT_GPIO(CLOCK_PIN);

  INP_GPIO(DATA_PIN);
  OUT_GPIO(DATA_PIN);

  while(1) {
    //green
    send_byte(0b100001);
    usleep(200*1000);

    //yellow
    send_byte(0b010010);
    usleep(200*1000);

    //red
    send_byte(0b001100);
    usleep(200*1000); 

    //yellow
    send_byte(0b010010);
    usleep(200*1000);
  }

  return 0;
}


//
// Set up a memory regions to access GPIO
//
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;


} // setup_io
