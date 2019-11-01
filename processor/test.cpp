#include "processor.h"
#include <gtest/gtest.h>

TEST(SimpleTasks, Factorial) {
  Processor processor("../factorial.txt", "../factorial_bin.txt");
  processor.Assemble();
  processor.LabelsProcessing();
  processor.Run();

}