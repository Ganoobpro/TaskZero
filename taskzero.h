#ifndef TASKZERO_H
#define TASKZERO_H

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <limits>

#define WORKLIST_FILE "./worklist.bin"

typedef char byte;



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
  std::string task_name;
  TaskPriority priority;
  TaskStatus status;
} Task;



////////////////////  COMMAND HANDLE  ////////////////////
typedef struct {
  bool no_id;
  bool no_name;
  bool no_deadline;
  bool no_consequence;
} InputFlags;

typedef struct {
  bool get_id;
  bool get_name;
  bool get_deadline;
  bool get_consequence;
} InputArguments;



////////////////////  TASKZERO  ////////////////////
typedef struct {
  bool hadError;
  bool done;
} CommandFlags;

typedef struct {
  std::string command{};
  std::string error_message{};
  std::vector<Task> task_list{};
  CommandFlags flags{};
} TaskZero;

#endif
