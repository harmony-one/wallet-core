// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../PublicKey.h"

#include <array>
#include <cstdint>
#include <vector>


// YZ
namespace TW::Harmony {

class Address {
  public:
    /// Number of bytes in an address
    static const size_t size = 20;

    /// Address data consisting of a prefix byte followed by the public key
    /// hash.
    std::array<uint8_t, size> bytes;

    /// Determines whether a collection of bytes makes a valid  address.
    static bool isValid(const Data& data) { return data.size() == size; }

    /// Determines whether a string makes a valid  address.
    static bool isValid(const std::string& string);

    /// Initializes an address with a string representation.
    explicit Address(const std::string& string);

    /// Initializes an address with a collection of bytes.
    explicit Address(const Data& data);

    /// Initializes an address with a public key.
    explicit Address(const PublicKey& publicKey);

    /// Returns a string representation of the address.
    std::string string() const;
};

inline bool operator==(const Address& lhs, const Address& rhs) {
    return lhs.bytes == rhs.bytes;
}

} // namespace TW::Harmony

/// Wrapper for C interface.
struct TWHarmonyAddress {
    TW::Harmony::Address impl;
};

/*
 
 #pragma once
 
 #include <string>
 
 #include "Data.h"
 #include "PublicKey.h"
 
 namespace TW::Harmony {
 
 class Address {
 public:
 /// 20-byte public key hash
 Data keyHash;
 
 /// Determines whether a string makes a valid  address
 static bool isValid(const std::string& string);
 
 /// Determines whether a byte slice makes a valid  key hash
 static bool isValid(const Data& data) { return data.size() == 20; }
 
 /// Initializes an address with a string representation
 explicit Address(const std::string& string);
 
 /// Initializes an address with a public key
 explicit Address(const PublicKey& publicKey);
 
 /// Initializes an address with a key hash
 explicit Address(const Data& keyHash);
 
 /// Returns a string representation of the address
 std::string string() const;
 };
 
 inline bool operator==(const Address& lhs, const Address& rhs) {
 return lhs.keyHash == rhs.keyHash;
 }
 
 } // namespace TW::Harmony
 
 /// Wrapper for C interface
 struct TWHarmonyAddress {
 TW::Harmony::Address impl;
 };
 
 */

