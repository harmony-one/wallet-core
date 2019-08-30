// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include <cstdint>
#include <stdint.h>

#include "Signer.h"
#include "../Ethereum/RLP.h"

using namespace TW;
using namespace TW::Harmony;

struct T : TW::Ethereum::RLP {

    static Data encode(const TW::Harmony::Transaction &transaction) {
        auto encoded = Data();
        using namespace TW::Ethereum;
        append(encoded, RLP::encode(transaction.nonce));
        append(encoded, RLP::encode(transaction.gasPrice));
        append(encoded, RLP::encode(transaction.gasLimit));
        append(encoded, RLP::encode(transaction.fromShardID));
        append(encoded, RLP::encode(transaction.toShardID));
        append(encoded, RLP::encode(transaction.to.bytes));
        append(encoded, RLP::encode(transaction.amount));
        append(encoded, RLP::encode(transaction.payload));
        append(encoded, RLP::encode(transaction.v));
        append(encoded, RLP::encode(transaction.r));
        append(encoded, RLP::encode(transaction.s));
        return encodeList(encoded);
    }

    template <typename T>
    static Data encodeList(T elements) noexcept {
        auto encodedData = Data();
        for (const auto &el : elements) {
            auto encoded = TW::Ethereum::RLP::encode(el);
            if (encoded.empty()) {
                return {};
            }
            encodedData.insert(encodedData.end(), encoded.begin(), encoded.end());
        }

        auto encoded = encodeHeader(encodedData.size(), 0xc0, 0xf7);
        encoded.insert(encoded.end(), encodedData.begin(), encodedData.end());
        return encoded;
    }
};

std::tuple<uint256_t, uint256_t, uint256_t> Signer::values(const uint256_t &chainID,
                                                           const Data &signature) noexcept {
    boost::multiprecision::uint256_t r, s, v;
    import_bits(r, signature.begin(), signature.begin() + 32);
    import_bits(s, signature.begin() + 32, signature.begin() + 64);
    import_bits(v, signature.begin() + 64, signature.begin() + 65);
    v += 27;

    boost::multiprecision::uint256_t newV;
    if (chainID != 0) {
        import_bits(newV, signature.begin() + 64, signature.begin() + 65);
        newV += 35 + chainID + chainID;
    } else {
        newV = v;
    }
    return std::make_tuple(r, s, newV);
}

std::tuple<uint256_t, uint256_t, uint256_t>
Signer::sign(const uint256_t &chainID, const PrivateKey &privateKey, const Data &hash) noexcept {
    auto signature = privateKey.sign(hash, TWCurveSECP256k1);
    return values(chainID, signature);
}

Proto::SigningOutput Signer::sign(const TW::Harmony::Proto::SigningInput &input) const noexcept {
    auto key = PrivateKey(Data(input.private_key().begin(), input.private_key().end()));
    auto transaction = Transaction(
        /* nonce: */ load(input.nonce()),
        /* gasPrice: */ load(input.gas_price()),
        /* gasLimit: */ load(input.gas_limit()),
        /* fromShardID */ load(input.from_shard_id()),
        /* toShardID */ load(input.to_shard_id()),
        /* to: */ Address(input.to_address()),
        /* amount: */ load(input.amount()),
        /* payload: */ Data(input.payload().begin(), input.payload().end()));
    sign(key, transaction);
    auto protoOutput = Proto::SigningOutput();
    auto encoded = T::encode(transaction);
    protoOutput.set_encoded(encoded.data(), encoded.size());
    auto v = store(transaction.v);
    protoOutput.set_v(v.data(), v.size());
    auto r = store(transaction.r);
    protoOutput.set_r(r.data(), r.size());
    auto s = store(transaction.s);
    protoOutput.set_s(s.data(), s.size());
    return protoOutput;
}

void Signer::sign(const PrivateKey &privateKey, Transaction &transaction) const noexcept {
    auto hash = this->hash(transaction);
    auto tuple = Signer::sign(chainID, privateKey, hash);

    transaction.r = std::get<0>(tuple);
    transaction.s = std::get<1>(tuple);
    transaction.v = std::get<2>(tuple);
}

Data Signer::hash(const Transaction &transaction) const noexcept {
    auto encoded = Data();
    using namespace TW::Ethereum;
    append(encoded, RLP::encode(transaction.nonce));
    append(encoded, RLP::encode(transaction.gasPrice));
    append(encoded, RLP::encode(transaction.gasLimit));
    append(encoded, RLP::encode(transaction.fromShardID));
    append(encoded, RLP::encode(transaction.toShardID));
    append(encoded, RLP::encode(transaction.to.bytes));
    append(encoded, RLP::encode(transaction.amount));
    append(encoded, RLP::encode(transaction.payload));
    append(encoded, RLP::encode(chainID));
    append(encoded, RLP::encode(0));
    append(encoded, RLP::encode(0));
    return Hash::keccak256(T::encodeList(encoded));
}
