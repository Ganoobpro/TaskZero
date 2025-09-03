////////////////////  MAIN  ////////////////////
#include "./taskzero.h"
#include "color.h"

TaskZero taskzero;

static void LoadFile();
static void SaveFile();

static void PrintError();
static void PrintTasks();
static void Reset();

static void CommandHandle();

int main() {
  LoadFile();

  while (!taskzero.flags.done) {
    Reset();
    PrintTasks();
    if (taskzero.flags.hadError)
      { PrintError(); }
    CommandHandle();
  }

  SaveFile();

  return 0;
}



////////////////////  PRINT  ////////////////////
static inline void AnnounceError(const std::string& error_message) {
  taskzero.error_message = error_message;
  taskzero.flags.hadError = true;
}

static void PrintError() {
  std::cerr << "[ERROR] " << taskzero.error_message
      << " Type 'help' for more information.\n";
  taskzero.flags.hadError = false;
}

static void PrintStatus(const Task* curr_task) {
  std::cout << "  ";
  switch (curr_task->status) {
    case TASK_PENDING:
      std::cout << _ADD_COLOR_TEXT_BG("[Pending]", _WHITE_BG, _BLACK) << '\n';
      break;

    case TASK_IN_PROGRESS:
      std::cout << _ADD_COLOR_TEXT_BG("[In Progress]", _YELLOW_BG, _BLACK) << '\n';
      break;

    case TASK_DONE:
      std::cout << _ADD_COLOR_TEXT_BG("[Done]", _GREEN_BG, _BLACK) << '\n';
      break;

    default:
      AnnounceError("Unknown task status!");
      return;
  }
}

static void PrintDeadline(const Task* curr_task) {
  if (curr_task->deadline.INF) {
    printf("XX-XX-XXXX");
    return;
  }

  printf("%i-%i-%i (%i) | ",
    curr_task->deadline.date, curr_task->deadline.month,
    curr_task->deadline.year, curr_task->deadline._date
  );
}

static void PrintTask(const int& task_id) {
  Task* curr_task = &taskzero.task_list.at(task_id);
  PrintStatus(curr_task);
  printf("  %3i. ", task_id+1);

  std::string task_name = curr_task->task_name;
  COUT_RAINBOW(task_name, task_name.length());
  std::cout << " | ";

  PrintDeadline(curr_task);
  printf("\n");
}

static void PrintTasks() {
  for (size_t task_id = 0; task_id < taskzero.task_list.size(); task_id++) {
    PrintTask(task_id);
    if (taskzero.flags.hadError)
      {return;}
  }
}

static void Reset() {
  // Clear screen
  std::cout << "\033[2J\033[1;1H"; 
}



////////////////////  TASK  ////////////////////
static inline void AddTask(const Task& new_task) {
  taskzero.task_list.push_back(new_task);
}

static inline void ChangeTask(const uint32_t &task_id, const Task& changed_task) {
  taskzero.task_list[task_id] = changed_task;
}

static inline void DeleteTask(const uint32_t &task_id) {
  taskzero.task_list.erase(taskzero.task_list.begin() + task_id);
}

static inline void MarkTask(const uint32_t &task_id, const TaskStatus &status) {
  taskzero.task_list[task_id].status = status;
}



////////////////////  FILE  ////////////////////
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

static void SaveFile() {
  std::ofstream out(WORKLIST_FILE, std::ios::binary);

  for (Task task : taskzero.task_list)
    { out.write((byte*)&task, sizeof(Task)); }

  out.close();
}



////////////////////  COMMANDS  ////////////////////
static void GetCommand() {
  std::cout << "> ";
  std::cin >> taskzero.command;
}

static void RemoveLeftoverInput(bool print_error) {
  if (std::cin.peek() != '\n' && std::cin.peek() != EOF) {
    if (print_error)
      { AnnounceError("Too much input!"); }
    taskzero.flags.hadError = true;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

static void
GetFlags(const InputFlags& input_flags, InputArguments* input_arguments) {
  // Skip white space
  while (std::cin.peek() == ' ')
    { std::cin.ignore(); }

  if (std::cin.peek() != '-')
    { return; }

  std::string flags;
  std::cin >> flags;
  char* c = &flags.at(1);

  do {
    switch (*c++) {
      case 'n':
        if (input_flags.no_name)
          { AnnounceError("Error flag!"); return; }
        input_arguments->get_name = true;
        break;

      case 'd':
        if (input_flags.no_deadline)
          { AnnounceError("Error flag!"); return; }
        input_arguments->get_deadline = true;
        break;

      case 'c':
        if (input_flags.no_consequence)
          { AnnounceError("Error flag!"); return; }
        input_arguments->get_consequence = true;
        break;

      default:
        AnnounceError("Unknown flag!");
        break;
    }
  } while (!(*c == '\n' || *c == '\0'));
}

static uint32_t GetNumber(uint32_t smallest_number,
                          const std::string& error_message) {
  std::string str_num;
  std::cin >> str_num;

  for (char c : str_num) {
    if (c < '0' || c > '9') {
      AnnounceError("Invalid number!");
      return 0;
    }
  }

  uint32_t res = (uint32_t) std::stoi(str_num);
  if (res >= smallest_number)
    { return res; }

  AnnounceError(error_message);
  return 0;
}

static inline uint32_t GetId() {
  return GetNumber(1, "Expect exist id number!") - 1;
}

static bool InvalidName() {
  std::string task_name;
  task_name = std::cin.peek();

  if (task_name.length() > TASK_NAME_LIMIT) {
    AnnounceError("Task name is too long!");
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
  }

  return false;
}

static bool WrongDateFormat(const std::string str_date) {
  // Format: DD-MM-YYYY
  if (str_date.length() > 10)
    { return true; }

  uint8_t split_sign = 2;
  for (size_t i = 0; i < str_date.length(); i++) {
    char c = str_date[i];

    if (c == '-' && split_sign > 0) {
      if (i == str_date.length()-1 || str_date[i+1] == '-')
        { return true; }

      split_sign--;
      continue;
    }

    if (c < '0' || c > '9')
      { return true; }
  }

  return (split_sign > 0);
}

static void ConvertToDate(Date& date, const char* p_str_date) {
  char* p_end{};
  date.date  = (uint32_t) strtol(p_str_date, &p_end, 10);
  p_str_date = p_end + 1;
  date.month = (uint32_t) strtol(p_str_date, &p_end, 10);
  p_str_date = p_end + 1;
  date.year  = (uint32_t) strtol(p_str_date, &p_end, 10);
}

// NP: no print
static bool InvalidDateNP(const Date& date) {
  if (date.year < 2000) {
    AnnounceError("Invalid year! Expect year over 2000.");
    return true;
  }

  if (date.month < 1 || date.month > 12) {
    AnnounceError("Invalid month!");
    return true;
  }

  uint32_t month_days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if ((date.year & 3) == 0)
    { month_days[2] = 29; }
  if (date.date < 1 || date.date > month_days[date.month]) {
    AnnounceError("Invalid date!");
    return true;
  }

  return false;
}

uint32_t month_sum_days[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static void CalculateDate(Date& date) {
  date._date = (date.year - 2000) >> 2;
  if (date.month >= 3 || (date.year & 3) > 0)
    { date._date++; }

  date._date += (date.year - 2000) * 365;
  date._date += month_sum_days[date.month-1];
  date._date += date.date;
}

static Date GetDate() {
  Date date{};
  std::string str_date;
  std::cin >> str_date;

  if (WrongDateFormat(str_date)) {
    AnnounceError("Invalid date format!");
    return date;
  }

  ConvertToDate(date, &str_date[0]);
  if (InvalidDateNP(date))
    { return date; }

  CalculateDate(date);
  return date;
}

static void
GetArguments(const InputArguments& input_arguments, Task* new_task) {
  if (input_arguments.get_name) {
    if (std::cin.peek() == '\n' || std::cin.peek() == EOF) {
      std::cout << "Task name: ";
      if (InvalidName()) { return; }
    }

    std::cin >> new_task->task_name;
  }

  if (input_arguments.get_deadline) {
    if (std::cin.peek() == '\n' || std::cin.peek() == EOF)
      { std::cout << "Deadline: "; }

    new_task->deadline = GetDate();

    if (taskzero.flags.hadError)
      { return; }

  } else
    { new_task->deadline.INF = true; }

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

static void CommandAdd() {
  // Get flags
  InputArguments input_arguments{};
  input_arguments.get_name = true;
  GetFlags(
    { .no_name = true, .no_deadline = false, .no_consequence = false },
    &input_arguments
  );
  if (taskzero.flags.hadError) {
    RemoveLeftoverInput(false);
    return;
  }

  // Get agruments
  Task new_task;
  GetArguments(input_arguments, &new_task);
  if (taskzero.flags.hadError) {
    RemoveLeftoverInput(false);
    return;
  }
  new_task.status = TASK_PENDING;

  // Leftover check & Execute main job
  RemoveLeftoverInput(true);
  if (taskzero.flags.hadError)
    { return; }
  AddTask(new_task);
}

static void CommandUpdate() {
  // Get flags
  InputArguments input_arguments{};
  input_arguments.get_id = true;
  GetFlags(
    { false, false, false },
    &input_arguments
  );
  if (taskzero.flags.hadError) {
    RemoveLeftoverInput(false);
    return;
  }

  // Get agruments
  uint32_t task_id = GetId();
  if (taskzero.flags.hadError || task_id >= taskzero.task_list.size()) {
    RemoveLeftoverInput(false);
    return;
  }
  Task changed_task;
  GetArguments(input_arguments, &changed_task);

  // Leftover check & Execute main job
  RemoveLeftoverInput(true);
  if (taskzero.flags.hadError)
    { return; }
  ChangeTask(task_id, changed_task);
}

static void CommandDelete() {
  // Get flags
  InputArguments input_arguments{};
  input_arguments.get_id = true;
  GetFlags(
    { true, true, true },
    &input_arguments
  );
  if (taskzero.flags.hadError) {
    RemoveLeftoverInput(false);
    return;
  }

  // Get agruments
  uint32_t task_id = GetId();
  if (taskzero.flags.hadError || task_id >= taskzero.task_list.size()) {
    RemoveLeftoverInput(false);
    return;
  }

  // Leftover check & Execute main job
  RemoveLeftoverInput(true);
  if (taskzero.flags.hadError)
    { return; }
  DeleteTask(task_id);
}

static void CommandMark(const TaskStatus status) {
  // Get flags
  InputArguments input_arguments{};
  input_arguments.get_id = true;
  GetFlags(
    { false, false, false },
    &input_arguments
  );
  if (taskzero.flags.hadError) {
    RemoveLeftoverInput(false);
    return;
  }

  // Get agruments
  uint32_t task_id = GetId();
  if (taskzero.flags.hadError || task_id >= taskzero.task_list.size()) {
    RemoveLeftoverInput(false);
    return;
  }

  // Leftover check & Execute main job
  RemoveLeftoverInput(true);
  if (taskzero.flags.hadError)
    { return; }
  MarkTask(task_id, status);
}

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
  else if (taskzero.command == "delete")
    { CommandDelete(); }
  else if (taskzero.command == "mark-in-progress")
    { CommandMark(TASK_IN_PROGRESS); }
  else if (taskzero.command == "mark-done")
    { CommandMark(TASK_DONE); }
  else if (taskzero.command == "quit")
    { taskzero.flags.done = true; }
  else {
    std::cout << "Unknown command: " << taskzero.command << "\n";
  }
}
