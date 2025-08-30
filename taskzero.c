////////////////////  MAIN  ////////////////////
#include "./taskzero.h"

static void InitFlags();

static void InitTaskList();
static void AddSimpleTask(SimpleTask* newTask);
static void DeleteTask(uint8_t taskIndex);
static void PrintPriorityTasks(Priority priority);

static void LoadFile();

Flags flags;
TaskList taskList;
char command_buffer[BUFFER_CHAR_LIMIT];
Scanner scanner;

int main() {
  InitFlags();
  InitTaskList();
  LoadFile();

  until (flags.done) {
    InitFlags();

    PrintPriorityTasks(MUST_DO_NOW);
    PrintPriorityTasks(SHOULD_DO);
    PrintPriorityTasks(CAN_DELAY);
    PrintPriorityTasks(YOUR_STUPID_STUFF);
    printf("\n");
  }

  return 0;
}



////////////////////  FLAGS  ////////////////////
static void InitFlags() {
  flags.done = false;
  flags.hadError = false;
}



////////////////////  TASK LIST  ////////////////////
static void InitTaskList() {
  taskList.numberOfTask = 0;
}

static SimpleTask MakeSimpleTask(Token* task_name, Priority priority) {
  SimpleTask task;
  memcpy(task.taskInfo.name, task_name->start, task_name->length);
  task.taskInfo.name[task_name->length] = '\0';
  task.taskInfo.id = tasks.numberOfTask;
  task.taskInfo.status = PENDING;
  task.taskInfo.priority = priority;
  return task;
}

static void AddSimpleTask(Token* task_name, Priority priority) {
  if (taskList.numberOfTask >= TASK_LIMIT) {
    fprintf(stderr, "You have too much tasks. Please stand up and do your fucking jobs.\n");
    exit(1);
  }

  taskList.tasks[tasks.numberOfTask++] = MakeSimpleTask(task_name, priority);
}

static void DeleteTask(uint8_t taskIndex) {
  for (taskIndex; taskIndex < taskList.numberOfTask; taskIndex++) {
    taskList.tasks[taskIndex] = taskList.tasks[taskIndex+1];
  }
  taskList.numberOfTask--;
}

char* statusString[] {
  [PENDING]     = "[Pending  X] ",
  [IN_PROGRESS] = "[Progress -] ",
  [DONE]        = "[Done     O] "
};

char* priorityString[] = {
  [YOUR_STUPID_STUFF] = "NONURGENT & NOT IMPORTANT",
  [CAN_DELAY]         = "NONURGENT & IMPORTANT",
  [SHOULD_DO]         = "URGENT    & NOT IMPORTANT",
  [MUST_DO_NO]        = "URGENT    & IMPORTANT"
};

static void PrintTask(SimpleTask* task) {
  printf(
    "%s %-3u | %50s\n",
    statusString[task->status],
    task->id,
    task->name
  );
}

static void PrintPriorityTasks(Priority priority) {
  printf("%s:\n", priorityString[priority]);

  for (uint8_t i=0; i < taskList.numberOfTask; i++) {
    if (taskList.tasks[i].taskInfo.priority == priority) {
      PrintTask(&taskList.tasks[i].taskInfo);
    }
  }
  printf("--------------------\n");
}



////////////////////  READ FILE  ////////////////////
static void LoadFile() {
  printf("Loading worklist file ...\n");

  FILE* file = fopen(WORKLIST_FILE, "rb");
  if (null file) {
    printf("Not exist worklist file.\n");
    printf("Creating new worklist file ...\n");
    FILE* file = fopen(WORKLIST_FILE, "wb");

    if (null file) {
      fprintf(stderr, "ERROR: Unable to create new work file.\n");
      exit(1);
    }

    fclose(file);
    return;
  }

  SimpleTask newTask;
  while (fread(&newTask, sizeof(SimpleTask), 1, file) == 1) {
    taskList.tasks[tasks.numberOfTask] = *newTask;
    taskList.numberOfTask++;
  }
  fclose(file);

  printf("Finished loading.\n");
}



////////////////////  GET COMMAND  ////////////////////
static void InputCommand() {
getcommand:;
  do {
    printf("> ");
  } while (null fgets(command_buffer, sizeof(command_buffer), stdin));

  if (empty strchr(command_buffer, '\n')) {
    // Discard leftover char
    int c;
    until ((c = getchar()) == '\n' || c == EOF);

    fprintf(stderr, "[ERROR] TaskZero can only handle %i chars in one line.\n", BUFFER_CHAR_LIMIT);
    goto getcommand;
  }

  scanner.curr = command_buffer;
}

#define advanced (*scanner.curr++)
#define peek     (*scanner.curr)

#define isNumber between('0', scanner.curr[-1], '9')
#define isAlpha  between('a', scanner.curr[-1], 'z')

static inline Token MakeToken(TokenType type) {
  return (Token){
    .start  = scanner.start,
    .length = scanner.curr - scanner.start,
    .type   = type
  };
}

static void SkipWhiteSpace() {
  until (null peek || peek == ' ' || peek == '\n') {
    advanced; 
  }
  scanner.start = scanner.start;
}

static Token ScanToken() {
  SkipWhiteSpace();

  switch (advanced) {
    case '\0':
      return MakeToken(TOKEN_EOF);

    case '"':
      until (advanced == '"');
      return MakeToken(TOKEN_STRING);

    default:
      if (isNumber) {
        while (isNumber) {advanced;}
        if (peek == '\0' || peek == '\n' || peek == ' ') {
          return MakeToken(TOKEN_NUMBER);
        }
      } elif (isAlpha) {
        while (isAlpha) {advanced;}
        if (peek == '\0' || peek == '\n' || peek == ' ') {
          return MakeToken(TOKEN_COMMAND);
        }
      } else {
        return MakeToken(TOKEN_ERROR);
      }
  }
}

#undef advanced
#undef peek

#undef isNumber
#undef isAlpha 

static bool CompareStr(
  const char* start, const char* rest,
  const uint8_t startIndex, const uint8_t startLength
) {
  // Safe (maybe?)
  if (strlen(rest) != length - startIndex) {
    return false;
  }

  return memcmp(start+startIndex, rest, length) == 0;
}

static TaskZeroCommand GetCommand() {
  Token cmd = ScanToken();
  if (cmd.type == TOKEN_EOF) {
    return COMMAND_HELP;
  }

  if (cmd.type != TOKEN_COMMAND) {
    return COMMAND_ERROR;
  }

  switch (*cmd.start) {
    case 'a':
      if (CompareStr(cmd.start, "dd", 2, cmd.length)) { return COMMAND_ADD; }
      break;

    case 'd':
      if (
        CompareStr(cmd.start, "el", 2, cmd.length) ||
        CompareStr(cmd.start, "elete", 5, cmd.length)
      ) { return COMMAND_DELETE; }
      break;

    case 'h':
      if (CompareStr(cmd.start, "elp", 3, cmd.length)) { return COMMAND_HELP; }
      break;

    case 'm':
      if (
        CompareStr(cmd.start, "ark-in-progress", 15, cmd.length) ||
        CompareStr(cmd.start, "ark-progress", 12, cmd.length)
      ) {
        return COMMAND_MARK_IN_PROGRESS;
      } elif (CompareStr(cmd.start, "ark-done", 8, cmd.length)) {
        return COMMAND_MARK_DONE;
      }
      break;

    case 'q':
      if (CompareStr(cmd.start, "uit", 3, cmd.length)) { return COMMAND_QUIT; }
      break;

    case 'u':
      if (CompareStr(cmd.start, "pdate", 5, cmd.length)) { return COMMAND_ADD; }
      break;

    default:
      return COMMAND_ERROR;
  };

  return COMMAND_ERROR;
}

static void CommandError(const char* error_message) {
  fprintf(stderr, "%s\n", error_message);
  flags.hadError = true;
}

#define Consume(token, type) \
\
  do { \
    if (token.type != type) { \
      CommandError("Invalid! Type 'help' for more information."); \
      return; \
    } \
  } while(0)

static void CommandAdd() {
  Token task_name = ScanToken();
  Consume(task_name, TOKEN_STRING);

  Token priority = ScanToken();
  Consume(priority, TOKEN_NUMBER);
  if (priority.length == 1) {
    CommandError("Expect priority number 0 to 3!");
    return;
  }

  AddSimpleTask(task_name, (*priority.start)-'0');
}

#undef Consume

static void CommandHandle() {
  TaskZeroCommand cmd = GetCommand();
  switch (cmd) {
    case COMMAND_ADD:
      printf("Adding...\n");
      CommandAdd();
      break;

    case COMMAND_UPDATE:
      printf("Updating...\n");
      break;

    case COMMAND_DELETE:
      printf("Deleting...\n");
      break;

    case COMMAND_MARK_IN_PROGRESS:
      printf("Marking task as in progress...\n");
      break;

    case COMMAND_MARK_DONE:
      printf("Marking task as done...\n");
      break;

    case COMMAND_HELP:
      printf("Showing help...\n");
      flags.extra_details = DETAIL_HELP;
      break;

    case COMMAND_QUIT:
      printf("Quitting program...\n");
      flags.done = true;
      break;

    case COMMAND_ERROR:
    default:
      CommandError("Unknown or error command!");
      break;
  }
}
