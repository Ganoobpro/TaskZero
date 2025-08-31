////////////////////  MAIN  ////////////////////
#include "./taskzero.h"

TaskZero taskzero;

int main() {
  return 0;
}



////////////////////  MAIN LOOP  ////////////////////
static void PrintError(const std::string& error_message) {
  cerr << "[ERROR] " << taskzero.error_message
       << "Type 'help' for more information.\n";
  taskzero.flags.hadError = true;
}

static void InitFlags() {
  taskzero.flags.hadError     = false;
  taskzero.flags.done         = false;
}

static void InitProgram() {
  InitFlags();
}



////////////////////  LOAD FILE  ////////////////////
static void LoadFile() {
  ifstream file(WORKLIST_FILE, ios::binary);

  if (!file.good()) {
    ofstream file("myfile.txt", ios::out);
    file.close();
    return;
  }

  Task new_task;
  while (file.read((byte*)&new_task, sizeof(Task))) {
    taskzero.push_back(new_task);
  }
}



////////////////////  COMMANDS  ////////////////////
static void GetCommand() {
  std::cout << "> ";
  std::cin >> taskzero.command;
}

static void RemoveLeftoverInput() {
  if (cin.peek() != '\n' && cin.peek() != EOF) {
    PrintError("Too much input!");
    until (cin.peek() == '\n' || cin.peek() == EOF);
  }
}

static void GetFlags(InputFlags* input_flags) {
  char* c = &input_flags[0];

  if (*c == '-') {
    c++;

    do {
      switch (*c++) {
        case 'i':
          if (input_flags->no_id)
            { PrintError("Error flag!"); return; }
          input_flags->no_id = false;
          break;

        case 'n':
          if (input_flags->no_name)
            { PrintError("Error flag!"); return; }
          input_flags->no_name = false;
          break;

        case 'd':
          if (input_flags->no_deadline)
            { PrintError("Error flag!"); return; }
          input_flags->no_deadline = false;
          break;

        case 'c':
          if (input_flags->no_consequence)
            { PrintError("Error flag!"); return; }
          input_flags->no_consequence = false;
          break;

        default:
          PrintError("Unknown flag!");
          break;
      }
    } until (*c == '\n' || *c == '\0');
  }
}

static void CommandHelp() {
  RemoveLeftoverInput();
  if (taskzero.hadError)
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
}

static void CommandAdd() {
  InputFlags input_flags = (InputFlags) {
    .no_id = false,
    .no_name = false,
    .no_deadline = true,
    .no_consequence = true
  };
  GetFlags(&input_flags);
  if (taskzero.hadError)
    { return; }

  Task new_task;
  cin >> new_task.task_name;

  // TODO: Add deadline, consequence later

  RemoveLeftoverInput();
  if (taskzero.hadError)
    { return; }
}

static void CommandHandle() {
  if (taskzero.command == "") {}
  else if (taskzero.command == "help") { CommandHelp(); }
  else if (taskzero.command == "add") {
    std::cout << "Add a new task.\n";
  }
  else if (taskzero.command == "update") {
    std::cout << "Update an existing task.\n";
  }
  else if (taskzero.command == "delete") {
    std::cout << "Delete a task.\n";
  }
  else if (taskzero.command == "mark-in-progress") {
    std::cout << "Mark task as in progress.\n";
  }
  else if (taskzero.command == "mark-done") {
    std::cout << "Mark task as done.\n";
  }
  else {
    std::cout << "Unknown command: " << command << "\n";
  }

  RemoveLeftoverInput();
}
