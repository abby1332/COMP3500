#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int main() {
	int arr[10];
	printf("Enter 10 numbers:\n");
	for(int i = 0; i < 10; i++) {
		scanf("%d", arr + i);
	}
	
	double average = 0.0;
	for(int i = 0; i < 10; i++) {
		average = average + sqrt(arr[i]);		
	}
	average = average/10;
	
	printf("The average of the square roots of all the numbers is %.2f\n", average);
}
