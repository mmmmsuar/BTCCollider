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
#ifndef SECP256K1H
#define SECP256K1H

#include "Point.h"
#include <string>
#include <vector>

// Address type
#define P2PKH  0
#define P2SH   1
#define BECH32 2

class SECP256k1 {

public:

  SECP256k1();
  ~SECP256k1();
  void Init();
  
  // Method to compute public key using the Int class (existing method)
  Point ComputePublicKey(Int *privKey);

  // New static utility method for converting private key to public key using scalar multiplication
  static Point privateKeyToPublicKey(const Int &privKey) {
    secp256k1_gej publicKey;
    secp256k1_scalar privateKeyScalar;
    secp256k1_scalar_set_int(&privateKeyScalar, privKey.GetInt32());  // Adjust based on your Int class implementation
    secp256k1_ecmult_gen(&ctx, &publicKey, &privateKeyScalar);  // Scalar multiplication
    Point result;
    // Populate the Point result based on secp256k1_gej
    return result;
  }

  Point NextKey(Point &key);
  void Check();
  bool EC(Point &p);

  // For address generation
  void GetHash160(int type, bool compressed,
    Point &k0, Point &k1, Point &k2, Point &k3,
    uint8_t *h0, uint8_t *h1, uint8_t *h2, uint8_t *h3);

  void GetCompressedHash160(int type,
    Int *x0, Int *x1, Int *x2, Int *x3,
    bool y0Odd, bool y1Odd, bool y2Odd, bool y3Odd,
    uint8_t *h0, uint8_t *h1, uint8_t *h2, uint8_t *h3);

  void GetHash160(int type, bool compressed, Point &pubKey, unsigned char *hash);
  void GetCompressedHash160(int type, Int *x, bool yOdd, unsigned char *hash);

  // For generating Bitcoin addresses
  std::string GetAddress(int type, bool compressed, Point &pubKey);
  std::string GetAddress(int type, bool compressed, unsigned char *hash160);
  std::vector<std::string> GetAddress(int type, bool compressed, unsigned char *h1, unsigned char *h2, unsigned char *h3, unsigned char *h4);
  std::string GetPrivAddress(bool compressed, Int &privKey);
  std::string GetPublicKeyHex(bool compressed, Point &p);
  Point ParsePublicKeyHex(std::string str, bool &isCompressed);

  bool CheckPudAddress(std::string address);

  static Int DecodePrivateKey(char *key, bool *compressed);

  // Elliptic Curve Operations
  Point Add(Point &p1, Point &p2);
  Point Add2(Point &p1, Point &p2);
  Point AddDirect(Point &p1, Point &p2);
  Point Double(Point &p);
  Point DoubleDirect(Point &p);

  Point G;   // Generator
  Int order; // Curve order

private:

  uint8_t GetByte(std::string &str, int idx);
  Int GetY(Int x, bool isEven);
  Point GTable[256 * 32]; // Generator table
};

#endif // SECP256K1H
