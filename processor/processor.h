/**
 * \file
 * \author Eremenko Ilya
 * \version three
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unordered_map>

#include "Stack_t.hpp"
#include  "cmd_code.hpp"
#include "const_params.hpp"


using std::string;
using std::unordered_map;

/**
 * \brief This class contains all processor properties such as stack, registers
 * array, labels table.
 */
class Processor {
public:
  Stack_t<int> stack;
  int registers[4]{0};
  uint8_t* program = nullptr;
  size_t program_size = PROGRAM_MIN_SIZE;
  unordered_map<string, int> labels_table;

  const char* program_filename     = "../program.txt";
  const char* program_bin_filename = "../program";

  struct Cmd {
    string name;
    uint8_t code;

    Cmd(const char* name, uint8_t code):
        name(name),
        code(code)
    {}
  };

  /**
   * Initializing program array with the size PROGRAM_MIN_SIZE.
   */
  Processor();

  /**
   * Initializing program array with the size PROGRAM_MIN_SIZE and getting the
   * file names.
   * @param program_file A name of file containing the program to run.
   * @param program_bin_file A name of the file which will be used to store the
   * bin code of the program.
   */
  Processor(const char* program_file, const char* program_bin_file);

  /**
   * Assembling program from program file into byte code and storing it in the
   * program array.
   */
  void Assemble();

  /**
   * Executing the stored in the program array program.
   */
  void Run();

  /**
   * Resolution of labels using the labels stored in the labels table.
   */
  void LabelsProcessing();

  /**
   * Saving the stored in the array program in the program_bin_file.
   */
  void SaveProgramInBin();

  /**
   * Downloading the stored in the program_bin_file program and saving it in the
   * program array
   */
  void GetProgramFromBin();

  ~Processor();

private:
  /**
   * \brief Resizing program array.
   *
   * Multiplying the size of array by two.
   */
  void ChangeProgramSize();

  /**
   * \brief Checking the size of program array, resizing it in case if there not
   * enough space.
   *
   * Resizing is done then there are equal or less than 5 bytes left in the
   * array, so there always would be a space for storing int value.
   */
  void CheckProgramSize(size_t index);

  /**
   * \brief Return the code of command stored in str.
   *
   * Returns the code of the command if it (the command) needs only one byte in
   * program array (so there are no int values following this command or etc.).
   * Otherwise the -1 will be returned as result.
   * @param str
   * @return
   */
  int GetCmdCode(const char* str);

  /**
   * \brief Gets the int value from the program array.
   *
   * Returns the int value stored in the program array in 4 following each other
   * bytes starting from index.
   */
  int GetConstValue(size_t index);

  /**
   * \brief Moving index in the program array from the current position to the
   * position on which the label is pointing.
   */
  int LabelShift(size_t index);

};

