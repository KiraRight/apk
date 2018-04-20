#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>

#define COLUMNS 14
#define ROWS 14
#define ITERATION 1000000
#define MAX 101

int main()
{
	int matrix[ROWS][COLUMNS];
	int sum_c[ROWS] = { 0 };
	int sum_asm[ROWS] = { 0 };
	int sum_mmx[ROWS] = { 0 };
	long double time_c = 0, time_asm = 0, time_mmx = 0;
	LARGE_INTEGER frequency, start, finish;

	for (int iter = 0; iter < ITERATION; iter++)
	{
		srand((unsigned)time(NULL));
		for (int j = 0; j < ROWS; j++) 
			for (int i = 0; i < COLUMNS; i++)
			{
				matrix[i][j] = rand() % MAX + 0;
			}

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLUMNS; j++)
			{
				sum_c[i] += matrix[i][j];
			}
		QueryPerformanceCounter(&finish);
		time_c += (finish.QuadPart - start.QuadPart)*1000.f / frequency.QuadPart;

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		_asm
		{
			/*pusha
			xor esi, esi
			xor edi, edi
			mov ecx, ROWS
			loop_1:
				push ecx
				mov ecx, COLUMNS
				loop_2 :
					mov eax, [matrix + esi]
					add[sum_asm + edi], eax
					add esi, 4
				loop loop_2
				add edi, 4
				pop ecx
				loop loop_1
			popa*/
			pusha
			xor edi, edi
			xor esi, esi
			mov ecx, ROWS
			rows :
				push ecx
				mov ecx, COLUMNS
				columns :
					mov edx, dword ptr matrix[esi]
					add dword ptr sum_asm[edi], edx
					add esi, 4
				loop columns
				add edi, 4
				pop ecx
				loop rows
			popa
		}
		QueryPerformanceCounter(&finish);
		time_asm += (finish.QuadPart - start.QuadPart)*1000.f / frequency.QuadPart;

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start);
		_asm
		{
			pusha
			xor esi, esi
			xor edi, edi
			mov ecx, ROWS
			loop_1mmx :
				push ecx
				mov ecx, COLUMNS / 2
				pxor MM0, MM0
					loop_2mmx :
						paddd MM0, [matrix + esi]
						add esi, 8
					loop loop_2mmx
				movd eax, MM0
				psrlq MM0, 32
				movd ebx, MM0
				add eax, ebx
				add[sum_mmx + edi], eax
				add edi, 4
				pop ecx
			loop loop_1mmx
			emms
			popa
		}
		QueryPerformanceCounter(&finish);
		time_mmx += (finish.QuadPart - start.QuadPart)*1000.f / frequency.QuadPart;
	}
	//for (int i = 0; i < ROWS; i++)
	//{
	//	for (int j = 0; j < COLUMNS; j++)
	//	{
	//		printf("%7d", matrix[i][j]);
	//	}
	//	printf("\n");
	//}
	for (int i = 0; i < ROWS; i++)
	{
		printf("Sum[%d]_C = %d\n", i + 1, sum_c[i]);
	}
	printf("Time C: %lf\n\n", time_c);
	for (int i = 0; i < ROWS; i++)
	{
		printf("Sum[%d]_asm = %d\n", i + 1, sum_asm[i]);
	}
	printf("Time asm: %lf\n\n", time_asm);
	for (int i = 0; i < ROWS; i++)
	{
		printf("Sum[%d]_mmx = %d\n", i + 1, sum_mmx[i]);
	}
	printf("Time MMX: %lf\n\n", time_mmx);

	system("pause");
}