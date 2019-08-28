// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Harmony/Address.h"
#include "Harmony/RLP.h"
#include "Harmony/Signer.h"
#include "HexCoding.h"

#include <gtest/gtest.h>

namespace TW::Harmony {

using boost::multiprecision::uint256_t;

class SignerExposed : public Signer {
public:
    SignerExposed(boost::multiprecision::uint256_t chainID) : Signer(chainID) {}
    using Signer::hash;
};

    // YZ: this can be "raw" transaction! The hashvalue can be different than
    // go code???  They can be the same as long as the data items packed in the
    // same way?
    TEST(Signer, HarmonyFrontierHash) {
        //auto address = Address("0xd7ff41ca29306122185a07d04293ddb35f24cf2d");
        auto address = Address("one18t4yj4fuutj83uwqckkvxp9gfa0568uc48ggj7");
        auto transaction = Transaction(
                                       /* nonce: */ 0x0,
                                       /* gasPrice: */ 20000000000,
                                       /* gasLimit: */ 21000,
                                       /* shardID:  */ 0,
                                       /* toShardID:*/ 0,
                                       /* to: */ address,
                                       ///* amount: */ 0x1158e460913d00000,
                                       //Err: integer literal is too large to be represented in any integer type
                                       
                                       /* amount: */ 10, //0x58e460913d,
                                       
                                       /* payload: */ {}
                                       );
        //auto signer = SignerExposed(1);
        auto signer = SignerExposed(0); //YZ: without the chainID
        
        auto hash = signer.hash(transaction);
        
        ASSERT_EQ(hex(hash), "1156a044c73f85d876780f3bd86d7b0665c2d1b6c8a1abaaaedc08c13968a598");
    }

TEST(Signer, Hash) {
    auto address = Address("0x3535353535353535353535353535353535353535");
    auto transaction = Transaction(
        /* nonce: */ 9,
        /* gasPrice: */ 20000000000,
        /* gasLimit: */ 21000,
        0,0,
        /* to: */ address,
        /* amount: */ 1000000000000000000,
        /* payload: */ {}
    );
    auto signer = SignerExposed(1);
    auto hash = signer.hash(transaction);
    ASSERT_EQ(hex(hash), "daf5a779ae972f972197303d7b574746c7ef83eadac0f2791ad23db92e4c8e53");
}

    
    
TEST(Signer, Sign) {
    auto address = Address("0x3535353535353535353535353535353535353535");
    auto transaction = Transaction(
        /* nonce: */ 9,
        /* gasPrice: */ 20000000000,
        /* gasLimit: */ 21000,
        0, 0,
        /* to: */ address,
        /* amount: */ 1000000000000000000,
        /* payload: */ {}
    );

    auto key = PrivateKey(parse_hex("0x4646464646464646464646464646464646464646464646464646464646464646"));
    auto signer = SignerExposed(1);
    signer.sign(key, transaction);
    ASSERT_EQ(transaction.v, 37);
    ASSERT_EQ(transaction.r, uint256_t("18515461264373351373200002665853028612451056578545711640558177340181847433846"));
    ASSERT_EQ(transaction.s, uint256_t("46948507304638947509940763649030358759909902576025900602547168820602576006531"));
}

} // namespace TW::Harmony
