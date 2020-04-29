#include "main.h"
#include "main_thread.h"

int main() {
  init_user_info_list();
  main_thread();
  return 0;
}