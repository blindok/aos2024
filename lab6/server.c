#include "server.h"

#define LOG_FILE_NAME_SIZE  16
#define LOG_FILE_SIZE       64

typedef struct {
    int port;
    char log_file[LOG_FILE_NAME_SIZE];
} ServerConfig;

int log_fd = 0;
ServerConfig config;
const char* log_file = NULL;
sigset_t sigset_hup;

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
    dprintf(log_fd, "[%ld] %s\n", time(NULL), msg);
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

void generate_task(Task* const task, int* const previous_result) {
    int operand1 = (previous_result == NULL) ? generate_random_number() : *previous_result;
    int operand2 = generate_random_number();
    char operator = operators[generate_random_number() % (sizeof(operators) / sizeof(char))];

    snprintf(task->expression, EXPRESSION_SIZE, "%d %c %d", operand1, operator, operand2);

    switch (operator) {
        case '+': task->correct_answer = operand1 + operand2; break;
        case '-': task->correct_answer = operand1 - operand2; break;
        case '*': task->correct_answer = operand1 * operand2; break;
        case '/': task->correct_answer = operand1 / operand2; break;
        case '%': task->correct_answer = operand1 % operand2; break;
        case '^': task->correct_answer = (int)pow(operand1, operand2); break;
    }
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

    setup_sighup_handler();

    log_fd = open(config.log_file, O_CREAT | O_WRONLY | O_APPEND, 0666);

    log_msg("HI!");

    return 0;
}
