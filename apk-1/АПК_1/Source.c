//10.F(x)=x*sqrt(x)/log2(x),x>0

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>
#include<Windows.h>

double setValue();

void main(void)
{
	double a, b, x, step, f;
	int check = 1;
	LARGE_INTEGER timerFrequency, timerStart, timerStop;
	while (check)
	{
		system("cls");
		printf("Your a=");
		do
		{
			a = setValue();
			if (a <= 1)
				printf("Error, please enter a > 1, a=");
		} while (a <= 1);
		printf("Your b=");
		do
		{
			b = setValue();
			if (b <= 1)
				printf("Error, please enter b > 1, b=");
		} while (b <= 1);
		printf("Your step=");
		step = setValue();
			

		x = a;
		f = 0;
		QueryPerformanceFrequency(&timerFrequency);
		QueryPerformanceCounter(&timerStart);
		while (x <= b)
		{
			f += x*sqrt(x) / log2(x);
			x += step;
		}

		QueryPerformanceCounter(&timerStop);
		printf("C time is %lf and f=%lf\n", (timerStop.QuadPart - timerStart.QuadPart)*1000.0f / timerFrequency.QuadPart, f);


		QueryPerformanceFrequency(&timerFrequency);
		QueryPerformanceCounter(&timerStart);

		x = a;
		f = 0;
		_asm
		{
			finit
			fld b
			fld x
			loop_start :
			fcom
				fstsw ax
				and ah, 01000101b
				jz loop_end

				fld x
				fld x
				fsqrt
				fmul
				fld1
				fld x
				fyl2x
				fdiv

				fadd f
				fstp f
				fadd step
				fst x
				jmp loop_start
				loop_end :
			fwait
		}

		QueryPerformanceCounter(&timerStop);
		printf("Assembler time is %lf and f=%lf\n", (timerStop.QuadPart - timerStart.QuadPart)*1000.0f / timerFrequency.QuadPart, f);
		printf("Enter 0 to exit");
		scanf("%d", &check);
	}
	return 0;
}

double setValue()
{
	double x;
	int check = 0;
	while (check != 1)
	{
		check = scanf("%lf", &x);
		system("cls");
		if(!check)
			printf("Error, please enter a numeric value\n");
		rewind(stdin);
	}
	return x;
}