#include<stdio.h>
#include<stdlib.h>
#include<string.h> 
#include<pthread.h>

int n1 = 0, n2 = 0;
void mergeSortedArrays(int arr1[], int arr2[], int n1, int n2, int arr3[])
{
    int i = 0, j = 0, k = 0;
    while (i<n1 && j <n2) // go through both array
    {
        if (arr1[i] < arr2[j]){
            arr3[k] = arr1[i];
            k++;
            i++; 
        }
        else
        {
            arr3[k] = arr2[j];
            k++;
            j++;
        }
    }
    // to deal with left over elements
    while (i < n1) {
        arr3[k++] = arr1[i++];
    }
    while (j < n2) {
        arr3[k++] = arr2[j++];
    }
}
// struct arg_struct { // to help me with pushing two arguments into insertionsort()
//     int * arg1;
//     int arg2;
// };

void* InsertionSortN1(void *arg) //pthread expect a function with void * output AND as input, or else you get warning, it would have been cleaner if i used one insertionsort to deal with sorting odd lengths, but due to time constrainst I couldnt fix a bug when i passed in struct with two arguements (one array and one int) to the function, so since I could only correctly use one arguement I had to do this way, while using size of arrays, N1,N2 as global variable
// the runtime should not be affected by thsi method since i do parallel threading on these functions anyways
{
int * arr1 = (int*) arg;
   int m=0;
    int n= 0;
    int temp = 0;
    for(m=0;m<n1;m++){ //n1 here is global variable, keeping track of size of second array
      temp=arr1[m];
      n=m-1;
      while((temp<arr1[n])&&(n>=0)){
         arr1[n+1]=arr1[n];
         n=n-1;
      }
      arr1[n+1]=temp;
   }
    return NULL;
}
void* InsertionSortN2(void *arg){ //pthread expect a function with void * output AND as input, or else you get warning 
int * arr1 = (int*) arg;
   int m=0;
    int n= 0;
    int temp = 0;
    for(m=0;m<n2;m++){
      temp=arr1[m];
      n=m-1;
      while((temp<arr1[n])&&(n>=0)){
         arr1[n+1]=arr1[n];
         n=n-1;
      }
      arr1[n+1]=temp;
   }
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc <= 1){ // this is for piping
    FILE* file = fopen("/dev/stdin","r");
    int i = 0;
    int j = 0;
    int count = 0;
    int array[10000] = {0};
    int array1[10000] = {0};
    int storeFileNumbers[10000] = {0};  
    int numbers[10000] = {0};
    pthread_t threads[2]; // 2 thread for each half of sorting, its starts counting from 1 i think
    while (!feof (file)) {  //while not end of file
        fscanf (file, "%d", &i);      
        storeFileNumbers[j] = i; //stores what is read from file
        j++;
    }
    count = j; // total number of element in file
    fclose (file);     
    
    for (int i = 0; i < j/2; i++) {     
        array[i] = storeFileNumbers[i];   //copy over first half of int  
    }  
    
    memcpy (array1,storeFileNumbers+(count/2),((count/2)+1) * sizeof( int )); // copy over second half of int
    
    n1 = count/2; // the number of element in each half array
    n2 = count - (count/2);

//    struct arg_struct args; // my attempt at passing a struct to insertionsort()
//     args.arg1 = array;
//     args.arg2 = n1;
    pthread_create(&threads[0], NULL, InsertionSortN1, (void *)array); //deal with first sublist
    pthread_create(&threads[1], NULL, InsertionSortN2, (void *)array1); //deal with second sublist

    //I wait here for threads to finish
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
   // I combine the two sorted sub array using merge
   mergeSortedArrays(array, array1, n1, n2, numbers);
    // printing to terminal the final output
    for(int i=0; i<count; i++) {
        if (i != count-1) {
            if (numbers[i] != numbers [i+1] ){ // added one otherwise 0 wont be printed since it will be compared to -1
                    printf("%d ", numbers[i]);
                }
        } else {
            printf("%d ", numbers[i]);
        }
    }
    return 0;
  } // so this is for piping/when no arg passed to psort, if not piped this never runs

  if(argc == 2){ //this is if the user enters psort input.txt, same logic as everything above
    FILE* file = fopen (argv[1], "r");  //read the file I pass in 
    int i = 0;
    int j = 0;
    int count = 0;
    int array[10000] = {0};
    int array1[10000] = {0};
    int storeFileNumbers[10000] = {0};  
    int numbers[10000] = {0};
    pthread_t threads[2];

    while (!feof (file))
    {  
        fscanf (file, "%d", &i);      
        storeFileNumbers[j] = i;
        j++;
    }
    count = j;
    fclose (file); 

    for (int i = 0; i < j/2; i++) {     
        array[i] = storeFileNumbers[i];     
    }  

    memcpy (array1,storeFileNumbers+(count/2),((count/2)+1) * sizeof( int ));

        n1 = count/2;
        n2 = count - (count/2);

    pthread_create(&threads[0], NULL, InsertionSortN1, (void*)array); //deal with first sublist

    pthread_create(&threads[1], NULL, InsertionSortN2, (void*)array1); //deal with second sublist


    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    mergeSortedArrays(array, array1, n1, n2, numbers);

    for(int i=0; i<count; i++) {
        if (i != count-1) {
            if (numbers[i] != numbers [i+1] ){ // added one otherwise 0 wont be printed since it will be compared to -1
                    printf("%d ", numbers[i]);
                }
        } else {
            printf("%d ", numbers[i]);
        }
    } 

  } // end of argv condition
  
  return 0;
} //end of main()

