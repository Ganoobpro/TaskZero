////////////////////  MAIN  ////////////////////
#include "./taskzero.h"

TaskZero taskzero;

static void PrintTasks();
static void CommandHandle();
static void Reset();

int main() {
  while (!taskzero.flags.done) {
    CommandHandle();
    PrintTasks();
    Reset();
  }

  return 0;
}



////////////////////  MAIN LOOP  ////////////////////
static void PrintError(const std::string& error_message) {
  std::cerr << "[ERROR] " << error_message
       << " Type 'help' for more information.\n";
  taskzero.flags.hadError = true;
}

std::string status_string[] = {
  [TASK_PENDING]     = "[Pending]",
  [TASK_IN_PROGRESS] = "[In Progress]",
  [TASK_DONE]        = "[Done]"
};

static void PrintTask(const int& task_id) {
#define CHAR_LIMIT_TASK_NAME 15

  Task* curr_task = &taskzero.task_list.at(task_id);
  printf("  %3i. ", task_id);
  std::cout << curr_task->task_name << " "
            << status_string[curr_task->status] << "\n";

#undef CHAR_LIMIT_TASK_NAME
}

static void PrintTasks() {
  for (size_t task_id = 0; task_id < taskzero.task_list.size(); task_id++) {
    PrintTask(task_id);
  }
}

static void Reset() {
  taskzero.flags.hadError = false;
}



////////////////////  LOAD FILE  ////////////////////
static void LoadFile() {
  std::ifstream in(WORKLIST_FILE, std::ios::binary);

  if (!in) {
    std::ofstream out(WORKLIST_FILE, std::ios::binary);
    out.close();
    return;
  }

  Task new_task;
  while (in.read((byte*)&new_task, sizeof(Task))) {
    taskzero.task_list.push_back(new_task);
  }

  in.close();
}



////////////////////  COMMANDS  ////////////////////
static void GetCommand() {
  std::cout << "> ";
  std::cin >> taskzero.command;
}

static void RemoveLeftoverInput(bool print_error) {
  if (std::cin.peek() != '\n' && std::cin.peek() != EOF) {
    if (print_error)
      { PrintError("Too much input!"); }
    taskzero.flags.hadError = true;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

static void
GetFlags(const InputFlags& input_flags, InputArguments* input_arguments) {
  std::string flags;
  flags = std::cin.peek();
  if (flags[0] != '-')
    { return; }

  std::cin >> flags;
  char* c = &flags.at(1);

  do {
    switch (*c++) {
      case 'i':
        if (input_flags.no_id)
          { PrintError("Error flag!"); return; }
        input_arguments->get_id = false;
        break;

      case 'n':
        if (input_flags.no_name)
          { PrintError("Error flag!"); return; }
        input_arguments->get_name = false;
        break;

      case 'd':
        if (input_flags.no_deadline)
          { PrintError("Error flag!"); return; }
        input_arguments->get_deadline = false;
        break;

      case 'c':
        if (input_flags.no_consequence)
          { PrintError("Error flag!"); return; }
        input_arguments->get_consequence = false;
        break;

      default:
        PrintError("Unknown flag!");
        break;
    }
  } while (!(*c == '\n' || *c == '\0'));
}

static unsigned int GetNumber() {
  std::string str_num;
  std::cin >> str_num;

  for (char c : str_num) {
    if (c < '0' || c > '9') {
      PrintError("Invalid number!");
      return 0;
    }
  }

  int res = std::stoi(str_num);
  if (res < 0) {
    PrintError("Expect positive number!");
    return 0;
  }
  return res;
}

static void
GetArguments(const InputArguments& input_arguments, Task* new_task) {
  if (input_arguments.get_name)
    { std::cin >> new_task->task_name; }
  if (input_arguments.get_deadline)
    { /* TODO: Will add after finish date calculator */ }
  if (input_arguments.get_consequence)
    { /* TODO: Will add after finish priority calculator */ }
}

static void CommandHelp() {
  RemoveLeftoverInput(false);
  if (taskzero.flags.hadError)
    { return; }

  std::cout << "TaskZero - A Simple To-Do List Manager\n";
  std::cout << "Usage:\n";
  std::cout << "  <command> [options] [task info]\n\n";

  std::cout << "Available commands:\n";
  std::cout << "  help              Show general help or command-specific help\n";
  std::cout << "  add               Add a new task\n";
  std::cout << "  update            Update an existing task\n";
  std::cout << "  delete            Delete a task\n";
  std::cout << "  mark-in-progress  Mark a task as in progress\n";
  std::cout << "  mark-done         Mark a task as done\n\n";

  std::cout << "Flags (options):\n";
  std::cout << "  -i   Exclude task ID     (required for update, delete, mark-in-progress, mark-done)\n";
  std::cout << "  -n   Exclude task name   (required for add, optional for update)\n";
  std::cout << "  -d   Exclude deadline    (optional, format: YYYY-MM-DD)\n";
  std::cout << "  -c   Exclude consequence (optional, a number in range 0 - 10)\n";
  std::cout << "  -h   Show detailed help for the current command\n\n";

  std::cout << "Examples:\n";
  std::cout << "  add -c \"Write report\" 2025-09-10\n";
  std::cout << "  update -dc 3 \"Finish draft\"\n";
  std::cout << "  mark-done 5\n\n";

  RemoveLeftoverInput(true);
  if (taskzero.flags.hadError)
    { return; }
}

#define ERROR_CHECK(other_cond)                  \
  if (taskzero.flags.hadError || (other_cond)) { \
    RemoveLeftoverInput(false);                  \
    return;                                      \
  }
#define LEFTOVER_CHECK         \
  RemoveLeftoverInput(true);   \
  if (taskzero.flags.hadError) \
    { return; }

static void CommandAdd() {
  // Get flags
  InputArguments input_arguments = (InputArguments){true, true, true, true};
  input_arguments.get_id = true;
  GetFlags(
    { .no_id = false, .no_name = false, .no_deadline = true, .no_consequence = true },
    &input_arguments
  );
  ERROR_CHECK(false);

  // Get agruments
  Task new_task;
  GetArguments(input_arguments, &new_task);
  ERROR_CHECK(false);
  new_task.status = TASK_PENDING;

  // Leftover check & Execute main job
  LEFTOVER_CHECK;
  taskzero.task_list.push_back(new_task);
}

static void CommandUpdate() {
  // Get flags
  InputArguments input_arguments = (InputArguments){true, true, true, true};
  GetFlags(
    { .no_id = false, .no_name = true, .no_deadline = true, .no_consequence = true },
    &input_arguments
  );
  ERROR_CHECK(false);

  // Get agruments
  unsigned int task_id = GetNumber();
  ERROR_CHECK(task_id >= taskzero.task_list.size());
  Task new_task;
  GetArguments(input_arguments, &new_task);

  // Leftover check & Execute main job
  LEFTOVER_CHECK;
  taskzero.task_list[task_id] = new_task;
}

#undef ERROR_CHECK
#undef LEFTOVER_CHECK

static void CommandHandle() {
  GetCommand();

  if (taskzero.command == "")
    {}
  else if (taskzero.command == "help")
    { CommandHelp(); }
  else if (taskzero.command == "add")
    { CommandAdd(); }
  else if (taskzero.command == "update")
    { CommandUpdate(); }
  else if (taskzero.command == "delete") {
    std::cout << "Delete a task.\n";
  }
  else if (taskzero.command == "mark-in-progress") {
    std::cout << "Mark task as in progress.\n";
  }
  else if (taskzero.command == "mark-done") {
    std::cout << "Mark task as done.\n";
  }
  else if (taskzero.command == "quit")
    { taskzero.flags.done = true; }
  else {
    std::cout << "Unknown command: " << taskzero.command << "\n";
  }
}
