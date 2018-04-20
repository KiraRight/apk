#include <dos.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct VIDEO
{
	unsigned char symb;
	unsigned char attr;
};

#define COLOR_COUNT 7

unsigned char colors[COLOR_COUNT] = { 0x71,0x62,0x43,0x54,0x35,0x26,0x17 };
char color = 0x71;  

void changeColor();
void get_reg();         		//get data from registres
void print(int offset, int);    //fill in the screen

//IRQ0-7
// IRQ 0  - interrupt of timer 
// IRQ 1  - interrupt of keypad
// IRQ 2  - interrupt for cascade interruptions in AT machines
// IRQ 3  - interrupt of async port COM 2
// IRQ 4  - interrupt of async port COM 1
// IRQ 5  - interrupt of hard disk controller
// IRQ 6  - interrupt of floppy disk controller 
// IRQ 7  - interrupt of printer
void interrupt(*int8) (...);
void interrupt(*int9) (...);
void interrupt(*intA) (...);
void interrupt(*intB) (...);
void interrupt(*intC) (...);
void interrupt(*intD) (...);
void interrupt(*intE) (...);
void interrupt(*intF) (...);
//IRQ8-15
// IRQ 8  - interrupt of real time clock
// IRQ 9  - interrupt of EGA controller
// IRQ 10 - reserved interrupt
// IRQ 11 - reserved interrupt
// IRQ 12 - reserved interrupt
// IRQ 13 - interrupt of mathematic soprocessor
// IRQ 14 - interrupt of hard disk
// IRQ 15 - reserved interrupt
void interrupt(*int70) (...);
void interrupt(*int71) (...);
void interrupt(*int72) (...);
void interrupt(*int73) (...);
void interrupt(*int74) (...);
void interrupt(*int75) (...);
void interrupt(*int76) (...);
void interrupt(*int77) (...);

// set function for service interrupt
void interrupt  intA8(...) { get_reg(); int8(); }
void interrupt  intA9(...) { changeColor(); get_reg(); int9(); }
void interrupt  intAA(...) { get_reg(); intA(); }
void interrupt  intAB(...) { get_reg(); intB(); }
void interrupt  intAC(...) { get_reg(); intC(); }
void interrupt  intAD(...) { get_reg(); intD(); }
void interrupt  intAE(...) { get_reg(); intE(); }
void interrupt  intAF(...) { get_reg(); intF(); }

void interrupt  int08(...) { get_reg(); int70(); }
void interrupt  int09(...) { get_reg(); int71(); }
void interrupt  int0A(...) { get_reg(); int72(); }
void interrupt  int0B(...) { get_reg(); int73(); }
void interrupt  int0C(...) { changeColor(); get_reg(); int74(); }
void interrupt  int0D(...) { get_reg(); int75(); }
void interrupt  int0E(...) { get_reg(); int76(); }
void interrupt  int0F(...) { get_reg(); int77(); }


void changeColor()
{
	color = colors[rand() % COLOR_COUNT-1];
	return;
}

// fill videomemory pointert
void print(int offset, int val)
{
	char temp;                                         // the current bit
	int i;
	VIDEO far* screen = (VIDEO far *)MK_FP(0xB800, 0); // get Video card buffer
	screen += offset;
	for (i = 7; i >= 0; i--)        				// find bits
	{
		temp = val % 2;								// get last bit
		val /= 2;                                   // move on right
		screen->symb = temp + '0';                  // save bit as a char ('0' or '1')
		screen->attr = color;                       // set color of print symbol
		screen++;									// get to next symbol
	}
}

// get data from registers
void get_reg()
{
	int reg;

	reg = inp(0x21);   // get mask Master registor
	print(0, reg);

	outp(0x20, 0x0B);// switch to Master's service register
	reg = inp(0x20);
	print(9, reg);

	outp(0x20, 0x0A);// switch to Master's request register
	reg = inp(0x20);
	print(18, reg);

	reg = inp(0xA1);  // get mask Slave registor
	print(80, reg);

	outp(0xA0, 0x0B); //switch to Slave's service register
	reg = inp(0xA0);
	print(80 + 9, reg);

	outp(0xA0, 0x0A);//requests
	reg = inp(0xA0);
	print(80 + 18, reg);
}

void init()
{
	//IRQ0-7
	int8 = getvect(0x08);
	int9 = getvect(0x09);
	intA = getvect(0x0A);
	intB = getvect(0x0B);
	intC = getvect(0x0C);
	intD = getvect(0x0D);
	intE = getvect(0x0E);
	intF = getvect(0x0F);

	//IRQ8-15
	int70 = getvect(0x70);
	int71 = getvect(0x71);
	int72 = getvect(0x72);
	int73 = getvect(0x73);
	int74 = getvect(0x74);
	int75 = getvect(0x75);
	int76 = getvect(0x76);
	int77 = getvect(0x77);

	setvect(0xA8, intA8); // 08-0F
	setvect(0xA9, intA9);
	setvect(0xAA, intAA);
	setvect(0xAB, intAB);
	setvect(0xAC, intAC);
	setvect(0xAD, intAD);
	setvect(0xAE, intAE);
	setvect(0xAF, intAF);

	setvect(0x08, int08); // 70-77
	setvect(0x09, int09);
	setvect(0x0A, int0A);
	setvect(0x0B, int0B);
	setvect(0x0C, int0C);
	setvect(0x0D, int0D);
	setvect(0x0E, int0E);
	setvect(0x0F, int0F);


	_disable(); // disable interrupts handling (cli)

	//ICW1 - initialize 
	//ICW2 - base vector 
	//ICW3 - the port bit of Slave (in binary format) or the port number of connected port on Master
	//ICW4 - default
	// MASTER 
	outp(0x20, 0x11);	
	outp(0x21, 0xA8);   
	outp(0x21, 0x04);	
	outp(0x21, 0x01);	

						// SLAVE 
	outp(0xA0, 0x11);	
	outp(0xA1, 0x08);  
	outp(0xA1, 0x02);	
	outp(0xA1, 0x01);	

	_enable(); // enable interrupt handling (sti)
}

int main()
{
	unsigned far *fp;
	init();
	FP_SEG(fp) = _psp; // segment
	FP_OFF(fp) = 0x2c; // offset
	_dos_freemem(*fp);

	_dos_keep(0, (_DS - _CS) + (_SP / 16) + 1);//TSR
	return 0;
}



