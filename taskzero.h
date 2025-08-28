#ifndef TASKZERO_H
#define TASKZERO_H

#define TASKNAME_CHAR_LIMIT    50
#define TASK_LIMIT             100
#define WORKLIST_FILE          "worklist.bin"

#include "./setup.h"

typedef enum {
  PENDING,
  // IN_PROGRESS,
  DONE
} TaskStatus;

// Four Quadrants rule
typedef enum {
  YOUR_STUPID_STUFF,   // NONURGENT & NOT IMPORTANT = Cleaning (but you want to keep it in your list)
  CAN_DELAY,           // NONURGENT & IMPORTANT     = Homework (but you can do later)
  SHOULD_DO,           // URGENT    & NOT IMPORTANT = Things you want to do (Like dye your hair)
  MUST_DO_NOW          // URGENT    & IMPORTANT     = Important things
} Priority;

// TODO: Create my own data structure
typedef struct {
  char taskName[TASKNAME_CHAR_LIMIT];
  uint32_t number;
  uint32_t createdDate;
  uint32_t deadline;
  TaskStatus status;
  Priority priority;
} TaskInfo;

typedef struct {
  TaskInfo task;
} SimpleTask;

typedef struct {
  SimpleTask tasks[TASK_LIMIT];
  uint8_t numberOfTask;
} TaskList;

#endif
