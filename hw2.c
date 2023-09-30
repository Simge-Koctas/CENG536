#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

int** matrix; // for first part
int** matrix1; // for second part
int** matrix2; // for second part
int** matrix3; // for third part
int** matrix4; // for third part
int** result2; // for second part
int** result3; // for third part

int threadCount = 0;
int threadNum = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;

double execTime;
clock_t exec_start, exec_stop;

int M_2, N_2;
int M_3, N_3, K_3;


void printArray(int arr[], int n)
{
	for(int i = 0; i < n; i++)
	{
		printf("%d ", arr[i]);
	}
}


int calculateNumber(char temp[])
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


typedef struct 
{
	int id;
	int shiftNum;
	int m;
}tParameter;



void print2darray(int** array, int m)
{
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < m; j++)
		{
			printf("%d ", array[i][j]);
		}// for j
		printf("\n");
	}//for i
}

void print2darray2(int** array, int r, int c)
{
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			printf("%d ", array[i][j]);
		}// for j
		printf("\n");
	}//for i
}



void* shiftMatrix(void* parameter)
{

	int start = 0;
	int end = 0;
	tParameter *tp; 
	tp = (struct tParameter*) parameter;
	
	//printf("tp->id:%d\n", tp->id);
	start = (tp->id)*((tp->m)/threadNum);
	end = ((tp->id)+1)*((tp->m) / threadNum)-1;
	//printf("start:%d\n", start);
	
	if(((tp->id) + 1 == threadNum) && ((tp->m) % threadNum) != 0) // if last thread
	{
		
		end = (tp->m)-1;
		
	}
	//printf("end:%d\n", end);	

	
	printf("inside shiftMatrix I am %d and start: %d\n", pthread_self(), start);
	printf("inside shiftMatrix I am %d and end: %d\n", pthread_self(), end);
	//printf("inside shiftMatrix threadCount: %d\n", threadCount);

	//shifting rows
	int c = 0;
	printf("I am %d and before shifting rows:\n", pthread_self());
	print2darray(matrix, tp->m);
	
	//exec_start = clock();
	//shift from left to right
	for(int i = start; i <= end; i++) 
	{
		while(c != tp->shiftNum)
		{
			int last = matrix[i][(tp->m)-1];
			for(int k = (tp->m)-1; k > 0; k--)
			{
				//printf("c: %d\n", c);
				//printf("tp->matrix[i][k] and tp->matrix[i][k-1]: %d  %d\n", tp->matrix[i][k],  tp->matrix[i][k-1]);
				//tp->matrix[i][k] = tp->matrix[i][k-1];
				matrix[i][k] = matrix[i][k-1];
				
				
				
			}// k
			c++;
			//tp->matrix[i][0] = last;
			matrix[i][0] = last;
		}//while
		c = 0;
	
	} // i
	c = 0;
	
	pthread_mutex_lock(&lock);
	threadCount++;
	printf("I am %d and after shifting rows\n", pthread_self());
	print2darray(matrix, tp->m);
	
	if(threadCount == threadNum)
		pthread_cond_broadcast(&cond);
	else
		pthread_cond_wait(&cond, &lock);
	pthread_mutex_unlock(&lock);
	
	printf("I am %d and before shifting columns\n", pthread_self());
	print2darray(matrix, tp->m);

	//printf("inside shiftMatrix I am %d and start: %d\n ", pthread_self(), start);
	//printf("inside shiftMatrix I am %d and end: %d\n ", pthread_self(), end);

	//shift from bottom to top
	for(int i = start; i <= end; i++) 
	{
		while(c != tp->shiftNum)
		{
			int first = matrix[0][i];
			//printf("first: %d\n", first);
			for(int k = 1; k < (tp->m); k++)
			{
				//printf("c: %d\n", c);
				//printf("tp->matrix[i][k] and tp->matrix[i][k-1]: %d  %d\n", tp->matrix[i][k],  tp->matrix[i][k-1]);
				//tp->matrix[i][k] = tp->matrix[i][k-1];
				matrix[k-1][i] = matrix[k][i];
				
				
				
			}// k
			c++;
			//tp->matrix[i][0] = last;
			matrix[(tp->m)-1][i] = first;
		}//while
		c = 0;
	
	} // i
	
	//exec_stop = clock();
	printf("I am %d and after shifting columns:\n", pthread_self());
	print2darray(matrix, tp->m);
		
	pthread_exit(NULL);
	
	

} //shiftMatrix



void* multMatrix(void* parameter)
{

	int start = 0;
	int end = 0;
	tParameter *tp; 
	tp = (struct tParameter*) parameter;
	
	//printf("tp->id:%d\n", tp->id);	


	printf("inside multMatrix I am %d:\n", pthread_self());

	//multiplication
	int sum = 0;
	
	//exec_start = clock();
	for(int i = (tp->id); i < M_3; i= i+M_3) 
	{
		
		for(int j = 0; j < K_3; j++)
		{
			sum = 0;
			for(int k = 0; k < N_3; k++)
			{
				sum += matrix3[i][k] * matrix4[k][j];
			}//k
			pthread_mutex_lock(&lock3);
			result3[i][j] = sum;
			pthread_mutex_unlock(&lock3);
					
		}// j
		
		
	} // i
	//exec_stop = clock();
	printf("I am %d and after multiplication:\n", pthread_self());
	print2darray2(result3, M_3, K_3);
	
	pthread_exit(NULL);
	
	

} //multMatrix


void* sumMatrix(void* parameter)
{

	int start = 0;
	int end = 0;
	tParameter *tp; 
	tp = (struct tParameter*) parameter;
	
	//printf("tp->id:%d\n", tp->id);	


	printf("inside sumMatrix I am %d:\n", pthread_self());

	//multiplication
	int sum = 0;
	
	//exec_start = clock();
	for(int i = (tp->id); i < M_2; i= i+M_2) 
	{
		for(int j = 0; j < N_2; j++)
		{
			sum = matrix1[i][j] + matrix2[i][j];
	
			pthread_mutex_lock(&lock2);
			result2[i][j] = sum;
			pthread_mutex_unlock(&lock2);
					
		}// j
		sum = 0;
		
	} // i
	//exec_stop = clock();

	printf("I am %d and after summation:\n", pthread_self());
	print2darray2(result2, M_2, N_2);
	
	pthread_exit(NULL);
	
	

} //sumMatrix



int m = 0;


int main()
{
	
	int shiftNum = 0;
	printf("Enter the number of threads to create: ");
	scanf("%d", &threadNum);
	printf("Enter the number of shifts: ");
	scanf("%d", &shiftNum);

	FILE* fp;
	char ch;
	printf("Contents of the input file:\n ");
	fp = fopen("input.txt", "r");
	//fp = fopen("inputtest.txt", "r");
	//fp = fopen("inputtest2.txt", "r");
 	char temp[10]; 
	int temp2[2];
	int num = 0;
	int count = 0;
	int j = 0;
	int k = 0;


	do // get m from input file for first part
	{
		ch = fgetc(fp);
		temp[k] = ch;
		k++;
	}while(ch != '\n');
					
	m = calculateNumber(temp);
	k = 0;	
	cleanArray(temp);
	printf("m:%d\n", m);

	matrix = malloc(m*sizeof(int*));
	for(int i = 0; i < m; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		matrix[i] = malloc(m*sizeof(int));
	}

	while(count != m) // get the array elements of first part
	{

		do 
		{
			ch = fgetc(fp);
			//printf("%c\n", ch);
			if(ch == ' ' || ch == EOF || ch == '\n'){
				//printf("bosluk\n");
				num = calculateNumber(temp);
				matrix[count][j] = num;
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
		}while(ch != '\n');
		count++;
		j = 0;
	} // while

	k = 0;	
	j = 0;
	cleanArray(temp);

	do // get M and N for the second part
	{
		ch = fgetc(fp);
		//printf("%c\n", ch);
		if(ch == ' ' || ch == EOF || ch == '\n'){
			//printf("bosluk\n");
			num = calculateNumber(temp);
			temp2[j] = num;
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
	}while(ch != '\n');
	
	M_2 = temp2[0];
	N_2 = temp2[1];
	//printf("M_2:%d\n", M_2);
	//printf("N_2:%d\n", N_2);

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);

	matrix1 = malloc(M_2*sizeof(int*));
	for(int i = 0; i < M_2; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		matrix1[i] = malloc(N_2*sizeof(int));
	}
	
	
	count = 0;
	while(count != M_2) // get the array elements of second part of MxN array
	{

		do 
		{
			ch = fgetc(fp);
			//printf("%c\n", ch);
			if(ch == ' ' || ch == EOF || ch == '\n'){
				//printf("bosluk\n");
				num = calculateNumber(temp);
				matrix1[count][j] = num;
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
		}while(ch != '\n');
		count++;
		j = 0;
	} // while

	k = 0;	
	j = 0;
	cleanArray(temp);

////////////////////////////////////////////////////////

	do // get M and N for the second part again but dont do anything, just read
	{
		ch = fgetc(fp);
		//printf("%c\n", ch);
		if(ch == ' ' || ch == EOF || ch == '\n'){
			//printf("bosluk\n");
			num = calculateNumber(temp);
			temp2[j] = num;
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
	}while(ch != '\n');

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);

	matrix2 = malloc(M_2*sizeof(int*));
	for(int i = 0; i < M_2; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		matrix2[i] = malloc(N_2*sizeof(int));
	}
	
	count = 0;
	while(count != M_2) // get the array elements of second part of MxN array
	{

		do 
		{
			ch = fgetc(fp);
			//printf("%c\n", ch);
			if(ch == ' ' || ch == EOF || ch == '\n'){
				//printf("bosluk\n");
				num = calculateNumber(temp);
				matrix2[count][j] = num;
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
		}while(ch != '\n');
		count++;
		j = 0;
	} // while

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);

	//printf("array 1 of second part:\n");
	//print2darray2(matrix1, M_2, N_2);
	//printf("array 2 of second part:\n");
	//print2darray2(matrix2, M_2, N_2);

/////////////////////////////////////////////// ---------


	do // get M and N for the third part
	{
		ch = fgetc(fp);
		//printf("%c\n", ch);
		if(ch == ' ' || ch == EOF || ch == '\n'){
			//printf("bosluk\n");
			num = calculateNumber(temp);
			temp2[j] = num;
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
	}while(ch != '\n');
	
	M_3 = temp2[0];
	N_3 = temp2[1];

	//printf("M_3:%d\n", M_3);
	//printf("N_3:%d\n", N_3);

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);

	matrix3 = malloc(M_3*sizeof(int*));
	for(int i = 0; i < M_3; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		matrix3[i] = malloc(N_3*sizeof(int));
	}
	
	count = 0;
	while(count != M_3) // get the array elements of third part of MxN array
	{

		do 
		{
			ch = fgetc(fp);
			//printf("%c\n", ch);
			if(ch == ' ' || ch == EOF || ch == '\n'){
				//printf("bosluk\n");
				num = calculateNumber(temp);
				matrix3[count][j] = num;
				printf("matrix3[count][j]:%d"",%d"",%d"",\n", matrix3[count][j], count,j);
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
		}while(ch != '\n');
		count++;
		j = 0;
	} // while

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);


	//printf("array 1 of third part:\n");
	//print2darray2(matrix3, M_3, N_3);

////////////////////////////////////////////////////////

	do // get N and K for the third part 
	{
		ch = fgetc(fp);
		//printf("%c\n", ch);
		if(ch == ' ' || ch == EOF || ch == '\n'){
			//printf("bosluk\n");
			num = calculateNumber(temp);
			temp2[j] = num;
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
	}while(ch != '\n');

	N_3 = temp2[0];
	K_3 = temp2[1];

	//printf("N_3:%d\n", N_3);
	//printf("K_3:%d\n", K_3);

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);

	matrix4 = malloc(N_3*sizeof(int*));
	for(int i = 0; i < N_3; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		matrix4[i] = malloc(K_3*sizeof(int));
	}
	
	count = 0;
	while(count != N_3) // get the array elements of third part of NxK array
	{

		do 
		{
			ch = fgetc(fp);
			//printf("%c\n", ch);
			if(ch == ' ' || ch == EOF || ch == '\n'){
				//printf("bosluk\n");
				num = calculateNumber(temp);
				matrix4[count][j] = num;
				printf("matrix4[count][j]:%d"",%d"",%d"",\n", matrix4[count][j], count,j);
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
		}while(ch != '\n');
		count++;
		j = 0;
	} // while

	k = 0;	
	j = 0;
	cleanArray(temp);
	cleanArray(temp2);


	//printf("array 2 of third part:\n");
	//print2darray2(matrix4, N_3, K_3);





	printf("matrix from parent before shifting:\n");
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < m; j++)
		{
			printf("%d ", matrix[i][j]);
		} // ikinci for
		printf("\n");
	} // ilk for


	printf("matrix1 from parent before summation:\n");
	print2darray2(matrix1, M_2, N_2);

	printf("matrix2 from parent before summation:\n");
	print2darray2(matrix2, M_2, N_2);

	printf("matrix3 from parent before multiplication:\n");
	print2darray2(matrix3, M_3, N_3);

	printf("matrix4 from parent before multiplication:\n");
	print2darray2(matrix4, N_3, K_3);


	result2 = malloc(M_2*sizeof(int*));
	for(int i = 0; i < M_2; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		result2[i] = malloc(N_2*sizeof(int));
	}

	result3 = malloc(M_3*sizeof(int*));
	for(int i = 0; i < M_3; i++)
	{
		//tp.matrix[i] = malloc(m*sizeof(int));
		result3[i] = malloc(K_3*sizeof(int));
	}

	
	
	pthread_t tid[threadNum];
	int start, end = 0;
	exec_start = clock();
	for(int i = 0; i < threadNum; i++) // for shifting
	{	
		tParameter tp;
		/*
		tp.start = start;
		tp.end = start + (m / threadNum) - 1;
		if((i + 1 == threadNum) && (m % threadNum) != 0) // if last thread
		{
			//tp.end = start + (m / threadNum);
			tp.end = m-1;
		}
		start = tp.end + 1;
		*/
		tp.shiftNum = shiftNum; 
		tp.m = m;
		tp.id = i;
		
		pthread_create(&tid[i], NULL, shiftMatrix, (void*)&tp);	
			
		sleep(1);

	} // for thread
	exec_stop = clock();
	
	for(int i = 0; i < threadNum; i++)
	{	
		pthread_join(tid[i], NULL);	
	} // for thread wait
	
	printf("matrix from parent after shifting:\n");
	print2darray(matrix, m);

	execTime = ((double) (exec_stop - exec_start)) / CLOCKS_PER_SEC;
	printf("exec time of shifting with %d threads:%f\n", threadNum, execTime);

	exec_start = clock();
	threadNum = M_2;
	for(int i = 0; i < threadNum; i++)
	{	
		tParameter tp;
		tp.id = i;

		pthread_create(&tid[i], NULL, sumMatrix, (void*)&tp);		
		sleep(1);

	} // for thread
	exec_stop = clock();
	for(int i = 0; i < threadNum; i++)
	{	
		pthread_join(tid[i], NULL);	
	} // for thread wait

	printf("result from parent after summation:\n");
	print2darray2(result2, M_2, N_2);

	execTime = ((double) (exec_stop - exec_start)) / CLOCKS_PER_SEC;
	printf("exec time of summation with %d threads:%f\n", threadNum, execTime);

	exec_start = clock();
	threadNum = M_3;
	for(int i = 0; i < threadNum; i++)
	{	
		tParameter tp;
		tp.id = i;

		pthread_create(&tid[i], NULL, multMatrix, (void*)&tp);		
		sleep(1);

	} // for thread
	exec_stop = clock();
	for(int i = 0; i < threadNum; i++)
	{	
		pthread_join(tid[i], NULL);	
	} // for thread wait

	printf("result from parent after multiplication:\n");
	print2darray2(result3, M_3, K_3);

	execTime = ((double) (exec_stop - exec_start)) / CLOCKS_PER_SEC;
	printf("exec time of multiplication with %d threads:%f\n", threadNum, execTime);

	FILE *fs = fopen("output.txt", "w");
	fprintf(fs, "%d", m);
	fprintf(fs, "\n");
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < m; j++)
		{
			fprintf(fs, "%d ", matrix[i][j]);
		}
		fprintf(fs, "\n");		
	}
	//fprintf(fs, "\n");
	fprintf(fs, "%d ", M_2);
	fprintf(fs, "%d ", N_2);
	fprintf(fs, "\n");
	for(int i = 0; i < M_2; i++)
	{
		for(int j = 0; j < N_2; j++)
		{
			fprintf(fs, "%d ", result2[i][j]);
		}
		fprintf(fs, "\n");		
	}
	//fprintf(fs, "\n");
	fprintf(fs, "%d ", M_3);
	fprintf(fs, "%d ", K_3);
	fprintf(fs, "\n");
	for(int i = 0; i < M_3; i++)
	{
		for(int j = 0; j < K_3; j++)
		{
			fprintf(fs, "%d ", result3[i][j]);
		}
		fprintf(fs, "\n");		
	}
	
	pthread_mutex_destroy(&lock);
	pthread_mutex_destroy(&lock2);
	pthread_mutex_destroy(&lock3);
	fclose(fp);
	fclose(fs);
	free(matrix);
	free(matrix1);
	free(matrix2);
	free(matrix3);
	free(matrix4);
	return 0;
}//main





