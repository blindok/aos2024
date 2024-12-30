#include "server.h"

#define LOG_FILE_NAME_SIZE  16
#define LOG_FILE_SIZE       64
#define SHM_KEY             1234
#define MSG_KEY             1235
#define MAX_CLIENTS         10
#define BUF_SIZE            256

typedef struct {
    int port;
    char log_file[LOG_FILE_NAME_SIZE];
} ServerConfig;

int log_fd = 0;
const char* log_file = NULL;
sigset_t sigset_hup;
ServerConfig config;

int read_config(ServerConfig* const config, const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        return 1;
    }

    char line[LOG_FILE_SIZE];
    char* cpy_res = NULL;
    while (fgets(line, LOG_FILE_SIZE, file)) {
        if (strncmp(line, "port=", 5) == 0) {
            config->port = atoi(line + 5);
        } else if (strncmp(line, "log_file=", 9) == 0) {
            cpy_res = strncpy(config->log_file, line + 9, LOG_FILE_NAME_SIZE - 1);
            config->log_file[strcspn(config->log_file, "\n")] = '\0';
        }
    }

    if (config->port == 0 || cpy_res == NULL || strlen(config->log_file) == 0) {
        return 2;
    }

    fclose(file);
    return 0;
}

void log_msg(const char* msg) {
    time_t rawtime = time(NULL);
    char* time_str = ctime(&rawtime);
    time_str[strlen(time_str)-1] = '\0';
    dprintf(log_fd, "[%s] %s\n", time_str, msg);
}

void sighup_handler() {
    int res = read_config(&config, log_file);
    if (res == 1) {
        fprintf(stderr, "[error] failed to parce server config\n");
    } else if (res == 2) {
        fprintf(stderr, "[error] invalid config\n");
    } else {
        printf("********************\nNEW CONFIG\nPort: %d\nLog file: %s\n********************\n", config.port, config.log_file);
    }
}

int setup_sighup_handler() {
    struct sigaction sa;
    sa.sa_handler = sighup_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL)) {
        return 1;
    }

    sigemptyset(&sigset_hup);
    sigaddset(&sigset_hup, SIGHUP);

    return 0;
}

int generate_random_number() {
    int num;
    FILE* urandom = fopen("/dev/urandom", "rb");
    fread(&num, sizeof(int), 1, urandom);
    fclose(urandom);
    return abs(num % MAX_NUMBER);
}

void generate_task(Task* const task, int previous_result) {
    int operand1 = (previous_result == INT_MIN) ? generate_random_number() : previous_result;
    int operand2 = generate_random_number();
    char operator = operators[generate_random_number() % (sizeof(operators) / sizeof(char))];

    memset(task->expression, 0, EXPRESSION_SIZE);
    //snprintf(task->expression, EXPRESSION_SIZE, "%d %c %d = ", operand1, operator, operand2);
    sprintf(task->expression, "%d %c %d = ", operand1, operator, operand2);

    switch (operator) {
        case '+': task->correct_answer = operand1 + operand2; break;
        case '-': task->correct_answer = operand1 - operand2; break;
        case '*': task->correct_answer = operand1 * operand2; break;
        case '/': task->correct_answer = operand1 / operand2; break;
        case '%': task->correct_answer = operand1 % operand2; break;
        case '^': task->correct_answer = (int)pow(operand1, operand2); break;
    }
}

int init_shared_memory() {
    //GET SHMID GENERATED USING FTOK
    int shmid = shmget(SHM_KEY, sizeof(Stat), IPC_CREAT | 0666);
    if (shmid < 0) return 1;
    stat = (Stat*)shmat(shmid, NULL, 0);
    if (stat == NULL) return 1;
    memset(stat, 0, sizeof(Stat));
    return 0;
}

void disable_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(Stat), IPC_CREAT | 0666);
    shmdt(stat);
    shmctl(shmid, IPC_RMID, NULL);
}

void sem_lock() {
    struct sembuf sem_op = {0, -1, SEM_UNDO};
    semop(sem_id, &sem_op, 1);
}

void sem_unlock() {
    struct sembuf sem_op = {0, 1, SEM_UNDO};
    semop(sem_id, &sem_op, 1);
}

void shut_down() {
    log_msg("*****SHUT UP SESSION*****");
    close(log_fd);
}

int init_message_queue(int* const msg_id) {
    *msg_id = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (*msg_id == -1) {
        return 1;
    }
    return 0; 
}

void close_message_queue(int msg_id) {
    msgctl(msg_id, IPC_RMID, NULL);
}

void handle_answer(int msg_id) {
    Message msg;
    msgrcv(msg_id, &msg, sizeof(msg.task), 1, 0);

    int is_correct = (msg.task.client_answer == msg.task.correct_answer);

    sem_lock();
    stat->tasks[stat->total_tasks] = msg.task;
    stat->total_tasks++;
    if (is_correct) stat->correct_task++;
    stat->total_time += msg.task.response_time;
    sem_unlock();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage format: %s <config file>\n", argv[0]);
        return 1;
    }

    log_file = argv[1];

    int res = read_config(&config, log_file);
    if (res == 1) {
        fprintf(stderr, "[error] failed to parce server config\n");
        return 1;
    } else if (res == 2) {
        fprintf(stderr, "[error] invalid config\n");
        return 1;
    }

    printf("********************\nPort: %d\nLog file: %s\n********************\n", config.port, config.log_file);

    log_fd = open(config.log_file, O_CREAT | O_WRONLY | O_APPEND, 0666);

    log_msg("*****START NEW SESSION*****");

    pid_t pid = fork();
    if (pid > 0) {
        exit (EXIT_SUCCESS);
    } else if (pid < 0) {
        fprintf(stderr, "[error] failed to fork\n");
        exit(EXIT_FAILURE);
    }

    if (setsid() < 0) {
        fprintf(stderr, "[error] failed to create a new session\n");
        exit(EXIT_FAILURE);
    }

    log_msg("creates a new session");

    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    // pid = fork();
    // if (pid > 0) {
    //     exit (EXIT_SUCCESS);
    // } else if (pid < 0) {
    //     fprintf(stderr, "[error] failed to fork\n");
    //     exit(EXIT_FAILURE);
    // }

    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDWR);
    dup(0); // STDOUT_FILENO
    dup(0); // STDERR_FILENO

    log_msg("redirects the stabdart fd's");

    setup_sighup_handler();

    if (init_shared_memory()) {
        log_msg("[error] failed to init shared memory");
        shut_down();
        return 1;
    }

    log_msg("successufully inits shared memory");

    struct sockaddr_in server_addr;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        log_msg("[error] failed to create a socket");
        return 1;
    }

    log_msg("created socket");

    int msg_id;
    if (init_message_queue(&msg_id)) {
        log_msg("[error] failed to create a massage queue");
        return 1;
    }

    log_msg("created message queue");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_msg("[error] failed to bind socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    log_msg("binded socket");

    if (listen(server_socket, MAX_CLIENTS) < 0) {
        log_msg("[error] failed to switch socket to listen mode");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    char buffer[BUF_SIZE];
    sprintf(buffer, "server is running on port %d...", config.port);
    log_msg(buffer);

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int previous_result = INT_MIN;

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        log_msg("client connected");

        read(client_socket, buffer, BUF_SIZE);

        struct timespec start, end;
        if (strncmp(buffer, "start", 5) == 0) {
            int quantity = atoi(buffer + 6);
            for (int i = 0; i < quantity; ++i) {
                Task task;
                Message msg;
                generate_task(&task, previous_result);
                log_msg(task.expression);
                write(client_socket, task.expression, strlen(task.expression));

                clock_gettime(CLOCK_MONOTONIC, &start);
                read(client_socket, buffer, 256);
                clock_gettime(CLOCK_MONOTONIC, &end);

                task.response_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
                task.client_answer = atoi(buffer);
                sem_lock();
                stat->total_time += task.response_time;
                sem_unlock();

                msg.mtype = 1;
                msg.task = task;
                msgsnd(msg_id, &msg, sizeof(msg.task), 0);

                pid_t pid = fork();
                if (pid == 0) {
                    handle_answer(msg_id);

                    pid = fork();
                    if (pid == 0) {
                        
                    }
                    return 0;
                }

                // if (*shared_counter == 0) {
                //     send_stats_to_client(client_socket);
                // }

                previous_result = task.client_answer;
            }
            char time[7];
            sprintf(time, "%f", stat->total_time);
            write(client_socket, time, strlen(time));
        }

        close(client_socket);
        goto finish;
    }
    
    finish:
    close_message_queue(msg_id);
    disable_shared_memory();
    shut_down();

    return 0;
}
