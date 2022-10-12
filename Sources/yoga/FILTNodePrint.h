/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#pragma once
#include <string>

#include "Filates.h"

namespace facebook {
namespace filates {

void FILTNodeToString(
    std::string* str,
    FILTNodeRef node,
    FILTPrintOptions options,
    uint32_t level);

} // namespace filates
} // namespace facebook
