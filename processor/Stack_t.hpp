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

FILE* LOG_FILE = nullptr;

/**
 * /brief The class Stack_t realization with defensive mechanisms and dumper.
 *
 * In case of using this class with custom
 * @tparam external_dumper The pointer to the custom dumper.
 * @tparam value Is used in hash calculation via Horner's method.
 * @tparam power Is used in hash calculation via Horner's method.
 */
template <typename Elem_t>
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
  explicit Stack_t(const char* name = nullptr);
  /**
   * @param poison Can be used to set a specific poison value. (Otherwise it
   * will be generated.)
   * @param log Can be specified and later used for output.
   * @param name The name of a variable that will be used in (default) dump.
   */
  explicit Stack_t(Elem_t poison, const char* name = nullptr, int value = 747, int power = 7);

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

  bool Dump(int code, const char* file, int line, const char* function);

  ~Stack_t();

protected:
  /**
   * \brief Calculating and saving the hash from stack via Horner's method
   * with the usage
   * of value and power template parameters.
   *
   * The division is implemented as a bitwise shift by a power number (as if
   * it was divided by the power of two.
   */
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
  int value = 0;
  int power = 0;

  const char* name = nullptr;

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
template <typename Elem_t>

Stack_t<Elem_t>::Stack_t(const char* name): name(name) {

  poison_value = PoisonValue();
  buffer = new Elem_t[3];

  buffer[0] = poison_value;
  buffer[1] = poison_value;
  buffer[2] = poison_value;

  ++capacity;
  Hash();
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

Stack_t<Elem_t>::Stack_t(Elem_t poison, const char* name, int val, int pow):
Stack_t(name) {
  poison_value = poison;
  value = val;
  power = pow;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::Push(const Elem_t& element) {

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
template <typename Elem_t>

Elem_t Stack_t<Elem_t>::Pop(ErrorCode* code) {

  VERIFIED

  if (size == 0) {
    if (code != nullptr) {
      *code = pop_from_empty_stack;
      DUMP(*code)
    } else {
      FORCE_FAIL(*code)
    }

    VERIFIED

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
template <typename Elem_t>

void Stack_t<Elem_t>::Hash() {
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
template <typename Elem_t>

bool Stack_t<Elem_t>::HashCheck() {
  int old_hash = hash;
  Hash();
  return hash == old_hash;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

bool Stack_t<Elem_t>::BufferCheck() {
  for (int index = size + 1; index < capacity + 1; ++index) {
    if (buffer[index] != poison_value) {
      return false;
    }
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

bool Stack_t<Elem_t>::SentinelCheck() {
  if ((buffer[0] != poison_value) || (buffer[capacity + 1] != poison_value)) {
    return false;
  }
  return true;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

bool Stack_t<Elem_t>::
Dump(int code, const char* file, const int line, const char* function) {

  //assert(LOG_FILE != nullptr);
  //assert(ferror(LOG_FILE) == 0);
  //assert(feof(LOG_FILE) == 0);
  //assert(file != nullptr);
  //assert(line > 0);
  //assert(function != nullptr);

  if (code == no_error) {
    return true;
  }

  return DefaultDump(code, file, line, function);
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

typename Stack_t<Elem_t>::ErrorCode Stack_t<Elem_t>::Verification() {

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
template <typename Elem_t>

Stack_t<Elem_t>::~Stack_t() {
  delete[] buffer;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

Elem_t Stack_t<Elem_t>::PoisonValue() {

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
template <typename Elem_t>

void Stack_t<Elem_t>::ChangeCapacity() {

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

template <typename Elem_t>

bool Stack_t<Elem_t>::
DefaultDump(int code, const char* file, const int line, const char* function) {

  //assert(LOG_FILE != nullptr);
  assert(ferror(LOG_FILE) == 0);
  assert(feof(LOG_FILE) == 0);
  assert(file != nullptr);
  assert(line > 0);
  assert(function != nullptr);

  bool flag = false;
  fprintf(LOG_FILE, "Verification failed from %s (%d) %s\n", file, line, function);
  fflush(LOG_FILE);
  if (name == nullptr) {
    name = "name not stated";
  }
  fprintf(LOG_FILE, "Stack_t \"%s\"[%p] {\n", name, this);
  fflush(LOG_FILE);
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

  fprintf(LOG_FILE, "  ErrorCode = %d (%s)\n", code, error_name);
  fflush(LOG_FILE);

  if (code == this_pointer_nullptr) {
    fprintf(LOG_FILE, "Unable to represent the data: pointer to the object is nullptr.");
    return flag;
  }


  fprintf(LOG_FILE, "  size = %d\n", size);
  fflush(LOG_FILE);
  fprintf(LOG_FILE, "  buffer[%d] = [%p]\n  {\n", capacity, buffer);
  fflush(LOG_FILE);

  ElementsOutput(poison_value);

  fprintf(LOG_FILE, "  }\n}\n");
  fflush(LOG_FILE);

  return flag;
}
////////////////////////////////////////////////////////////////////////////////

///////////////////////////ElementsOutputCollection/////////////////////////////

///unsigned long long///////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(unsigned long long sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %llu\n", index, buffer[index]);
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %llu", index, buffer[index]);
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

///long long////////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(long long sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %lld\n", index, buffer[index]);
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %lld", index, buffer[index]);
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

///char* ///////////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(char* sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %s\n", index, buffer[index]);
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %s", index, buffer[index]);
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

///void* ///////////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(void* sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %s\n", index, buffer[index]);
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %s", index, buffer[index]);
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

///char/////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(char sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %c\n", index, buffer[index]);
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %c", index, buffer[index]);
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

///std::string//////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(std::string sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %s\n", index, buffer[index].c_str());
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %s", index, buffer[index].c_str());
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

///int//////////////////////////////////////////////////////////////////////////
template <typename Elem_t>

void Stack_t<Elem_t>::ElementsOutput(int sample) {
  for (int index = 1; index < size + 1; ++index) {
    fprintf(LOG_FILE, "   *[%d] = %i\n", index, buffer[index]);
    fflush(LOG_FILE);
  }

  for (int index = size + 1; index < capacity + 1; ++index) {
    fprintf(LOG_FILE, "    [%d] = %i", index, buffer[index]);
    fflush(LOG_FILE);
    if (buffer[index] == poison_value) {
      fprintf(LOG_FILE, " (POISON?)\n");
      fflush(LOG_FILE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////



