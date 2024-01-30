#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int input_array[4];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//making three threads 
pthread_cond_t sum_check = PTHREAD_COND_INITIALIZER;
pthread_cond_t even_or_odd_check = PTHREAD_COND_INITIALIZER;
pthread_cond_t mul_check = PTHREAD_COND_INITIALIZER;
int sum_Count = 0, Even_or_odd_count = 0,  mul_count = 0;

//summation method:
void *sum(void *thread_id) {
    pthread_mutex_lock(&mutex);


    printf("thread_1 (TID %lu) reads X = %d and Y = %d from input_array[]\n", pthread_self(), input_array[0], input_array[1]);

    //reading inputs and then summing them. 
    //saving the result in input_array[2]
    input_array[2] = input_array[0] + input_array[1];
    printf("thread_1 (TID %lu) writes X + Y = %d to the input_array[2]\n", pthread_self(), input_array[2]);
    
    sum_Count = 1;
    pthread_cond_signal(&sum_check);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
//even or odd method
//inside thread_2
void *even_odd(void *thread_id) {
    pthread_mutex_lock(&mutex);
    while(!sum_Count) {
        pthread_cond_wait(&sum_check, &mutex);
    }
    printf("thread_2 (TID %lu) reads %d from the input_array[2]\n", pthread_self(), input_array[2]);

    //reading the sum of two integers from before and then identifying whether its even or odd
    if(input_array[2] % 2 == 0) {
        printf("thread_2 (TID %lu) identifies that %d is an even number\n", pthread_self(), input_array[2]);
    } else {
        printf("thread_2 (TID %lu) identifies that %d is an odd number\n", pthread_self(), input_array[2]);
    }
    Even_or_odd_count = 1;
    pthread_cond_signal(&even_or_odd_check);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
//multiplicaion method
//inside thread_2
void *multiplication(void *thread_id) {
    pthread_mutex_lock(&mutex);
    while(!Even_or_odd_count) {
        pthread_cond_wait(&even_or_odd_check, &mutex);
    }
    printf("thread_2 (TID %lu) reads X and Y from input_array[],", pthread_self());

    //reading the two initial integers and multiplying them together
    //addin this resulting number to input_array[3]
    input_array[3] = input_array[0] * input_array[1];
    printf("writes X * Y = %d to input_array[3]\n", input_array[3]);


    mul_count = 1;
    pthread_cond_signal(&mul_check);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

//reverse method
//inside thread_2
void *reverse_num(void *thread_id) {
    pthread_mutex_lock(&mutex);
    while(!mul_count) {
        pthread_cond_wait(&mul_check, &mutex);
    }
    printf("thread_3 (TID %lu) reads %d from input_array[3]\n", pthread_self(), input_array[3]);

    int first = input_array[3];
    int after_reverse = 0;
    ////reading the multiplication result from array 3 and reversing number
    while(input_array[3] != 0) {
        //using this method to reverse the number 
        //shifting after_reverse one digit to the left to make room for the next digit
        after_reverse = after_reverse * 10 + input_array[3] % 10;
        input_array[3] /= 10;
    }
    printf("thread_3 (TID %lu) reverses the number %d  ->  %d\n", pthread_self(), first, after_reverse);
    pthread_mutex_unlock(&mutex);
    return NULL;
}


int main(int argc, char *argv[]) {

    pthread_t tid_sum, tid_even_or_odd, tid_mul;

    //putting the inputs (two numbers) into array[0] and array[1]
    input_array[0] = atoi(argv[1]);
    input_array[1] = atoi(argv[2]);

    printf("parent (PID %d) receives X = %d and Y = %d from the user\n", getpid(), input_array[0], input_array[1]);
    printf("parent (PID %d) writes X = %d and Y = %d to input_array[]\n", getpid(), input_array[0], input_array[1]);

    //creating threads
    pthread_create(&tid_sum, NULL, sum, NULL);
    pthread_create(&tid_even_or_odd, NULL, even_odd, NULL);
    pthread_create(&tid_mul, NULL, multiplication, NULL);
    pthread_create(&tid_mul, NULL, reverse_num, NULL);

    //using join_thread to make sure OS is going to go through threads
    pthread_join(tid_sum, NULL);
    pthread_join(tid_even_or_odd, NULL);
    pthread_join(tid_mul, NULL);

    //used for destroying mutex and condition_variables in multi-thread
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&sum_check);
    pthread_cond_destroy(&even_or_odd_check);
    pthread_cond_destroy(&mul_check);

    return 0;
}