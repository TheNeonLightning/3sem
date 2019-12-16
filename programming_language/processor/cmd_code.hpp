/**
 * \file
 * \brief THis file is containing all commands and their codes
 *
 * The codes of the jump commands are done in a special way:
 * the first 4 bites are the same for all jump commands and are equal to
 * 0x01100000 which is a letter 'j' in ACSII table. Each of the following 4
 * bites represents its own meaning:
 * 0x00001000 stands for <
 * 0x00000100 stands for = (which will be first in "==")
 * 0x00000010 stands for >
 * 0x00000001 stands for = (which will be second in "==" or "!=" etc.)
 *
 * To represent the jne command ("!=") the '<' and  the '>' bites will be set as
 * 1;
 *
 * For example the code 0x01101001 stands for jbe command ("<=").
 *
 * Than receiving a register code the code of the previous command will be
 * increased by 10 changing it meaning. For example having read the push command
 * and than receiving a register code means the push command (code: 1) will
 * automatically
 * become push_reg (code: 11) command.
 */

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