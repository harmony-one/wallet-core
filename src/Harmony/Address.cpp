// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.


#include "Address.h"
#include "AddressChecksum.h"
#include "../Hash.h"
#include "../HexCoding.h"

// YZ
#include "../Bech32.h"
using namespace TW::Harmony;

// Harmony uses the address length as that of Ethereum
 bool Address::isValid(const std::string& string) {
    /*if (string.size() != 42 || string[0] != '0' || string[1] != 'x') {
        return false;
    }
    const auto data = parse_hex(string);
    if (!Address::isValid(data))
        return false;
    // check Bech32 and checksum
    // std::string addr = string.substr(2); //without 0x
    */
     auto dec = Bech32::decode(string);
     if (dec.second.empty()) {
         return false;   //checksum is done here
     }
     if (dec.first != "one") {
         return false;
     }
     
     // Why Harmony address OK?
     // ??each byte from 5th to 8th bits
     // check??? from ioTex
     Data keyHash;
     auto success = Bech32::convertBits<5, 8, false>(keyHash, dec.second);
     if (!success || keyHash.size() != 20) {
         return false;
     }
     // check size?
     Data conv;
     auto success2 =
     Bech32::convertBits<5, 8, false>(conv, Data(dec.second.begin(), dec.second.end()));
     if (!success2 || conv.size() < 2 || conv.size() > 40) {
         return false;
     }
     return true;
 }

// TBV
Address::Address(const std::string& string) {
    if (!isValid(string)) {
        throw std::invalid_argument("Invalid address data");
    }
    const auto data = parse_hex(string);
    std::copy(data.begin(), data.end(), bytes.begin());
}

Address::Address(const Data& data) {
    if (!isValid(data)) {
        throw std::invalid_argument("Invalid address data");
    }
    std::copy(data.begin(), data.end(), bytes.begin());
}

Address::Address(const PublicKey& publicKey) {
    if (publicKey.type != TWPublicKeyTypeSECP256k1Extended) {
        throw std::invalid_argument("Ethereum::Address needs an extended SECP256k1 public key.");
    }
    const auto data = publicKey.hash({}, static_cast<Data(*)(const byte*, const byte*)>(Hash::keccak256), true);
    
    // YZ: For Harmony Address, Bech32 is used to encode Ethereum Address which is a keccak256 hash
    // TBD: encoding to Bech32
    //
    
    //
    
    std::copy(data.end() - Address::size, data.end(), bytes.begin());
}

std::string Address::string() const {
    return checksumed(*this, ChecksumType::eip55);
}

