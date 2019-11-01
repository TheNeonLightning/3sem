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

  Processor();

  Processor(const char* program_file, const char* program_bin_file);

  void Assemble();

  void Run();

  void LabelsProcessing();

  void SaveProgramInBin();

  void GetProgramFromBin();

  ~Processor();

private:

  void ChangeProgramSize();

  void CheckProgramSize(size_t index);

  int GetCmdCode(const char* str);

  int GetConstValue(size_t index);

  int LabelShift(size_t index);

};

