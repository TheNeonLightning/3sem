#include "Stack_t.hpp"

int main() {
  FILE* file = fopen("../log_file", "w");
  Stack_t<int> stack(file, VAR_NAME(stack));
  for (int i = 0; i < 11; ++i) {
    stack.Push(-2);
  }
  stack.Dump(static_cast<char>(Stack_t<char>::ErrorCode::no_error_run_of_dump), __FILE__, __LINE__, __PRETTY_FUNCTION__);
  for (int i = 0; i < 5; ++i) {
    stack.Pop();
  }
  stack.Dump(static_cast<char>(Stack_t<char>::ErrorCode::no_error_run_of_dump), __FILE__, __LINE__, __PRETTY_FUNCTION__);
  fclose(file);
  return 0;
}
