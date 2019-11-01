#include "processor.h"
#include <gtest/gtest.h>

int main(int argc, char* argv[]) {

  Processor processor("../quadratic_equation.txt", "../quadratic_equation_bin.txt");
  processor.Assemble();
  /*for (size_t index = 0; index < processor.program_size; ++index) {
    printf("%d ", processor.program[index]);
  }
  fflush(stdout);*/
  printf("\n");
  processor.LabelsProcessing();
  /*for (size_t index = 0; index < processor.program_size; ++index) {
    printf("%d ", processor.program[index]);
  }
  fflush(stdout);*/
  processor.Run();
  //testing::InitGoogleTest(&argc, argv);
  //RUN_ALL_TESTS();
  return 0;
}