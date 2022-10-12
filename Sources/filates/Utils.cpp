/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Utils.h"

FILTFlexDirection FILTFlexDirectionCross(
    const FILTFlexDirection flexDirection,
    const FILTDirection direction) {
  return FILTFlexDirectionIsColumn(flexDirection)
      ? FILTResolveFlexDirection(FILTFlexDirectionRow, direction)
      : FILTFlexDirectionColumn;
}

bool FILTValueEqual(const FILTValue a, const FILTValue b) {
  if (a.unit != b.unit) {
    return false;
  }

  if (a.unit == FILTUnitUndefined ||
      (std::isnan(a.value) && std::isnan(b.value))) {
    return true;
  }

  return fabs(a.value - b.value) < 0.0001f;
}
