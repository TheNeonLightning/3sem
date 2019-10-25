#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unordered_map>
#include "Stack_t.hpp"

using std::string;
using std::unordered_map;

const int PROGRAM_MIN_SIZE = 10;
const int MAX_COMMAND_SIZE = 10;

class Processor {
public:
  Stack_t<int> stack;
  int registers[4]{0};
  uint8_t* program = nullptr;
  size_t program_size = PROGRAM_MIN_SIZE;

  const char* program_filename     = "../program.txt";
  const char* listing_filename     = "../listing.txt";
  const char* program_bin_filename = "../program";

  unordered_map<string, int> labels_table;

  struct JumpCmd {

    unsigned int second_equal : 1;
    unsigned int less : 1;
    unsigned int equal : 1;
    unsigned int more : 1;
    unsigned int code : 4;

    JumpCmd(const char* str) {
      assert(str != nullptr);

      code = 6;
      more = 0;
      equal = 0;
      less = 0;
      second_equal = 0;

      switch (str[1]) {

      case ('a'):
        less = 1;
        break;

      case ('b'):
        more = 1;
        break;

      case ('e'):
        equal = 1;
        second_equal = 1;
        break;

      default:
        more = 1;
        less = 1;
      }

      if (str[2] == 'e') {
        second_equal = 1;
      }
    }
  };

  Processor() {
    program = new uint8_t[program_size]{0};
  }

  Processor(const char* program_file, const char* listing_file, const char* program_bin_file): Processor()
  {
    assert(program_file != nullptr);
    assert(listing_file != nullptr);
    assert(program_bin_file != nullptr);

    program_filename = program_file;
    listing_filename = listing_file;
    program_bin_filename = program_bin_file;
  }

  void MakeListing() {
    FILE* listing_file = fopen(listing_filename, "w");
    assert(listing_file != nullptr);
    size_t index = 0;
    fprintf(listing_file, "addr code              source\n");
    while(program[index] != 0) {
      fprintf(listing_file, "%.4zX %.8X ", index, program[index]);
      switch (program[index++]) {
      case (1):
        fprintf(listing_file, "%.8X push %d\n", program[index], program[index++]);
        break;
      case (2):
        fprintf(listing_file, "%.8X pop\n", 0);
        break;
      case (4):
        fprintf(listing_file, "%.8X out\n", 0);
        break;
      case (5):
        fprintf(listing_file, "%.8X add\n", 0);
        break;
      case (6):
        fprintf(listing_file, "%.8X sub\n", 0);
        break;
      case (7):
        fprintf(listing_file, "%.8X mul\n", 0);
        break;
      case (8):
        fprintf(listing_file, "%.8X div\n", 0);
        break;
      case (11):
        fprintf(listing_file, "%.8X push %cx\n", program[index], program[index++] + 'a');
        break;
      case (12):
        fprintf(listing_file, "%.8X pop %cx\n", program[index], program[index++] + 'a');
        break;
      case (13):
        fprintf(listing_file, "%.8X ", program[index]);
        fprintf(listing_file, "in ");
        fprintf(listing_file, "%cx ", program[index++] + 'a');
        fprintf(listing_file, "%d\n", program[index++]);
        break;
      }
    }
    fprintf(listing_file, "%.4zX %.8X %.8X end", index, program[index], 0);
    fclose(listing_file);
  }

  void ChangeProgramSize() {
    uint8_t* copy_buffer = new uint8_t[program_size * 2];
    for (size_t index = 0; index < program_size; ++index) {
      copy_buffer[index] = program[index];
    }
    delete[] program;
    program = copy_buffer;
    program_size *= 2;
  }

  void CheckProgramSize(size_t index) {
    if (index >= program_size - 5) {
      ChangeProgramSize();
    }
  }

  void Preprocessing() {
    assert(program_filename != nullptr);
    FILE* program_file = fopen(program_filename, "r");
    char* str = new char[MAX_COMMAND_SIZE];
    int reg_index = 0;
    size_t index = 0;
    while(fscanf(program_file, "%s", str) != EOF) {
      assert(str != nullptr);
      CheckProgramSize(index);

      if (strcmp(str, "ax") == 0) {
        assert(index > 0);
        program[index - 1] += 10;
        program[index++] = 0;
        continue;
      }

      if (strcmp(str, "bx") == 0) {
        assert(index > 0);
        program[index - 1] += 10;
        program[index++] = 1;
        continue;
      }

      if (strcmp(str, "cx") == 0) {
        assert(index > 0);
        program[index - 1] += 10;
        program[index++] = 2;
        continue;
      }

      if (strcmp(str, "dx") == 0) {
        assert(index > 0);
        program[index - 1] += 10;
        program[index++] = 3;
        continue;
      }

      if (strcmp(str, "push") == 0) {
        program[index++] = 1;
        continue;
      }

      if (strcmp(str, "pop") == 0) {
        program[index++] = 2;
        continue;
      }

      if (strcmp(str, "in") == 0) {
        program[index++] = 3;
        continue;
      }

      if (strcmp(str, "out") == 0) {
        program[index++] = 4;
        continue;
      }

      if (strcmp(str, "add") == 0) {
        program[index++] = 5;
        continue;
      }

      if (strcmp(str, "sub") == 0) {
        program[index++] = 6;
        continue;
      }

      if (strcmp(str, "mul") == 0) {
        program[index++] = 7;
        continue;
      }

      if (strcmp(str, "div") == 0) {
        program[index++] = 8;
        continue;
      }

      if (strcmp(str, "sqrt") == 0) {
        program[index++] = 9;
        continue;
      }

      if (strcmp(str, "pow") == 0) {
        program[index++] = 10;
        continue;
      }

      if (strcmp(str, "end") == 0) {
        program[index++] = 0;
        continue;
      }

      if (str[0] == 'j') {

        JumpCmd command(str);
        program[index++] = *reinterpret_cast<uint8_t*>(&command);

        fscanf(program_file, "%s", str);
        if (labels_table.find(str) != labels_table.end()) {
          program[index] = labels_table[str];
        }

        ++index;
        continue;
      }

      if (isdigit(str[0])) {
        uint8_t* value_start_ptr = &program[index];
        char* end = nullptr;
        int* value_address = reinterpret_cast<int*>(value_start_ptr);
        *value_address = (int)strtol(str, &end, 10);
        index += 4;
        continue;
      }

      string str_for_hash = str;
      str_for_hash.pop_back();
      labels_table.emplace(str_for_hash, index);
    }
    fclose(program_file);
  }

  void SaveProgramInBin() {
    FILE* program_bin = fopen(program_bin_filename, "wb");
    assert(program_bin != nullptr);

    fputc(program_size, program_bin);
    fwrite(program, sizeof(uint8_t), program_size, program_bin);
    fclose(program_bin);
  }

  void GetProgramFromBin() {
    FILE* program_bin = fopen(program_bin_filename, "rb");
    assert(program_bin != nullptr);

    program_size = fgetc(program_bin);
    fread(program, sizeof(uint8_t), program_size, program_bin);
    fclose(program_bin);
  }


  void LabelsProcessing() {
    FILE* program_file = fopen(program_filename, "r");
    char* str = new char[MAX_COMMAND_SIZE];
    size_t index = 0;

    while(fscanf(program_file, "%s", str) != EOF) {
      assert(str != nullptr);

      if (str[0] == 'j') {
        fscanf(program_file, "%s", str);
        assert(str != nullptr);
        program[++index] = labels_table[str];
      }

      if (isdigit(str[0])) {
        index += 4;
        continue;
      }

      ++index;

    }
    fclose(program_file);
  }

  int GetConstValue(size_t index) {
    int* value = reinterpret_cast<int*>(&program[index]);
    return *value;
  }

  int LabelShift(size_t index) {
    assert(index >= 0);
    uint8_t command = program[index];
    if ((command & 0b01100000) == 0b01100000) {
      if ((command & 0b00001010) == 0b00001010) {
        if ((command & 0b00000001) == 0b00000001) {
          if (stack.Pop() != stack.Pop()) {
            return program[index + 1];
          }
        } else {
          return program[index + 1];
        }
      } else if ((command & 0b00000001) == 0b00000001) {
        if ((command & 0b00001000) == 0b00001000) {
          if (stack.Pop() <= stack.Pop()) {
            return program[index + 1];
          }
        }
        if ((command & 0b00000100) == 0b00000100) {
          if (stack.Pop() == stack.Pop()) {
            return program[index + 1];
          }
        }
        if (stack.Pop() >= stack.Pop()) {
          return program[index];
        }
      } else {
        if ((command & 0b00001000) == 0b00001000) {
          if (stack.Pop() < stack.Pop()) {
            return program[index + 1];
          }
        }
        if (stack.Pop() > stack.Pop()) {
          return program[index + 1];
        }
      }
    }
    return ++index;
  }

  void Run() {
    size_t index = 0;
    int value = 0;

    while(program[index] != 0) {
      switch (program[index++]) {

      case (1): ///push const
        stack.Push(GetConstValue(index));
        index += 4;
        break;

      case (2): ///pop
        stack.Pop();
        break;

      case (4): ///out
        value = stack.Pop();
        printf("%d ", value);
        stack.Push(value);
        fflush(stdout);
        break;

      case (5): ///add
        stack.Push(stack.Pop() + stack.Pop());
        break;

      case (6): ///sub
        stack.Push(stack.Pop() - stack.Pop());
        break;

      case (7): ///mul
        stack.Push(stack.Pop() * stack.Pop());
        break;

      case (8): ///div
        stack.Push(stack.Pop() / stack.Pop());
        break;

      case (9): ///sqrt
        stack.Push(sqrt(stack.Pop()));
        break;

      case (10): ///pow
        stack.Push(pow(stack.Pop(), stack.Pop()));
        break;

      case (11): ///push reg
        stack.Push(registers[program[index++]]);
        break;

      case (12): ///pop to reg
        registers[program[index++]] = stack.Pop();
        break;

      case (13):
        registers[program[index++]] = GetConstValue(index);
        index += 4;
        break;

      default: ///jump
        index = LabelShift(index - 1);
      }
    }
  }

  ~Processor() {
    delete[] program;
  }

};

int main() {
  Processor processor;
  processor.Preprocessing();
  processor.LabelsProcessing();
  processor.MakeListing();
  processor.Run();
  return 0;
}