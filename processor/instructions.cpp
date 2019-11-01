INSTRUCTION(PUSH_CONST, 1, {
  stack.Push(GetConstValue(index));
  index += 4;
})

INSTRUCTION(POP, 2, {
  stack.Pop();
})

INSTRUCTION(OUT, 4, {
  value = stack.Pop();
  printf("%d ", value);
  stack.Push(value);
  fflush(stdout);
})

INSTRUCTION(ADD, 5, {
  stack.Push(stack.Pop() + stack.Pop());
})

INSTRUCTION(SUB, 6, {
  stack.Push(stack.Pop() - stack.Pop());
})
INSTRUCTION(MUL, 7, {
  stack.Push(stack.Pop() * stack.Pop());
})

INSTRUCTION(DIV, 8, {
  stack.Push(stack.Pop() / stack.Pop());
})

INSTRUCTION(SQRT, 9, {
  stack.Push(sqrt(stack.Pop()));
})

INSTRUCTION(POW, 10, {
  stack.Push(pow(stack.Pop(), stack.Pop()));
})

INSTRUCTION(PUSH_REG, 11, {
  stack.Push(registers[program[index++]]);
})

INSTRUCTION(POP_TO_REG, 12, {
  registers[program[index++]] = stack.Pop();
})

INSTRUCTION(IN_REG_CONST, 13, {
  registers[program[index++]] = GetConstValue(index);
  index += 4;
})




