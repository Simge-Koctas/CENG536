#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

struct Pipe
{
	int fd[2];
	double execTime;
	int sortedArray[7];
	char signalName[8];
	char sigReceiveTime[9];
};

void sigUSR1_handler()
{
	printf("sigusr1!\n");
}

void sigUSR2_handler()
{
	printf("sigusr2!\n");
	
}


void printArray(int arr[], int n)
{
	for(int i = 0; i < n; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void printArrayDouble(double arr[], int n)
{
	//printf("double array: ");
	for(int i = 0; i < n; i++)
	{
		printf("%f ", arr[i]);
	}
	printf("\n");
}

void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void selectionSort(int arr[], int n)
{
	int i, j, min;
	for(i = 0; i < n-1; i++)
	{
		min = i;
		for(j = i+1; j<n; j++)
		{
			if(arr[j] < arr[min])
			{
				min = j;
			}	

		}
		swap(&arr[min],&arr[i]);
	}

}

void insertionSort(int arr[], int n)
{
	int i,j, key;
	for(i = 1; i < n; i++)
	{
		key = arr[i];
		j = i - 1;

		while(key < arr[j] && j >= 0)
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
	}
}

int cmpfcn(const void *a, const void *b)
{
	double f1 = *((double*)a);
	double f2 = *((double*)b);
	if(f1 < f2)
		return -1;
	else if(f1 > f2)
		return 1;
	return 0;
}

int main()
{
	int childNum = 0;
	printf("Enter the number of child processes to create: ");
	scanf("%d", &childNum);
	int childId = 1;
	pid_t cid = -1;
	clock_t start, end;
	struct Pipe pipeArray[childNum];
	double execTime;
	char sig1[8] = "SIGUSR1";
	char sig2[8] = "SIGUSR2";
	char timeSig[9];
	double temp[childNum];
	time_t rawtime = time(NULL);

	for(int i=0; i < childNum; i++){
		pipe(pipeArray[i].fd);

		
		signal(SIGUSR1, sigUSR1_handler);
		signal(SIGUSR2, sigUSR2_handler);
		struct tm *ptm = localtime(&rawtime);

		cid = fork();

		if(cid == 0){
			int b;
			int receivedArray[7];
			printf("Hello I am child with id: %d\n ", getpid());

			read(pipeArray[i].fd[0], &receivedArray, 7*sizeof(int));

			printf("Array Elements Before Sorting:\n");
			for(int j = 0; j < 7; j++)
			{
				printf("%d ", receivedArray[j]);
			}
			printf("\n");
			
			int n = sizeof(receivedArray)/sizeof(receivedArray[0]);
			if(getpid() % 2 == 1)
			{	
				start = clock();
				selectionSort(receivedArray, n);
			}
			else
			{
				start = clock();
				insertionSort(receivedArray, n);
			}
			end = clock();
			printf("Array Elements After Sorting:\n");
			printArray(receivedArray, n);
			printf("\n");
			execTime = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("Execution time:%f\n ", execTime );

			int waitTime = rand()%7+1;

			sleep(waitTime);
			execTime += waitTime;	
		
			sprintf(timeSig, "%d:%d:%d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

			printf("Signal receive time:%s\n", timeSig);

			close(pipeArray[i].fd[0]);
			write(pipeArray[i].fd[1], &receivedArray, 7*sizeof(int));
			write(pipeArray[i].fd[1], &execTime, sizeof(double));

			if(getpid() % 2 == 1){
				write(pipeArray[i].fd[1], sig1, sizeof(sig1));
			}
			else{
				write(pipeArray[i].fd[1], sig2, sizeof(sig2));
			}
			//write(pipeArray[i].fd[1], &sigReceiveTime, sizeof(double));
			write(pipeArray[i].fd[1], timeSig, sizeof(timeSig)); 
			exit(0);
			
		}
		else{
		
		childId++;
		int randArray[7];
		int sortedArray[7];
		

		for(int i = 0; i < 7; i++)
		{	
			randArray[i] = rand()%100+1;

		}
		
		write(pipeArray[i].fd[1], &randArray, 7*sizeof(int));

		int returnStatus;
		waitpid(cid, &returnStatus, 0);
		
		if(cid % 2 == 1)
				kill(cid, SIGUSR1);
		else
				kill(cid, SIGUSR2);
		
		close(pipeArray[i].fd[1]);
		read(pipeArray[i].fd[0], &(pipeArray[i].sortedArray), 7*sizeof(int));
		
		read(pipeArray[i].fd[0], &(pipeArray[i].execTime), sizeof(double));

		//for(int j = 0; j < 7; j++)
		//{
		//	printf("%d ", pipeArray[i].sortedArray[j]);
		//}

		read(pipeArray[i].fd[0], &(pipeArray[i].signalName), sizeof(sig1));
		//read(pipeArray[i].fd[0], &(pipeArray[i].sigReceiveTime), sizeof(double));
		read(pipeArray[i].fd[0], &(pipeArray[i].sigReceiveTime), sizeof(timeSig)); 

		printf("parent reads execTime: %f\n ", pipeArray[i].execTime);
		printf("parent reads signal name:%s\n ", pipeArray[i].signalName);
		printf("parent reads signal receive time:%s\n ", pipeArray[i].sigReceiveTime); 
		
		temp[i] = pipeArray[i].execTime;
		
		} 
	} 
	
	printf("Printing exec time array:\n");
	printArrayDouble(temp, childNum);
	//selectionSort(temp, childNum);
	qsort(temp, childNum, sizeof(double),cmpfcn);
	printf("Printing exec time array after sort:\n");
	printArrayDouble(temp, childNum);

	FILE *fp = fopen("outputpipe.txt", "w");

	printf("Results:\n");
	for(int i = 0; i < childNum; i++)
	{
		for(int j = 0; j < childNum; j++)
		{

			if(temp[i] == pipeArray[j].execTime)
			{
				printf("%f", pipeArray[j].execTime);
				fprintf(fp, "%f ", pipeArray[j].execTime);

				printf(" ");
				fprintf(fp, " ");

				for(int k = 0; k < 7; k++)
				{
					printf("%d ",pipeArray[j].sortedArray[k]);
					fprintf(fp, "%d ", pipeArray[j].sortedArray[k]);
				}

				printf(" ");
				fprintf(fp, " ");

				printf("%s",pipeArray[j].signalName);
				fprintf(fp, "%s ", pipeArray[j].signalName);

				printf(" ");
				fprintf(fp, " ");

				printf("%s",pipeArray[j].sigReceiveTime); 
				fprintf(fp, "%s ", pipeArray[j].sigReceiveTime); 
				
				printf("\n");
				fprintf(fp, "\n");
			}
		} 

	} 
	fclose(fp);
		
		
	return 0;

} // main
