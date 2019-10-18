#include "shush-stack.hpp"

///[CANARY]  [HASH]    [CUR_SIZE] [BUFFER_SIZE] [B - U - F - F - E - R] [CANARY]
///uint64_t  uint64_t  size_t     size_t                                uint64_t

int main() {
  try {
    shush::stack::SafeStack<int> stack;
    stack.Push(1);

    char* start    = *reinterpret_cast<char**>(&stack);
    int* start_int = *reinterpret_cast<int**>(&stack);

    uint64_t* canary = *reinterpret_cast<uint64_t**>(&stack);
    printf("first canary: %lx\n", *canary);

    uint64_t* hash = canary + 1;
    printf("hash value:   %lu\n", *hash);

    int* first_element =
        start_int + (2 * sizeof(uint64_t) + 2 * sizeof(size_t)) / sizeof(int);
    printf("element: %d\n", *first_element);

    start = start + (2 * sizeof(uint64_t) +sizeof(size_t));
    size_t* capacity = reinterpret_cast<size_t*>(start);
    printf("capacity: %lu\n", *capacity);

    start = *reinterpret_cast<char**>(&stack);

    unsigned long size_of_stack = (3 * sizeof(uint64_t) + 2 * sizeof(size_t) + sizeof(int) * *capacity);
    start = start + size_of_stack - 8;
    uint64_t* last_canary = reinterpret_cast<uint64_t*>(start);
    printf("size of all stack: %lu\n", size_of_stack);
    printf("last canary:  %lx\n", *last_canary);

    start = *reinterpret_cast<char**>(&stack);

    *first_element = 0;
    printf("element: %d\n", *first_element);

    const uint64_t fake_hash = std::hash<size_t>()(reinterpret_cast<size_t>(&stack))+
                               std::hash<std::string_view>() (
                                   std::string_view(start, sizeof(uint64_t))
                               ) +
                               std::hash<std::string_view>() (
                                   std::string_view(start + sizeof(uint64_t) + sizeof(uint64_t),
                                                    size_of_stack - sizeof(uint64_t) - sizeof(uint64_t) + 1));

    printf("fake hash: %lu\n", fake_hash);

    *hash = fake_hash;

    stack.Push(1);

    stack.Push(1);
    stack.Pop();

    ////////////////////////////////////////////////////////////////////////////

    delete canary;
    printf("first canary: %lx\n", *canary);
    stack.Push(1);
    stack.Push(20);
    stack.Pop();


  } catch(shush::dump::Dump& dump) {
    shush::dump::HandleFinalDump(dump);
  }
}