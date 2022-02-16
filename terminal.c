#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define TAMANHO_MAXIMO_STRING 1024
#define MAX_ARGS 128
#define COMANDOS_BUILT_IN 4

void iniciarTerminal();
void printUsuario();
void printPastaAtual();
void abrirHelp();
int userInput(char *input);
int processarInput(char *input, char **parsed);
void parseComandos(char *comando, char **parsed);
int builtinHandler(char **parsed);
void execArgs(char **parsedArgs);

//é preciso instalar o pacote libreadline-dev
//comando de compilação gcc terminal.c -o terminal -lreadline

int main(void) {
    //inputString define o comando digitado pelo usuario, e parsedArgs constitui todos so comandos digitados
    char inputString[TAMANHO_MAXIMO_STRING], *parsedArgs[MAX_ARGS];
    int exec;

    char initPath[TAMANHO_MAXIMO_STRING] = "/home/";
    strcat(initPath, getenv("USER"));

    chdir(initPath);
    iniciarTerminal();

    while(1) {
        //verifica se algo foi digitado pelo usuario
        if(!userInput(inputString)) {
            continue;
        }
        
        exec = processarInput(inputString, parsedArgs);

        if (exec == 1) execArgs(parsedArgs);
    }
    return 0;
}

void iniciarTerminal() {
    printUsuario();
    printPastaAtual();
    abrirHelp();
}

int userInput(char *input) {
    char *buf;
    printf("\n%s", getenv("USER"));
    buf = readline("\n$ ");
    if (strlen(buf) > 0) {
        add_history(buf);
        strcpy(input, buf);
        return 1;
    }
    return 0;
}

void parseComandos(char *comando, char **parsed) {
    for (int i = 0; i < MAX_ARGS; i++) {
        parsed[i] = strsep(&comando, " ");
        if (parsed[i] == NULL) break;
        if (strlen(parsed[i]) == 0) i--;
    }
}

int processarInput(char *input, char **parsed) {
    parseComandos(input, parsed);
    if (builtinHandler(parsed)) return 0;
    return 1;
}

int builtinHandler(char **parsed) {
    int i, builtinArg = 0;
    char* builtincomandos[COMANDOS_BUILT_IN];
    char dir[TAMANHO_MAXIMO_STRING];
    builtincomandos[0] = "exit";
    builtincomandos[1] = "cd";
    builtincomandos[2] = "help";
    builtincomandos[3] = "directory";
    //para esses built-in funcionarem, serao comparados somente com o "primeiro" comando digitado(o unico comando)
    for(i = 0; i < COMANDOS_BUILT_IN; i++) {
        if (strcmp(parsed[0], builtincomandos[i]) == 0) {builtinArg = i + 1; break;}
    }
    switch (builtinArg) {
        case 1:
            exit(0);
        case 2:
            chdir(parsed[i]);
            return 1;
        case 3:
            abrirHelp();
            return 1;
        case 4:
            printf("%s\n", getcwd(dir, sizeof(dir)));
            return 1;
        default:
            break;
    }
    return 0;
}

void execArgs(char **parsedArgs) {
    pid_t pid = fork();
    if (pid < 0) perror("Falha ao realizar o fork\n");
    else if (pid == 0) {
        if (execvp(parsedArgs[0], parsedArgs) < 0) perror("Não foi possivel executar o comando\n");
        exit(0);
    }
    wait(NULL);
}

void printUsuario() {
    printf("Seja bem vindo %s\n", getenv("USER"));
}

void printPastaAtual() {
    char currentDirectory[1024];
    printf("Voce está na pasta %s\n", getcwd(currentDirectory, sizeof(currentDirectory)));
}

void abrirHelp() {
    printf("\nExercicio de Terminal da disciplina SO\n");
    printf("Feito por Rafael Aranzate com base no tutorial do site GeeksForGeeks\n");
    printf("\nURL: https://www.geeksforgeeks.org/making-linux-shell-c/\n\n");
    printf("Comandos suportados: Todos aqueles normalmente disponiveis em um terminal Linux (incluindo pipe)\n");
    printf("Também suportados os comandos built-in: cd, ls, exit; E o comando custom: directory.\n");
}
