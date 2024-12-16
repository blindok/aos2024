#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define EXPRESSION_SIZE   64
#define MAX_TASKS_NUMBER  100
#define MAX_NUMBER        100

const char operators[] = "+-*/%^";

typedef struct {
    char expression[EXPRESSION_SIZE];
    int correct_answer;
    int client_answer;
    double response_time;
} Task;

typedef struct {
    Task tasks[MAX_TASKS_NUMBER];
    int total_tasks;
    double correct_percent;  
} Stat;

typedef struct {
    long mtype;
    Task task;
} Message;

int generate_random_number();
void generate_task(Task* const task, int* const previous_result);