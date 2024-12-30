#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>

#define EXPRESSION_SIZE   64
#define MAX_TASKS_NUMBER  100
#define MAX_NUMBER        100

typedef struct {
    char expression[EXPRESSION_SIZE];
    int correct_answer;
    int client_answer;
    double response_time;
} Task;

typedef struct {
    Task tasks[MAX_TASKS_NUMBER];
    int total_tasks;
    int correct_task;
    double correct_percent;  
    double total_time;
} Stat;

typedef struct {
    long mtype;
    Task task;
} Message;

Stat* stat;
int sem_id;
const char operators[] = "+-*/%^";

int generate_random_number();
void generate_task(Task* const task, int previous_result);