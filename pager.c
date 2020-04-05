#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The maximum possible value for argv[1] (aka frames) */
#define MAX_FRAMES 32

// Boolean type
typedef enum { false, true } bool;

// Returns the index of the first instance of n in arr. If the array doesn't contain the value, it returns -1.
int arrayContains(int arr[], int len, int n)
{
	int i;

	// Goes through the array back to front.
	for (i = 0; i < len; i++)
		if (arr[i] == n)
			return i;
	return -1;
}

// Returns the index of the smallest element in an array.
int findSmallestValue(int array[], int len)
{
	int i, min = 0;

	// Find the smallest value's index and return it.
	for (i = 1; i < len; i++)
		if (array[i] < array[min])
			min = i;
	return min;
}

// Add a value into a queue. If the queue is full, the first index (first in) is dropped.
void enqueue(int queue[], int val, int len)
{
	int i;
	
	// Shift everything 1 to the left.
	for (i = 1; i < len; i++)
		queue[i - 1] = queue[i];

	// Set the last index to the value enqueued.
	queue[len - 1] = val;
}

// Returns the next index of a value, n, starting at index, start. Returns -1 if there isn't one.
int nextIndexOf(int array[], int start, int len, int n)
{
	int i;

	for (i = start; i < len; i++)
		if (array[i] == n)
			return i;
	return -1;
}

// Returns the index of the value that will be used farthest in the future that is currently in the frames array.
int findOptimalValue(int frames[], int prs[], int start, int len)
{
	int temp[len], i, optimal = 0;

	// Finds the next index of each value in the array of frames and stores it in temp.
	for (i = 0; i < len; i++)
		temp[i] = nextIndexOf(frames, start, len, frames[i]);

	// Finds the greatest index or -1 in the temp array.
	for (i = 0; i < len; i++)
	{
		if (temp[i] == -1)
			return i;
		if (temp[i] > temp[optimal])
			optimal = i;
	}

	return optimal;
}

// Returns number of faults for the FIFO replacement algorithm.
int fifo_faults(int frames, int prs[], int len)
{
	// FIFO replaces the oldest element, implement it like a queue.
	int queue[frames], faults = 0, i;

	memset(queue, -1, sizeof(int) * frames);

	for (i = 0; i < len; i++)
	{	
		// Check if the page number is in the queue, if not we need to enqueue in.
		if(arrayContains(queue, frames, prs[i]) == -1)
		{
			enqueue(queue, prs[i], frames);
			faults++;
		}
	}

	return faults;
}

// Returns the number of faults for the LRU replacement algorithm.
int lru_faults(int frames, int prs[], int len)
{
	// framesCounter keep track of the last time a frame was used (index of the prs array).
	int faults = 0, framesArray[frames], framesCounter[frames], i, lru, temp;
	
	memset(framesArray, -1, sizeof(int) * frames);
	memset(framesCounter, -1, sizeof(int) * frames);

	for (i = 0; i < len; i++)
	{
		temp = arrayContains(framesArray, frames, prs[i]); 
		
		if (temp == -1)
		{
			// If there is a fault, find the least recently used frame and load the page into that frame with a timestamp.
			lru = findSmallestValue(framesCounter, frames);
			framesArray[lru] = prs[i];
			framesCounter[lru] = i;
			faults++;
		}
		else
			 framesCounter[temp] = i; // Update the counter for this frame to indicate it was used again.
	}
	
	return faults;
}

// Returns the number of faults for the OPT/MIN replacement algorithm.
int opt_faults(int frames, int prs[], int len)
{
	int i, j, framesArray[frames], temp[frames], optimal = 0, faults = 0;

	memset(framesArray, -1, sizeof(int) * frames);

	for (i = 0; i < len; i++)
	{
		if(arrayContains(framesArray, frames, prs[i]) == -1)
		{
			// Fill the temp array with the next usage of each element in framesArray.
			for (j = 0; j < frames; j++)
				temp[j] = nextIndexOf(prs, i, len, framesArray[j]);

			// Find the largest number in the array (-1 is treated as infinity).
			for (j = 0; j < frames; j++)
			{
				if (temp[j] == -1)
				{
					optimal = j;
					break; // Break since we found a -1 and the loop doesn't need to continue.
				}
				if (temp[j] >= temp[optimal])
					optimal = j;
			}

			// Replace the optimal index
 			framesArray[optimal] = prs[i];

			faults++;
		}
	}

	return faults;
}

int main(int argc, char *argv[])
{
	int i; /* Loop counter */
	int *prs; /* This will hold the page reference string. (*prs and prs[] are the same thing) */
	int frames;  /* Int variable to hold first argument */
	int len=argc-2;

	/* Error check on argc */
	if (argc<2)
	{
		printf("Usage: ./a.out frames page_reference_list\n");
		exit(1);
	}

	/* Error check on argv[1] (aka frames) */
	frames = atoi(argv[1]);
	if (frames < 1 || frames > MAX_FRAMES)
	{
		printf("Usage: ./a.out frames page_reference_list\n");
		printf("       frames must be an integer between 1 and %d\n", MAX_FRAMES);
		exit(1);
	}
	
	/* Convert page_referenced_list into an array of ints */
	prs = malloc(sizeof(int)*(len));
	for (i=0; i<len; ++i) *(prs+(i))=atoi(argv[i+2]);

	/* Test the three functions */
	printf("FIFO faults: %d\n", fifo_faults(frames, prs, len));
	printf("LRU faults: %d\n", lru_faults(frames, prs, len));
	printf("OPT faults: %d\n", opt_faults(frames, prs, len));
}
