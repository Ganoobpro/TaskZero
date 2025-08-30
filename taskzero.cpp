////////////////////  MAIN  ////////////////////
#include "./taskzero.h"

TaskZero taskzero;

int main() {
  return 0;
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
static void CommandHandle() {
  std::string command;

  std::cout << "> ";
  std::cin >> command;

  if (command == "add") {
    cout << "Adding task...\n";
  } else if (command == "update") {
    cout << "Updating task...\n";
  } else if (command == "delete") {
    cout << "Deleting task...\n";
  } else if (command == "mark-in-progress") {
    cout << "Marking task as in progress...\n";
  } else if (command == "mark-done") {
    cout << "Marking task as done...\n";
  } else {
    cout << "Unknown command.\n";
  }
}
