/***********************************
题目一：找出数组中重复的数字
在一个长度为n的数组里的所有数字都在0~n-1的范围内。数组中某些数字是重复的，
但不知道有几个数字重复了，也不知道每个数字重复了几次。请找出数组中任意一个
重复的数字。eg：如果输入长度为7的数组{2,3,1,0,2,5,3},那么对应的输出是
重复的数字2或者3
************************************/
#include <stdio.h>


bool duplicate(int numbers[], int length, int* duplication)
{
	if(numbers == NULL || length <= 0)
	{
		return false;
	}

	for(int i=0; i<length; ++i)
	{
		if(numbers[i]<0 || numbers[i]>length-1)
			return false;
	}

	for(int i=0; i<length; ++i)
	{
		while(numbers[i] != i)
		{
			if(numbers[i] == numbers[numbers[i]])
			{
				*duplication = numbers[i];
			}

			// swap numbers[i] and numbers[numbers[i]]
			int temp = numbers[i];
			numbers[i] = numbers[temp];
			numbers[temp] = temp;

		}
	}

	return false;
}




/***********************************
题目二：不修改数组找出重复的数字
在一个长度为n+1的数组里的所有数字都在0~n的范围内。所以数组中至少有一个数
字是重复的。请找出数组中任意一个重复的数字，但不能修改输入的数组。
eg：如果输入长度为7的数组{2,3,1,0,2,5,3},那么对应的输出是
重复的数字2或者3
************************************/
int getDuplication(const int* numbers, int length)
{
	if(numbers == nullptr || length <= 0)
		return -1;

	int start = 1;
	int end = length - 1;
	
	while(end>=start)
	{
		int middle = ((end - start) >> 1) + start;
		int count = countRange(numbers, length, start, middle);
		
		if(end == start)
		{
			if(count>1)
				return start;
			else
				break;
		}

		if(count>(middle-start+1))
			end = middle;
		else
			start = middle + 1;
	}
	return -1;
}


int countRange(const int* numbers, int length, int start, int end)
{
	if(numbers == nullptr)
		return 0;

	int count = 0;
	for(int i=0; i<length; i++)
	{
		if(numbers[i] >= start && numbers[i] <= end)
			++count;
	}

	return count;
}








