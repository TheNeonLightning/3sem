enum CmdCode {
  push = 1,
  pop = 2,
  in = 3,
  out = 4,
  add = 5,
  sub = 6,
  mul = 7,
  div_code = 8,
  sqrt_code = 9,
  pow_code = 10,
  end = 0,
  push_reg = 11,
  pop_to_reg = 12,

  jmp = 0b01101010, ///
  ja =  0b01100010, /// >
  jae = 0b01100011, /// >=
  jb =  0b01101000, /// <
  jbe = 0b01101001, /// <=
  je =  0b01100101, /// ==
  jne = 0b01101011, /// !=

  ax = 200,
  bx = 201,
  cx = 202,
  dx = 203
};