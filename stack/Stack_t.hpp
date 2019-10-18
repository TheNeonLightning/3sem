/**
 * \file
 * \brief This file contains template class Stack_t.
 * \author Eremenko Ilya
 *
 * The class Stack_t contained in this file is an realization of data structure
 * stack with some additional defensive mechanisms.
 */
#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

#define VAR_NAME(name)  #name ///Is used to get the Stack_t variable name.

#define VERIFIED (Dump(Verification(), __FILE__, __LINE__, __PRETTY_FUNCTION__) || (assert(false), false));

#define DUMP(error_code) (Dump(error_code, __FILE__, __LINE__, __PRETTY_FUNCTION__));

#define FORCE_FAIL(error_code) (Dump(error_code, __FILE__, __LINE__, __PRETTY_FUNCTION__) || (assert(false), false));

/**
 * /brief The class Stack_t realization with defensive mechanisms and dumper.
 *
 * In case of using this class with custom
 * @tparam external_dumper The pointer to the custom dumper.
 * @tparam value Is used in hash calculation via Horner's method.
 * @tparam power Is used in hash calculation via Horner's method.
 */
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*) = nullptr, int value = 741, int power = 7>
class Stack_t {
public:


  enum ErrorCode {

    no_error = -1,

    no_error_run_of_dump = 0,

    this_pointer_nullptr = 1,

    pop_from_empty_stack = 2,

    beginning_point_wrong_value = 4,

    ending_point_wrong_value = 8,

    hash_wrong_value = 16,

    buffer_poison_value_warning = 32,

    buffer_sentinel_wrong_value = 64

  };

  Stack_t(const Stack_t& stack)            = delete;

  Stack_t& operator=(const Stack_t& stack) = delete;

  /**
   * @param log Can be specified and later used for output.
   * @param name The name of a variable that will be used in (default) dump.
   */
  explicit Stack_t(FILE* log = nullptr, const char* name = nullptr);
  /**
   * @param poison Can be used to set a specific poison value. (Otherwise it
   * will be generated.)
   * @param log Can be specified and later used for output.
   * @param name The name of a variable that will be used in (default) dump.
   */
  explicit Stack_t(Elem_t poison, FILE* log = nullptr, const char* name = nullptr);

  void Push(const Elem_t& element);
  /**
   * \brief Returns the top element in the stack and removes it from it. In case
   * if the stack was empty the behavior depends on the code parameter.
   *
   *    If code is nullptr (default), the error will occur and the dumper will be
   * called before finishing the process.
   *    If code is not nullptr, the dumper
   * will be called, but it won't be followed by finishing process. The error
   * code will be placed in code and the stack won't change. The returned value
   * will be poison value.
   */
  Elem_t Pop(ErrorCode* code = nullptr);
  /**
   * \brief Calculating and saving the hash from stack via Horner's method
   * with the usage
   * of value and power template parameters.
   *
   * The division is implemented as a bitwise shift by a power number (as if
   * it was divided by the power of two.
   */

  bool Dump(int code, const char* file, int line, const char* function);

  ~Stack_t();

protected:

  void Hash();

  bool HashCheck();
  /**
   * \brief Checking that the unused part of buffer is filled with poison value.
   */
  bool BufferCheck();
  /**
   * \brief Checking that the buffer's sentinels (in the beginning and in the
   * ending) are equal to poison value.
   */
  bool SentinelCheck();
  /**
   * @param code Error code.
   * @param file The name of the file n which the error occured
   * @param line The number of a line in which the error occured.
   * @param function The name of the function in which the error occured.
   */
  bool DefaultDump(int code, const char* file, int line, const char* function);
  /**
   * \brief A set of checks of the stack and its content.
   */
  ErrorCode Verification();

  /// The following functions are used to output the base types in DefaultDumper.
  void ElementsOutput(unsigned long long sample);

  void ElementsOutput(long long sample);

  void ElementsOutput(char* sample);

  void ElementsOutput(void* sample);

  void ElementsOutput(char sample);

  void ElementsOutput(std::string sample);

  void ElementsOutput(int sample);

  int beginning_point = 0;
  int size = 0;
  int capacity = 0;

  const char* name = nullptr;

  FILE* log_file = nullptr;
  bool open_log = false;

  Elem_t* buffer = nullptr;
  Elem_t poison_value = PoisonValue();

  int hash = 0;
  int ending_point = 0;

  /**
   * \brief Generating poison value.
   */
  Elem_t PoisonValue();
  /**
   * \brief Increasing the capacity of the buffer by multiplying it by two.
   */
  void ChangeCapacity();
};


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

Stack_t<Elem_t, external_dumper, value, power>::Stack_t(FILE* log, const char* name): name(name) {

  poison_value = PoisonValue();
  buffer = new Elem_t[3];

  if (log == nullptr) {
    log_file = fopen("../log_file", "a");
    open_log = true;
  } else {
    log_file = log;
  }

  assert(log_file != nullptr);

  buffer[0] = poison_value;
  buffer[1] = poison_value;
  buffer[2] = poison_value;

  ++capacity;
  Hash();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

Stack_t<Elem_t, external_dumper, value, power>::Stack_t(Elem_t poison, FILE* log, const char* name): Stack_t(log, name) {
  assert(ferror(log) == 0);
  assert(feof(log) == 0);

  poison_value = poison;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::Push(const Elem_t& element) {

  VERIFIED

  if (size == capacity) {
    ChangeCapacity();
  }
  buffer[1 + (size++)] = element;
  Hash();

  VERIFIED

}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

Elem_t Stack_t<Elem_t, external_dumper, value, power>::Pop(ErrorCode* code) {

  VERIFIED

  if (size == 0) {
    if (code != nullptr) {
      *code = pop_from_empty_stack;
      DUMP(*code)
    } else {
      FORCE_FAIL(*code)
    }
    return poison_value;
  }


  Elem_t element = buffer[(--size) + 1];
  buffer[size + 1] = poison_value;
  Hash();

  VERIFIED

  return element;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::Hash() {
  assert(buffer != nullptr);

  const char* buffer_str = reinterpret_cast<char*>(buffer);
  const char* stack_str  = reinterpret_cast<char*>(this);
  hash = 0;

  assert(buffer_str != nullptr);
  assert(stack_str  != nullptr);

  for (int index = 0; index < capacity * 2 + 1; ++index) {
    hash = (hash * value + buffer_str[index]) << power;
  }

  for (int index = 0; index < sizeof(*this); ++index) {
    hash = (hash * value + stack_str[index]) << power;
  }
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

bool Stack_t<Elem_t, external_dumper, value, power>::HashCheck() {
  int old_hash = hash;
  Hash();
  return hash == old_hash;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

bool Stack_t<Elem_t, external_dumper, value, power>::BufferCheck() {
  for (int index = size + 1; index < capacity + 1; ++index) {
    if (buffer[index] != poison_value) {
      return false;
    }
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

bool Stack_t<Elem_t, external_dumper, value, power>::SentinelCheck() {
  if ((buffer[0] != poison_value) || (buffer[capacity + 1] != poison_value)) {
    return false;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

bool Stack_t<Elem_t, external_dumper, value, power>::
Dump(int code, const char* file, const int line, const char* function) {

  assert(log_file != nullptr);
  assert(ferror(log_file) == 0);
  assert(feof(log_file) == 0);
  assert(file != nullptr);
  assert(line > 0);
  assert(function != nullptr);

  if (code == no_error) {
    return true;
  }

  if (external_dumper == nullptr) {
    return DefaultDump(code, file, line, function);
  } else {
    return external_dumper(code, file, line, function, log_file);
  }
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

typename Stack_t<Elem_t, external_dumper, value, power>::
    ErrorCode
Stack_t<Elem_t, external_dumper, value, power>::Verification() {

  if (this == nullptr) {
    return this_pointer_nullptr;
  }

  if (beginning_point != 0) {
    return beginning_point_wrong_value;
  }

  if (ending_point != 0) {
    return ending_point_wrong_value;
  }

  if (!HashCheck()) {
    return hash_wrong_value;
  }

  if (!BufferCheck()) {
    return buffer_poison_value_warning;
  }

  if (!SentinelCheck()) {
    return buffer_sentinel_wrong_value;
  }

  return no_error;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

Stack_t<Elem_t, external_dumper, value, power>::~Stack_t() {
  if (open_log) {
    fclose(log_file);
  }
  delete[] buffer;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

Elem_t Stack_t<Elem_t, external_dumper, value, power>::PoisonValue() {

  assert(this != nullptr);

  std::srand(time(0));
  char* result_str = new char[sizeof(Elem_t)];
  for (unsigned long long index = 0; index < sizeof(Elem_t); ++index) {
    result_str[index] = rand();
  }
  Elem_t result = *result_str;
  delete[] result_str;
  return result;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ChangeCapacity() {

  VERIFIED

  capacity *= 2;
  Elem_t* new_buffer = new Elem_t[capacity  + 2];
  new_buffer[0] = poison_value;

  for (int index = 1; index <= size; ++index) {
    new_buffer[index] = buffer[index];
  }

  new_buffer[capacity + 1] = poison_value;
  delete[] buffer;

  buffer = new_buffer;
  assert(buffer != nullptr);

  for (int index = size + 1; index < capacity + 1; ++index) {
    buffer[index] = poison_value;
  }
  Hash();

  VERIFIED

}
////////////////////////////////////////////////////////////////////////////////

template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

bool Stack_t<Elem_t, external_dumper, value, power>::
DefaultDump(int code, const char* file, const int line, const char* function) {

  assert(log_file != nullptr);
  assert(ferror(log_file) == 0);
  assert(feof(log_file) == 0);
  assert(file != nullptr);
  assert(line > 0);
  assert(function != nullptr);

  bool flag = false;
  fprintf(log_file, "Verification failed from %s (%d) %s\n", file, line, function);
  fflush(log_file);
  if (name == nullptr) {
    name = "name not stated";
  }
  fprintf(log_file, "Stack_t \"%s\"[%p] {\n", name, this);
  fflush(log_file);
  const char* error_name = nullptr;

  if (code == this_pointer_nullptr) {
    error_name = "Pointer to the object is nullptr";
  }


  else if (code == no_error_run_of_dump) {
    error_name = "No error";
    flag = true;
  }

  else if (code == beginning_point_wrong_value) {
    error_name = "Beginning point wrong value";
  }

  else if (code == ending_point_wrong_value) {
    error_name = "Ending point wrong value";
  }

  else if (code == hash_wrong_value) {
    error_name = "Hash wrong value";
  }

  else if (code == buffer_poison_value_warning) {
    error_name = "No error | Buffer poison value warning";
    flag = true;
  }

  else if (code == buffer_sentinel_wrong_value) {
    error_name = "Buffer sentinel wrong value";
  }

  else {
    error_name = "Failed to determine the error name";
  }

  fprintf(log_file, "  ErrorCode = %d (%s)\n", code, error_name);
  fflush(log_file);

  if (code == this_pointer_nullptr) {
    fprintf(log_file, "Unable to represent the data: pointer to the object is nullptr.");
    return flag;
  }


  fprintf(log_file, "  size = %d\n", size);
  fflush(log_file);
  fprintf(log_file, "  buffer[%d] = [%p]\n  {\n", capacity, buffer);
  fflush(log_file);

  ElementsOutput(poison_value);

  fprintf(log_file, "  }\n}\n");
  fflush(log_file);

  if ((!flag) && (open_log)) {
    fclose(log_file);
  }
  return flag;
}
////////////////////////////////////////////////////////////////////////////////

///////////////////////////ElementsOutputCollection/////////////////////////////

///unsigned long long///////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(unsigned long long sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %llu\n", index, buffer[index]);
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %llu", index, buffer[index]);
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

///long long////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(long long sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %lld\n", index, buffer[index]);
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %lld", index, buffer[index]);
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

///char* ///////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(char* sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %s\n", index, buffer[index]);
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %s", index, buffer[index]);
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

///void* ///////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(void* sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %s\n", index, buffer[index]);
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %s", index, buffer[index]);
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

///char/////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(char sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %c\n", index, buffer[index]);
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %c", index, buffer[index]);
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

///std::string//////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(std::string sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %s\n", index, buffer[index].c_str());
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %s", index, buffer[index].c_str());
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

///int//////////////////////////////////////////////////////////////////////////
template <typename Elem_t, bool (*external_dumper)
    (int, const char*, const int, const char*, FILE*), int value, int power>

void Stack_t<Elem_t, external_dumper, value, power>::ElementsOutput(int sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(log_file, "   *[%d] = %i\n", index, buffer[index]);
    fflush(log_file);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(log_file, "    [%d] = %i", index, buffer[index]);
    fflush(log_file);
    if (buffer[index] == poison_value) {
      fprintf(log_file, " (POISON?)\n");
      fflush(log_file);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////



