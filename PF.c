#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <string.h>


#define wordLenght 20
#define definitionLenght 200
#define wordsInArchive 29
#define N_THREADS 6
// create semaphore
sem_t semaphore_id;

int wordsNum;
int definitionsNum;
int saveIntrscIndxH=0;
int saveIntrcIndcV=0;

// array to know where the used words are
int usedWordIndx[wordsInArchive];
//array and matrix of word and definition
char words[wordsInArchive][wordLenght];
char definitions[wordsInArchive][definitionLenght];

// read words from archive
void readWords()
{
	wordsNum = 0;
	FILE *fd = fopen ("palabras_proyectoFinal.txt", "r");
	
	while(fscanf(fd, "%s", words[wordsNum]) != EOF){
		wordsNum++;
	}
	fclose(fd);
	
	
	printf("número total de palabras %d\n",wordsNum);
	/*for(int i=0 ; i<wordsNum; i++){
		printf("numero %d: %s\n",i+1, words[i]);
	}*/
}
// read definitions from archive
void readDefinitions()
{
	definitionsNum = 0;
	FILE *fd = fopen ("definiciones_proyectoFinal.txt", "r");
	
    //arreglo donde van las definiciones
	char definitionsArray[definitionLenght];
	
	while(fgets(definitionsArray, definitionLenght, fd) != NULL)
    {
		definitionsArray[strcspn(definitionsArray, "\n")] = '\0';
		strcpy(definitions[definitionsNum], definitionsArray);
		definitionsNum++;
	}
	
	fclose(fd);
	
	printf("número total de definiciones %d\n",definitionsNum);
	/*for(int i=0 ; i<definitionsNum; i++){
		printf("sentence numero %d: %s\n",i+1, definitions[i]);
	}*/
}

int horizontalWordIndx[3];
int actualThreadH = 0;
// thread to choose a word horizontally
void* horizontally(void* arg)
{
    srand(time(NULL));
	int flag;
	int rH;
	sem_wait(&semaphore_id);
	flag = 0; 
	while(flag == 0)
	{
		rH = rand() % wordsInArchive;
		// when usedIndexH = 1 its bc that word is already used
		if(usedWordIndx[rH]== 0)
		{
			usedWordIndx[rH] = 1;
			horizontalWordIndx[actualThreadH] = rH;
			printf("horizontal: %d \n", rH);
            flag = 1;
            actualThreadH++;
		}
	}
	sem_post(&semaphore_id);
    pthread_exit( NULL );
}
int verticalWordIndx [3];
int actualThreadV = 0;
// thread to choose a word vertically
void* vertically(void* arg) 
{
    srand(time(NULL));
	int flag;
	int rV;
	sem_wait(&semaphore_id);
	flag = 0;
	while(flag == 0)
	{
		rV = rand() % wordsInArchive;
		// when usedIndexH = 1 its bc that word is already used
		if(usedWordIndx[rV]!= 1)
		{
			usedWordIndx[rV] = 1;
			verticalWordIndx[actualThreadV] = rV;
			printf("vertical: %d \n", rV);
			flag = 1;
			actualThreadV++;
		}
	}
	sem_post(&semaphore_id);
    pthread_exit( NULL );
}
int currentRowWordH = 0; int currentRowWordV = 0;
// find intersections between words
void findIntersections ()
{
    int found = 0;
	// the lenght of the first two words
	int wordLenght1 = strlen (words[verticalWordIndx[currentRowWordV]]);
	printf("V: %d \n", verticalWordIndx[currentRowWordV]);
	int wordLenght2 = strlen (words[horizontalWordIndx[currentRowWordH]]);
	printf("H: %d \n", horizontalWordIndx[currentRowWordH]);
	//find the intersections
	for (int i = 0; i < wordLenght2; i++)
	{
	    for(int j = 0; j < wordLenght1; j++)
	    {
	        if(words[horizontalWordIndx[currentRowWordH]][i]== words[verticalWordIndx[currentRowWordV]][j])
	        {
	            found = 1;
				printf("Intersección en la posición %d de la palabra %s y %d de la palabra %s.\n", i,words[horizontalWordIndx[0]],j,words[verticalWordIndx[0]] );
				saveIntrcIndcV = j;
				saveIntrscIndxH = i;
				i = wordLenght2; j = wordLenght1;
			}
		}
	}
}

int main()
{
	readWords();
	readDefinitions();

   pthread_t thread_1[3], thread_2[3];
   sem_init(&semaphore_id, 0, 1);

	// create threads
   for(int i = 0; i < 3; i++)
   {
        pthread_create( &thread_1[i], NULL, horizontally, NULL );
		pthread_create( &thread_2[i], NULL, vertically, NULL );
   }
   // wait for threads
   for(int i = 0; i < 3; i++)
   {
		pthread_join( thread_1[i], NULL );
		pthread_join( thread_2[i], NULL );
   }
   
   findIntersections();
   sem_destroy(&semaphore_id);
	return 0;
}