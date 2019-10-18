#include "Stack_t.hpp"
#include "gtest/gtest.h"

TEST(PushPop, Stack_t) {
  ASSERT_NO_THROW(Stack_t<int> stack);
  Stack_t<int> stack;
  ASSERT_NO_THROW(stack.Push(1));
  ASSERT_EQ(stack.Pop(), 1);
  Stack_t<int>::ErrorCode code;
  ASSERT_NO_THROW(stack.Pop(&code));
  int poison = stack.Pop(&code);
  ASSERT_EQ(code, Stack_t<int>::pop_from_empty_stack);
}

TEST(FileManagment, Stack_t) {
  FILE* file = fopen("../log_file", "a");
  Stack_t<int> stack(file, VAR_NAME(stack));
  for (int i = 0; i < 11; ++i) {
    stack.Push(-2);
    ASSERT_EQ(ferror(file), 0);
    ASSERT_EQ(feof(file), 0);
  }
  ASSERT_NE(file, nullptr);
  ASSERT_NO_THROW(stack.Dump(static_cast<char>(Stack_t<char>::ErrorCode::no_error_run_of_dump),
             __FILE__, __LINE__, __PRETTY_FUNCTION__));
  for (int i = 0; i < 5; ++i) {
    stack.Pop();
    ASSERT_EQ(ferror(file), 0);
    ASSERT_EQ(feof(file), 0);
  }
  ASSERT_NO_THROW(stack.Dump(static_cast<char>(Stack_t<char>::ErrorCode::no_error_run_of_dump),
             __FILE__, __LINE__, __PRETTY_FUNCTION__));
  ASSERT_NE(file, nullptr);
  fclose(file);
}
