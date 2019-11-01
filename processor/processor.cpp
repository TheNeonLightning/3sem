#include "processor.h"
#include "commands_array.hpp"

///public///////////////////////////////////////////////////////////////////////

Processor::Processor() {
  program = new uint8_t[program_size]{0};
}


Processor::Processor(const char* program_file, const char* program_bin_file): Processor()
{
  assert(program_file != nullptr);
  assert(program_bin_file != nullptr);

  program_filename = program_file;
  program_bin_filename = program_bin_file;
}


void Processor::Assemble() {
  assert(program_filename != nullptr);
  FILE* program_file = fopen(program_filename, "r");
  char* str = new char[MAX_COMMAND_SIZE];
  bool is_command = false;
  int reg_index = 0, cmd = 0;
  size_t index = 0;

  #define COMMAND(NAME, CMD_CODE, PARSING) \
    case CMD_CODE: \
      PARSING \
      break;

  while (fscanf(program_file, "%s", str) != EOF) {
    assert(str != nullptr);
    CheckProgramSize(index);

    cmd = GetCmdCode(str);

    if (cmd != -1) {
      if (cmd < COMPLEX_PARSING_CMD) {
        program[index++] = cmd;
        continue;
      }

      switch (cmd) {
        #include "commands.cpp"
      }
      continue;
    }

    if (isdigit(str[0]) || str[0] == '-') {
      uint8_t *value_start_ptr = &program[index];
      char *end = nullptr;
      int *value_address = reinterpret_cast<int *>(value_start_ptr);
      *value_address = (int)strtol(str, &end, 10);
      index += 4;
      continue;
    }

    string str_for_hash = str;
    if (str_for_hash.back() == ':') {
      str_for_hash.pop_back();
      labels_table.emplace(str_for_hash, index);
    }
  }
  #undef COMMAND

  fclose(program_file);
}


void Processor::Run() {
  size_t index = 0;
  int value = 0;

  #define INSTRUCTION(NAME, CODE, ACTION) \
    case(CODE): \
      ACTION \
      break;

  while(program[index] != 0) {
    switch (program[index++]) {

      #include "instructions.cpp"

    default:
      index = LabelShift(index - 1);
    }
  }
  #undef INSTRUCTION

}


void Processor::LabelsProcessing() {
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

    string label_str = str;
    if (label_str.back() == ':') {
      continue;
    }

    if (isdigit(str[0]) || (str[0] == '-')) {
      index += 4;
      continue;
    }

    ++index;

  }
  fclose(program_file);
}


void Processor::SaveProgramInBin() {
  FILE* program_bin = fopen(program_bin_filename, "wb");
  assert(program_bin != nullptr);

  fputc(program_size, program_bin);
  fwrite(program, sizeof(uint8_t), program_size, program_bin);
  fclose(program_bin);
}


void Processor::GetProgramFromBin() {
  FILE* program_bin = fopen(program_bin_filename, "rb");
  assert(program_bin != nullptr);

  program_size = fgetc(program_bin);
  fread(program, sizeof(uint8_t), program_size, program_bin);
  fclose(program_bin);
}


Processor::~Processor() {
  delete[] program;
}

///private//////////////////////////////////////////////////////////////////////

void Processor::ChangeProgramSize() {
  uint8_t* copy_buffer = new uint8_t[program_size * 2];
  for (size_t index = 0; index < program_size; ++index) {
    copy_buffer[index] = program[index];
  }
  delete[] program;
  program = copy_buffer;
  program_size *= 2;
}


void Processor::CheckProgramSize(size_t index) {
  if (index >= program_size - 5) {
    ChangeProgramSize();
  }
}


int Processor::GetCmdCode(const char* str) {
  for (size_t index = 0; index < COMMANDS_QUANTITY; ++index) {
    if (commands[index].name == str) {
      return commands[index].code;
    }
  }
  return -1;
}


int Processor::GetConstValue(size_t index) {
  int* value = reinterpret_cast<int*>(&program[index]);
  return *value;
}


int Processor::LabelShift(size_t index) {
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
        if (stack.Pop() >= stack.Pop()) {
          return program[index + 1];
        }
      }

      if ((command & 0b00000100) == 0b00000100) {
        if (stack.Pop() == stack.Pop()) {
          return program[index + 1];
        }
      }

      if ((command & 0b00000010) == 0b00000010) {
        if (stack.Pop() <= stack.Pop()) {
          return program[index];
        }
      }

    } else {

      if ((command & 0b00001000) == 0b00001000) {
        if (stack.Pop() > stack.Pop()) {
          return program[index + 1];
        }
      }
      if ((command & 0b00000010) == 0b00000010) {
        if (stack.Pop() < stack.Pop()) {
          return program[index + 1];
        }
      }

    }
  }
  return index += 2;
}
