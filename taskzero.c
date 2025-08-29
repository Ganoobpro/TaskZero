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
}



////////////////////  TASK LIST  ////////////////////
static void InitTaskList() {
  taskList.numberOfTask = 0;
}

static void AddSimpleTask(SimpleTask* newTask) {
  if (taskList.numberOfTask >= TASK_LIMIT) {
    fprintf(stderr, "You have too much tasks. Please stand up and do your fucking jobs.\n");
    exit(1);
  }

  taskList.tasks[tasks.numberOfTask] = *newTask;
  taskList.numberOfTask++;
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
    AddSimpleTask(&newTask);
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
}

static Token ScanToken() {
  scanner.start = command_buffer;
  scanner.curr = command_buffer;

  while (*scanner.curr) {
    if (*scanner.curr == ' ' || *scanner.curr == '\n') {
      if (scanner.curr != scanner.start) {
        Token token = (Token) {
          scanner.start,
          scanner.curr - scanner.start
        };

        scanner.start = ++scanner.curr;
        return token;
      }
    }

    scanner.curr++;
  }

  return (Token) {scanner.start, 0u};
}

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

static bool CommandHandle() {
  TaskZeroCommand cmd = GetCommand();
  switch (cmd) {
    case COMMAND_ADD:
      printf("Adding...\n");
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
      printf("Unknown or error command!\n");
      break;
  }

  return true;
}
