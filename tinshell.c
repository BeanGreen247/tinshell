#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int tinshell_cd(char **args);
int tinshell_help(char **args);
int tinshell_exit(char **args);

char *builtinshell_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtinshell_func[]) (char **) = {
  &tinshell_cd,
  &tinshell_help,
  &tinshell_exit
};

int tinshell_num_builtins() {
  return sizeof(builtinshell_str) / sizeof(char *);
}

int tinshell_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "tinshell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("tinshell");
    }
  }
  return 1;
}

int tinshell_help(char **args)
{
  int i;
  printf("Thomas Mozdren's tinshell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < tinshell_num_builtins(); i++) {
    printf("  %s\n", builtinshell_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int tinshell_exit(char **args)
{
  return 0;
}

int tinshell_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("tinshell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("tinshell");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int tinshell_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < tinshell_num_builtins(); i++) {
    if (strcmp(args[0], builtinshell_str[i]) == 0) {
      return (*builtinshell_func[i])(args);
    }
  }

  return tinshell_launch(args);
}

#define tinshell_RL_BUFSIZE 1024

char *tinshell_read_line(void)
{
  int bufsize = tinshell_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "tinshell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += tinshell_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "tinshell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define tinshell_TOK_BUFSIZE 64
#define tinshell_TOK_DELIM " \t\r\n\a"

char **tinshell_split_line(char *line)
{
  int bufsize = tinshell_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "tinshell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, tinshell_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += tinshell_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "tinshell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, tinshell_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void tinshell_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("tinshell> ");
    line = tinshell_read_line();
    args = tinshell_split_line(line);
    status = tinshell_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  tinshell_loop();
  return EXIT_SUCCESS;
}
