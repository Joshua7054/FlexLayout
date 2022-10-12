/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#include "FILTLayout.h"

const std::array<float, 2> kFILTDefaultDimensionValues = {
    {FILTUndefined, FILTUndefined}};

FILTLayout::FILTLayout()
    : position(),
      dimensions(kFILTDefaultDimensionValues),
      margin(),
      border(),
      padding(),
      direction(FILTDirectionInherit),
      computedFlexBasisGeneration(0),
      computedFlexBasis(FILTUndefined),
      hadOverflow(false),
      generationCount(0),
      lastParentDirection((FILTDirection)-1),
      nextCachedMeasurementsIndex(0),
      cachedMeasurements(),
      measuredDimensions(kFILTDefaultDimensionValues),
      cachedLayout(FILTCachedMeasurement()),
      didUseLegacyFlag(false),
      doesLegacyStretchFlagAffectsLayout(false) {}

bool FILTLayout::operator==(FILTLayout layout) const {
  bool isEqual = position == layout.position &&
      dimensions == layout.dimensions && margin == layout.margin &&
      border == layout.border && padding == layout.padding &&
      direction == layout.direction && hadOverflow == layout.hadOverflow &&
      lastParentDirection == layout.lastParentDirection &&
      nextCachedMeasurementsIndex == layout.nextCachedMeasurementsIndex &&
      cachedLayout == layout.cachedLayout;

  for (uint32_t i = 0; i < FILT_MAX_CACHED_RESULT_COUNT && isEqual; ++i) {
    isEqual = isEqual && cachedMeasurements[i] == layout.cachedMeasurements[i];
  }

  if (!FILTFloatIsUndefined(computedFlexBasis) ||
      !FILTFloatIsUndefined(layout.computedFlexBasis)) {
    isEqual = isEqual && (computedFlexBasis == layout.computedFlexBasis);
  }
  if (!FILTFloatIsUndefined(measuredDimensions[0]) ||
      !FILTFloatIsUndefined(layout.measuredDimensions[0])) {
    isEqual =
        isEqual && (measuredDimensions[0] == layout.measuredDimensions[0]);
  }
  if (!FILTFloatIsUndefined(measuredDimensions[1]) ||
      !FILTFloatIsUndefined(layout.measuredDimensions[1])) {
    isEqual =
        isEqual && (measuredDimensions[1] == layout.measuredDimensions[1]);
  }

  return isEqual;
}

bool FILTLayout::operator!=(FILTLayout layout) const {
  return !(*this == layout);
}
