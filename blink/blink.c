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

int main(int argc, char **argv)
{
  // Set up gpi pointer for direct register access
  setup_io();
  // Disable buffering for stdout
  setbuf(stdout, NULL);
  //uint32_t const PINS[] = {7, 9, 10, 17, 27, 22};
  uint32_t const PINS[] = {9, 27, 10, 22, 7, 17};
  int32_t pin_mask = 0;
  int dir = 2;
  int g;
  int g2;

  for (g=0; g < ARRAY_LENGTH(PINS); g++) {
    int pin = PINS[g];
    pin_mask |= 1 << pin;
    INP_GPIO(pin);
    OUT_GPIO(pin);
  }
  g = 0;
  GPIO_CLR = pin_mask;
  while(1) {
    g = (g + dir) % ARRAY_LENGTH(PINS);
    GPIO_SET = 1 << PINS[g] | 1 << PINS[g+1];
    usleep(100*1000);
    GPIO_CLR = 1 << PINS[g] | 1 << PINS[g+1];
    usleep(100*1000);
    if (g == 0) {
      dir = (dir == 2) ? ARRAY_LENGTH(PINS) - 2 : 2;
    }
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
