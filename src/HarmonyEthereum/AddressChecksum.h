// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include <string>

namespace TW::Harmony {

/// Checksum types for Ethereum-based blockchains.
/// YZ: check to see if same for Harmony blockchain ??
enum ChecksumType {
    eip55 = 0,
    wanchain = 1,
};

std::string checksumed(const Address& address, enum ChecksumType type);

} // namespace TW::Harmony
