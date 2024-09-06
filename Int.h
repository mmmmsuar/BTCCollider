/*
 * This file is part of the BTCCollider distribution (https://github.com/JeanLucPons/BTCCollider).
 * Copyright (c) 2020 Jean Luc PONS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// Big integer class (Fixed size)

#ifndef BIGINTH
#define BIGINTH

#include "Random.h"
#include <string>
#include <inttypes.h>

// We need 1 extra block for Knuth div algorithm , Montgomery multiplication and ModInv
#define BISIZE 256

#if BISIZE==256
  #define NB64BLOCK 5
  #define NB32BLOCK 10
#elif BISIZE==512
  #define NB64BLOCK 9
  #define NB32BLOCK 18
#else
  #error Unsupported size
#endif

class Int {

public:

  Int();
  Int(int64_t i64);
  Int(Int *a);

  // Op
  void Add(uint64_t a);
  void Add(Int *a);
  void Add(Int *a, Int *b);
  void AddOne();
  void Sub(uint64_t a);
  void Sub(Int *a);
  void Sub(Int *a, Int *b);
  void SubOne();
  void Mult(Int *a);
  void Mult(uint64_t a);
  void IMult(int64_t a);
  void Mult(Int *a, uint64_t b);
  void IMult(Int *a, int64_t b);
  void Mult(Int *a, Int *b);
  void Div(Int *a, Int *mod = NULL);
  void MultModN(Int *a, Int *b, Int *n);
  void Neg();
  void Abs();

  // Right shift (signed)
  void ShiftR(uint32_t n);
  void ShiftR32Bit();
  void ShiftR64Bit();
  // Left shift
  void ShiftL(uint32_t n);
  void ShiftL32Bit();
  void ShiftL64Bit();

  // Comp 
  bool IsGreater(Int *a);
  bool IsGreaterOrEqual(Int *a);
  bool IsLowerOrEqual(Int *a);
  bool IsLower(Int *a);
  bool IsEqual(Int *a);
  bool IsZero();
  bool IsOne();
  bool IsStrictPositive();
  bool IsPositive();
  bool IsNegative();
  bool IsEven();
  bool IsOdd();

  // Modular arithmetic
  static void SetupField(Int *n, Int *R = NULL, Int *R2 = NULL, Int *R3 = NULL, Int *R4 = NULL);
  static Int *GetR();
  static Int *GetR2();
  static Int *GetR3();
  static Int *GetR4();
  static Int* GetFieldCharacteristic();
  
  void GCD(Int *a);
  void Mod(Int *n);
  void ModInv();
  void MontgomeryMult(Int *a, Int *b);
  void MontgomeryMult(Int *a);
  void ModAdd(Int *a);
  void ModAdd(Int *a, Int *b);
  void ModAdd(uint64_t a);
  void ModSub(Int *a);
  void ModSub(Int *a, Int *b);
  void ModSub(uint64_t a);
  void ModMul(Int *a, Int *b);
  void ModMul(Int *a);
  void ModSquare(Int *a);
  void ModCube(Int *a);
  void ModDouble();
  void ModExp(Int *e);
  void ModNeg();
  void ModSqrt();
  bool HasSqrt();

  // Specific SecpK1
  static void InitK1(Int *order);
  void ModMulK1(Int *a, Int *b);
  void ModMulK1(Int *a);
  void ModMulK1order(Int *a);
  void ModSquareK1(Int *a);
  void ModAddK1order(Int *a, Int *b);

  // Size
  int GetSize();
  int GetBitLength();

  // Setter
  void SetInt32(uint32_t value);
  void Set(Int *a);
  void SetBase10(char *value);
  void SetBase16(char *value);
  void SetBaseN(int n, char *charset, char *value);
  void SetByte(int n, unsigned char byte);
  void SetDWord(int n, uint32_t b);
  void SetQWord(int n, uint64_t b);
  void Rand(int nbit);
  void Set32Bytes(unsigned char *bytes);
  void MaskByte(int n);

  // Getter
  uint32_t GetInt32();
  int GetBit(uint32_t n);
  unsigned char GetByte(int n);
  void Get32Bytes(unsigned char *buff);

  // To String
  std::string GetBase2();
  std::string GetBase10();
  std::string GetBase16();
  std::string GetBaseN(int n, char *charset);
  std::string GetBlockStr();
  std::string GetC64Str(int nbDigit);

  // Check function
  static void Check();

  union {
    uint32_t bits[NB32BLOCK];
    uint64_t bits64[NB64BLOCK];
  };

private:

  void ShiftL32BitAndSub(Int *a, int n);
  uint64_t AddC(Int *a);
  void AddAndShift(Int *a, Int *b, uint64_t cH);
  void Mult(Int *a, uint32_t b);
  int  GetLowestBit();
  void CLEAR();
  void CLEARFF();
};

// Inline routines

#ifndef WIN64
static uint64_t inline _umul128(uint64_t a, uint64_t b, uint64_t *h) {
  uint64_t rhi;
  uint64_t rlo;
  __asm__("mulq  %[b];" :"=d"(rhi), "=a"(rlo) :"1"(a), [b]"rm"(b));
  *h = rhi;
  return rlo;
}

static uint64_t inline __shiftright128(uint64_t a, uint64_t b, unsigned char n) {
  uint64_t c;
  __asm__("movq %1,%0;shrdq %3,%2,%0;" : "=D"(c) : "r"(a), "r"(b), "c"(n));
  return c;
}

static uint64_t inline __shiftleft128(uint64_t a, uint64_t b, unsigned char n) {
  uint64_t c;
  __asm__("movq %1,%0;shldq %3,%2,%0;" : "=D"(c) : "r"(b), "r"(a), "c"(n));
  return c;
}

#define _subborrow_u64(a, b, c, d) __builtin_ia32_sbb_u64(a, b, c, (long long unsigned int*)d);
#define _addcarry_u64(a, b, c, d) __builtin_ia32_addcarryx_u64(a, b, c, (long long unsigned int*)d);
#define _byteswap_uint64 __builtin_bswap64
#else
#include <intrin.h>
#endif

#endif // BIGINTH
