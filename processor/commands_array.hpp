#include <vector>

using std::vector;

static vector<Processor::Cmd> commands = vector<Processor::Cmd>{{"push", push}, {"pop", pop},
{"in", in}, {"out", out}, {"add", add},
{"sub", sub}, {"mul", mul}, {"div", div_code},
{"sqrt", sqrt_code}, {"pow", pow_code}, {"end", end},
{"jmp", jmp}, {"ja", ja}, {"jae", jae}, {"jb", jb},
{"jbe", jbe}, {"je", je}, {"jne", jne}, {"ax", ax},
{"bx", bx}, {"cx", cx}, {"dx", dx}};