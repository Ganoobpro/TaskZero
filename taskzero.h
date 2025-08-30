#ifndef TASKZERO_H
#define TASKZERO_H

#define EXTRA_CHAR             2  // I use fget so there are \n & \0
#define TASKNAME_CHAR_LIMIT    50
#define TASK_LIMIT             100
#define BUFFER_CHAR_LIMIT      200
#define START_YEAR             2000
#define WORKLIST_FILE          "worklist.bin"

#include "./setup.h"
#include <string.h>



////////////////////  FLAGS  ////////////////////
typedef enum {
  DETAIL_NONE,
  DETAIL_HELP
} ExtraDetails;

typedef struct {
  ExtraDetails extra_details;
  bool done;
  bool hadError;
} Flags;



////////////////////  TASK  ////////////////////
typedef enum {
  PENDING,
  IN_PROGRESS,
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
  char name[ TASKNAME_CHAR_LIMIT + EXTRA_CHAR ];
  TaskStatus status;
  Priority priority;
} TaskInfo;

typedef struct {
  TaskInfo taskInfo;
} SimpleTask;

typedef struct {
  SimpleTask tasks[TASK_LIMIT];
  uint8_t numberOfTask;
} TaskList;



////////////////////  COMMAND HANDLE  ////////////////////
typedef enum {
  TOKEN_COMMAND,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_ERROR,
  TOKEN_EOF
} TokenType;

typedef enum {
  COMMAND_ADD,
  COMMAND_UPDATE,
  COMMAND_DELETE,

  COMMAND_MARK_IN_PROGRESS,
  COMMAND_MARK_DONE,

  COMMAND_HELP,
  COMMAND_QUIT,
  COMMAND_ERROR
} TaskZeroCommand;

typedef struct {
  const char* start;
  const char* curr;
} Scanner;

typedef struct {
  const char* start;
  TokenType type;
  uint8_t length;
} Token;

#endif
