////////////////////  MAIN  ////////////////////
#include "./taskzero.h"

static void InitTaskList();
static void AddSimpleTask(SimpleTask* newTask);
static void DeleteTask(uint8_t taskIndex);

static void LoadFile();

TaskList tasks;

int main() {
  InitTaskList();
  LoadFile();

  return 0;
}



////////////////////  Task List  ////////////////////
static void InitTaskList() {
  tasks.numberOfTask = 0;
}

static void AddSimpleTask(SimpleTask* newTask) {
  if (tasks.numberOfTask >= TASK_LIMIT) {
    fprintf(stderr, "You have too much tasks. Please stand up and do your fucking jobs.\n");
    exit(1);
  }

  tasks.tasks[tasks.numberOfTask] = *newTask;
  tasks.numberOfTask++;
}

static void DeleteTask(uint8_t taskIndex) {
  for (taskIndex; taskIndex < tasks.numberOfTask; taskIndex++) {
    tasks.tasks[taskIndex] = tasks.tasks[taskIndex+1];
  }
  tasks.numberOfTask--;
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
