COMMAND(AX, 200, {
  assert(index > 0);
  program[index - 1] += 10;
  program[index++] = 0;
})

COMMAND(BX, 201, {
  assert(index > 0);
  program[index - 1] += 10;
  program[index++] = 1;
})

COMMAND(CX, 202, {
  assert(index > 0);
  program[index - 1] += 10;
  program[index++] = 2;
})

COMMAND(DX, 203, {
  assert(index > 0);
  program[index - 1] += 10;
  program[index++] = 3;
})

COMMAND(JMP, jmp, {
  program[index++] = jmp;
  ++index;
})

COMMAND(JA, ja, {
  program[index++] = ja;
  ++index;
})

COMMAND(JAE, jae, {
  program[index++] = jae;
  ++index;
})

COMMAND(JB, jb, {
  program[index++] = jb;
  ++index;
})

COMMAND(JBE, jbe, {
  program[index++] = jbe;
  ++index;
})

COMMAND(JE, je, {
  //printf("%d\n", je);
  program[index++] = je;
  ++index;
})

COMMAND(JNE, jne, {
  program[index++] = jne;
  ++index;
})





