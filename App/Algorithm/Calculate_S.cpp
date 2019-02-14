#include <stdio.h>
#include <iostream>

using namespace std;

double exp(double x)
{
	int i = 0;

	x = 1.0 + x/256;
	for(; i<8; i++)
	{
		x = x*x;
	}


	return x;		
}

void CaluclateSModelLine(float fre[], int arr[], int len, float fre_max, float fre_min, float flexible)
{
	int i = 0;
	float x;
	float y;
	float D_value = fre_max - fre_min;

	for(;i<len; i++)
	{
		x = flexible*(i-(len/2))/(len/2);

		printf("x:%lf\n", x);
		y = 1.0/(1+exp(-x));
		fre[i] = D_value*y + fre_min;
		arr[i] = (int)((8000000/fre[i]) - 1);
		// printf("fre[%d]:%lf\n", i, fre[i]);
		// printf("arr[%d]:%d\n", i, arr[i]);
	}
	return;
}

int main(void)
{
  	float fre[800] = {0.0};
	int   arr[800] = {0};

	CaluclateSModelLine(fre, arr, 800, 6400, 400, 6);

	return 0;
}

