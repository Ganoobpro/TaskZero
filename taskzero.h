#ifndef TASKZERO_H
#define TASKZERO_H

#include "./setup.h"
#include <string.h>
#include <vector.h>
#include <iostream>
#include <stdint.h>

#define WORKLIST_FILE "./worklist.bin"

typedef byte char;



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



////////////////////  TASKZERO  ////////////////////
typedef struct {
  std::vector<Task> task_list;
} TaskZero;


#endif
