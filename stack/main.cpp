#include <stdio.h>
#include <string>
#include <cassert>

using std::string;

string VARIABLE_NAME = "NULL";

#define SAVE_VARIABLE_NAME(variable) VARIABLE_NAME = string(#variable);

#define VERIFIED (Dump(Verification(), __FILE__, __LINE__, __PRETTY_FUNCTION__) || (assert(false), false));

template <typename Elem_t, int first_value = 5741, int second_value = 8192>
class Stack_t {
public:

  enum ErrorCode {
    no_error = -1,
    no_error_run_of_dump = 0,
    this_pointer_nullptr = 1,
    beginning_point_wrong_value = 10,
    ending_point_wrong_value = 100,
    hash_wrong_value = 1000,
    buffer_poison_value_warning = 10000
  };


  Stack_t() {
    poison_value = PoisonValue();
    buffer = new Elem_t[1];
    buffer[0] = poison_value;
    ++capacity;
    hash = Hash();
  }


  explicit Stack_t(Elem_t poison_value): poison_value(poison_value) {
    buffer = new Elem_t[1];
    buffer[0] = poison_value;
    ++capacity;
    hash = Hash();
  }


  void Push(const Elem_t& element) {
    VERIFIED
    if (size == capacity) {
      ChangeCapacity();
    }
    buffer[size++] = element;
    hash = Hash();
    VERIFIED
  }


  Elem_t Pop() {
    VERIFIED
    Elem_t value = buffer[--size];
    buffer[size] = poison_value;
    hash = Hash();
    VERIFIED
    return value;
  }


  int Hash() {
    char* string_for_hash = reinterpret_cast<char*>(buffer);
    return HashFromString(string_for_hash, sizeof(Elem_t) * capacity + sizeof(this));
  }


  bool HashCheck() {
    int current_hash = Hash();
    return current_hash == hash;
  }


  bool BufferCheck() {
    for (int index = size; index < capacity; ++index) {
      if (buffer[index] != poison_value) {
        return false;
      }
    }
    return true;
  }


  bool Dump(ErrorCode code, const string& file, const int& line, const string&  function, const char* log_file = "../log_file") {
    if (code == no_error) {
      return true;
    }
    bool flag = false;
    printf("Verification failed from %s (%d) %s\n", file.c_str(), line, function.c_str());
    printf("Stack_t \"%s\"[%p] {\n", VARIABLE_NAME.c_str(), this);
    string error_name;
    if (code == no_error_run_of_dump) {
      error_name = "No error";
      flag = true;
    }
    if (code == this_pointer_nullptr) {
      error_name = "Pointer to the object is nullptr";
    }
    if (code == beginning_point_wrong_value) {
      error_name = "Beginning point wrong value";
    }
    if (code == ending_point_wrong_value) {
      error_name = "Ending point wrong value";
    }
    if (code == hash_wrong_value) {
      error_name = "Hash wrong value";
    }
    if (code == buffer_poison_value_warning) {
      error_name = "No error | Buffer poison value warning";
      flag = true;
    }
    printf("  ErrorCode = %d (%s)\n", code, error_name.c_str());
    printf("  size = %d\n", size);
    printf("  buffer[%d] = [%p]\n  {\n", capacity, buffer);
    for (int index = 0; index < size; ++index) {
      printf("   *[%d] = %d\n", index, buffer[index]);
    }
    for (int index = size; index < capacity; ++index) {
      printf("    [%d] = %d", index, buffer[index]);
      if (buffer[index] == poison_value) {
        printf(" (POISON?)\n");
      }
    }
    printf("  }\n}\n");
    return flag;
  }


  ErrorCode Verification(const char* log_file = "../log_file") {

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
    return no_error;

  }


private:

  int beginning_point = 0;
  int size = 0;
  int capacity = 0;
  Elem_t* buffer = nullptr;
  Elem_t poison_value = PoisonValue();
  int hash = 0;
  int ending_point = 0;


  Elem_t PoisonValue() {
    std::srand(time(0));
    char* value = new char[sizeof(Elem_t)];
    for (unsigned long long index = 0; index < sizeof(Elem_t); ++index) {
      value[index] = rand();
    }
    Elem_t result = *value;
    delete[] value;
    return result;
  }


  void ChangeCapacity() {
    capacity *= 2;
    Elem_t* new_buffer = new Elem_t[capacity];
    for (int index = 0; index < size; ++index) {
      new_buffer[index] = buffer[index];
    }
    delete[] buffer;
    buffer = new_buffer;
    for (int index = size; index < capacity; ++index) {
      buffer[index] = poison_value;
    }
  }


  int HashFromString(const char* string_for_hash, int size_of_string) {
    int local_hash = 0;
    for (int index = 0; index < size_of_string; ++index) {
      local_hash = (local_hash * first_value + string_for_hash[index]) % second_value;
    }
    return local_hash;
  }


public:

  ~Stack_t() {
    delete[] buffer;
  }

};

////////////////////////////////////////////////////////////////////////////////

int main() {
  Stack_t<int> stack;
  for (int i = 0; i < 9; ++i) {
    stack.Push(i);
  }
  stack.Dump(Stack_t<int>::ErrorCode::no_error_run_of_dump, __FILE__, __LINE__, __PRETTY_FUNCTION__);
  for (int i = 0; i < 5; ++i) {
    stack.Pop();
  }
  stack.Dump(Stack_t<int>::ErrorCode::no_error_run_of_dump, __FILE__, __LINE__, __PRETTY_FUNCTION__);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

//////////Stack_t///////////////////////////////////////////////////////////////
