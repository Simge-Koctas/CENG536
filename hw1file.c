#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>


void printArray(int arr[], int n)
{
	for(int i = 0; i < n; i++)
	{
		printf("%d ", arr[i]);
	}
}

void printArrayFloat(float arr[], int n)
{
	for(int i = 0; i < n; i++)
	{
		printf("%f ", arr[i]);
	}
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

int calculateNumber(char temp[], int k)
{
	int result = 0;
	sscanf(temp, "%d", &result);

	return result;
}

float calculateNumberFloat(char temp[], int k)
{
	float result = 0.0;
	sscanf(temp, "%f", &result);

	return result;
}

void cleanArray(char temp[])
{
	for(int i = 0; i < 10; i++)
	{
		temp[i] = ' ';
	}
}

void removeDuplicates(float array[], int *n)
{
	for(int i = 0; i < *n; i++)
	{
		for(int j = i + 1; j < *n; j++)
		{
			if(array[i] == array[j])
			{
				for(int k = j; k < *n; k++)
				{
					array[k] = array[k+1];
				}
				j--;
				(*n)--;
			}
		}
	}
}

void sigUSR1_handler()
{
	printf("sigusr1!\n");
}

void sigUSR2_handler()
{
	printf("sigusr2!\n");
	
}


int main()
{
	int childNum = 0;
	printf("Enter the number of child processes to create: ");
	scanf("%d", &childNum);
	int childId = 1;
	pid_t cid = -1;
	clock_t start, end;
	time_t rawtime = time(NULL);

	for(int i=0; i < childNum; i++){
		
		
		if(getpid()%2 == 1)
			signal(SIGUSR1, sigUSR1_handler);
		else
			signal(SIGUSR2, sigUSR2_handler);
		struct tm *ptm = localtime(&rawtime);
		

		cid = fork();

		if(cid == 0){

			printf("Hello I am child with id: %d\n ", getpid());
			FILE* fp;
			char ch;
			char filename[100];
			char outfilename[100];
			char myoutfilename[100];
			printf("Contents of the file ");

	/*
			for(int i = 0; i < childNum; i++){
				sprintf(filename, "input%d.txt", i+1);
				printf("%s:\n", filename);
				fp = fopen(filename, "r");
				do{
					ch = fgetc(fp);
					printf("%c", ch);
				}while(ch != EOF);
			}
			fclose(fp);
	*/

			sprintf(filename, "input%d.txt", childId); //open the related input file
			printf("%s:\n", filename);
			fp = fopen(filename, "r");
			int array[7];
 			char temp[10]; 
			int num = 0;
			int j = 0;
			int k = 0;
			do{
				ch = fgetc(fp);
				int count = 0;
				if(ch == ' ' || ch == EOF){ //get the numbers from the file
					//printf("bosluk\n");
					num = calculateNumber(temp, k);
					array[j] = num;
					j++;
					cleanArray(temp);
					k = 0;
					continue;
				}
				else{
					//printf("bosluk degil\n");
					temp[k] = ch;
					k++;

				}
			}while(ch != EOF);
			
			fclose(fp);
			printf("Array Elements:\n");
			printArray(array, 7);
			printf("\n");
			
			int n = sizeof(array)/sizeof(array[0]);
		
			double execTime;
			double signalReceiveTime;

			if(getpid() % 2 == 1) 
			{	
				start = clock();
				selectionSort(array, n);
			}
			else
			{
				start = clock();
				insertionSort(array, n);
			}			
			end = clock();

			printf("Array Elements After Sorting:\n");
			printArray(array, n);
			printf("\n");

			execTime = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("Execution time:%f\n ", execTime );

			sprintf(outfilename, "output%d.txt", childId);
			sprintf(myoutfilename, "myoutput%d.txt", childId);
			FILE *fs = fopen(outfilename, "w");
			FILE *myfile = fopen(myoutfilename, "w");
			
			for(i = 0; i < 7; i++)
			{
				fprintf(fs, "%d ", array[i]);
				
			}
			
			int waitTime = rand()%7+1;
			fprintf(fs, "\n");
			fprintf(fs, "%f", waitTime + execTime);
			fprintf(myfile, "%f", waitTime + execTime);
			fprintf(fs, "\n");
			fprintf(myfile, "\n");

			for(i = 0; i < 7; i++)
			{
				fprintf(myfile, "%d ", array[i]);
				
			}
	
			sleep(waitTime);
			
			fprintf(myfile, "\n");

			if(getpid() % 2 == 1){
				fprintf(fs, "%s", "SIGUSR1");
				fprintf(myfile, "%s", "SIGUSR1");
			}
			else{
				fprintf(fs, "%s", "SIGUSR2");
				fprintf(myfile, "%s", "SIGUSR2");
			}
			fprintf(fs, " ");
			fprintf(myfile, "\n");

			fprintf(fs, "%d", ptm->tm_hour);
			fprintf(fs, "%s", ":");
			fprintf(fs, "%d", ptm->tm_min);
			fprintf(fs, "%s", ":");
			fprintf(fs, "%d", ptm->tm_sec);

			fprintf(myfile, "%d", ptm->tm_hour);
			fprintf(myfile, "%s", ":");
			fprintf(myfile, "%d", ptm->tm_min);
			fprintf(myfile, "%s", ":");
			fprintf(myfile, "%d", ptm->tm_sec);
			
			fclose(fs);
			fclose(myfile);
			exit(0);

		}
		else
		{

			childId++;

			
			int returnStatus;
			waitpid(cid, &returnStatus, 0);
			if(cid % 2 == 1){
				printf("killing cid:%d\n", cid);
				kill(cid, SIGUSR1);
			}
			else{
				printf("killing cid:%d\n", cid);
				kill(cid, SIGUSR2);
			}

			
		}
	}
		
			FILE *fg;
			char pOutfilename[100];
			char cha;
			float execTimeArray[childNum];
			char temp2[10];
			char signalName[10];
			float fnum = 0.0;
			int inum = 0;
			char sigReceiveTime[10];
			int k2 = 0;
			int array2[7];
			int s = 0;
			

			for(int i = 1; i <= childNum; i++)
			{
				sprintf(pOutfilename, "output%d.txt", i);
				fg = fopen(pOutfilename, "r");
				cha = fgetc(fg);
				while(cha != '\n') // find the second line to read execTime
				{			
					cha = fgetc(fg);
				}

				do
				{
					cha = fgetc(fg); // get to the second line
					temp2[k2] = cha;
					k2++;
				
				}while(cha != '\n');
				
				k2 = 0;	
				fnum = calculateNumberFloat(temp2, k2);
				execTimeArray[s] = fnum;
				s++;

				cleanArray(temp2);
						
					
			}
				int newSize = childNum;
				removeDuplicates(execTimeArray, &newSize);
				insertionSort(execTimeArray, newSize);
				printf("execTimeArray: ");
				printArrayFloat(execTimeArray, newSize);



			FILE* fe;
			FILE* fm;

			fm = fopen("output.txt", "w");
			
			for(int j = 0; j < newSize; j++)
			{
				
				int si = 0;


				for(int r = 1; r <= childNum; r++)
				{

					sprintf(pOutfilename, "myoutput%d.txt", r);	

			  
					fe = fopen(pOutfilename, "r");
					
					do // get execTime
					{
						cha = fgetc(fe);
						temp2[k2] = cha;
						k2++;
					}while(cha != '\n');
					
					fnum = calculateNumberFloat(temp2, k2);
					k2 = 0;	
					cleanArray(temp2);
					printf("fnum:%f\n", fnum);

					printf("exectimearray[j]: %f and fnum: %f\n", execTimeArray[j], fnum);

					
					if(execTimeArray[j] == fnum)
					{
						fprintf(fm, "%f", fnum);

					int jj = 0;
					do // get sortedArray
					{
						
						cha = fgetc(fe);
						if(cha == ' '){
							//printf("bosluk\n");
							inum = calculateNumber(temp2, k2);
							array2[jj] = inum;
							jj++;
							//printf("inum:%d\n", inum);
							cleanArray(temp2);
							k2 = 0;
							continue;
						}
						else{
							//printf("bosluk degil\n");
							temp2[k2] = cha;
							k2++;

						}
					}while(cha != '\n');
					
					k2 = 0;	
					cleanArray(temp2);
					//printArray(array2, 7);
					//cha = fgetc(fe);
					


					do // get signal name
					{
						cha = fgetc(fe);
						temp2[k2] = cha;
						k2++;
					}while(cha != '\n');
					
					memcpy(signalName, temp2, 7);
					k2 = 0;	
					cleanArray(temp2);
					printf("signal name:%s\n", signalName);

					//cha = fgetc(fe);
					do // get signal receive time
					{
						cha = fgetc(fe);
						temp2[k2] = cha;
						k2++;
					}while(cha != EOF);
					
					memcpy(sigReceiveTime, temp2, 8);
					k2 = 0;	
					cleanArray(temp2);
					printf("signal receive time:%s\n", sigReceiveTime);

					fprintf(fm, " ");
					for(int i = 0; i < 7; i++)
					{
						fprintf(fm, "%d ", array2[i]);
					}
					fprintf(fm, " ");
					fprintf(fm, "%s", signalName);
					fprintf(fm, " ");
					fprintf(fm, "%s", sigReceiveTime);
					fprintf(fm, "\n");



					}				

				
				}
				



				
			}

		fclose(fe);
		fclose(fm);		
			
			

	return 0;
}





