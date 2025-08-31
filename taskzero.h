#ifndef TASKZERO_H
#define TASKZERO_H

#include "./setup.h"
#include <string.h>
#include <vector.h>
#include <iostream>
#include <stdint.h>

#define WORKLIST_FILE "./worklist.bin"

typedef byte char;



////////////////////  TASKZERO  ////////////////////
typedef struct {
  bool hadError;
  bool done;
} CommandFlags;

typedef struct {
  std::string command;
  std::vector<Task> task_list;
  CommandFlags flags;
} TaskZero;



////////////////////  TASK  ////////////////////
typedef enum {
  TASK_PENDING,
  TASK_IN_PROGRESS,
  TASK_DONE
} TaskStatus;

typedef enum {
  NONURGENT_NOT_IMPORTANT,
  NONURGENT_IMPORTANT,
  URGENT_NOT_IMPORTANT,
  URGENT_IMPORTANT
} TaskPriority;

typedef struct {
  uint32_t id;
  std::string task_name;
  TaskStatus status;
  TaskPriority priority;
} Task;



////////////////////  COMMAND HANDLE  ////////////////////
typedef struct {
  bool no_id;
  bool no_name;
  bool no_deadline;
  bool no_consequence;
} InputFlags;

#endif
