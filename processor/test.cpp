#include "processor.h"
#include <gtest/gtest.h>

TEST(NoThrow, Assemble) {
  Processor processor("../factorial.txt",
      "../factorial_bin.txt");
  ASSERT_NO_THROW(processor.Assemble());
}

TEST(NoThrow, LabelsProcessing) {
  Processor processor("../factorial.txt",
      "../factorial_bin.txt");
  processor.Assemble();
  ASSERT_NO_THROW(processor.LabelsProcessing());
}

TEST(NoThrow, StoringInBin) {
  Processor processor("../factorial.txt",
      "../factorial_bin.txt");
  processor.Assemble();
  processor.LabelsProcessing();
  ASSERT_NO_THROW(processor.SaveProgramInBin());
  ASSERT_NO_THROW(processor.SaveProgramInBin());
}

TEST(StoreInBin, StoreInBin) {
  Processor processor("../program_for_testing.txt",
      "../factorial_bin.txt");
  processor.Assemble();
  processor.LabelsProcessing();
  processor.SaveProgramInBin();
  processor.GetProgramFromBin();
  processor.Run();
  ASSERT_EQ(processor.stack.Pop(), 30);
}

TEST(AssemblyCheck, Code) {
  int program[24]{1, 10, 0, 0, 0, 1, 20, 0, 0, 0, 106, 12, 5, 0, 7, 13, 1, 10,
                  13, 2, 20, 11, 2, 0};
  Processor processor("../program_for_testing.txt",
      "../factorial_bin.txt");
  processor.Assemble();
  processor.LabelsProcessing();
  for (size_t index = 0; index < 16; ++index) {
    ASSERT_EQ(processor.program[index], program[index]);
  }
}

TEST(SimpleTasks, Factorial) {
  Processor processor("../factorial.txt",
      "../factorial_bin.txt");
  processor.Assemble();
  processor.LabelsProcessing();
  ASSERT_NO_THROW(processor.Run());
  printf("\n");
}

TEST(SimpleTasks, QuadraticEquation) {
  Processor processor("../quadratic_equation.txt",
      "../quadratic_equation_bin.txt");
  processor.Assemble();
  processor.LabelsProcessing();
  ASSERT_NO_THROW(processor.Run());
  printf("\n");
}