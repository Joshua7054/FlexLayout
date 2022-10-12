/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include "Filates.h"

using FILTVector = std::vector<FILTNodeRef>;

FILT_EXTERN_C_BEGIN

WIN_EXPORT float FILTRoundValueToPixelGrid(const float value,
                                         const float pointScaleFactor,
                                         const bool forceCeil,
                                         const bool forceFloor);

FILT_EXTERN_C_END

extern const std::array<FILTEdge, 4> trailing;
extern const std::array<FILTEdge, 4> leading;
extern bool FILTValueEqual(const FILTValue a, const FILTValue b);
extern const FILTValue FILTValueUndefined;
extern const FILTValue FILTValueAuto;
extern const FILTValue FILTValueZero;

template <std::size_t size>
bool FILTValueArrayEqual(
    const std::array<FILTValue, size> val1,
    const std::array<FILTValue, size> val2) {
  bool areEqual = true;
  for (uint32_t i = 0; i < size && areEqual; ++i) {
    areEqual = FILTValueEqual(val1[i], val2[i]);
  }
  return areEqual;
}

struct FILTCachedMeasurement {
  float availableWidth;
  float availableHeight;
  FILTMeasureMode widthMeasureMode;
  FILTMeasureMode heightMeasureMode;

  float computedWidth;
  float computedHeight;

  FILTCachedMeasurement()
      : availableWidth(0),
        availableHeight(0),
        widthMeasureMode((FILTMeasureMode)-1),
        heightMeasureMode((FILTMeasureMode)-1),
        computedWidth(-1),
        computedHeight(-1) {}

  bool operator==(FILTCachedMeasurement measurement) const {
    bool isEqual = widthMeasureMode == measurement.widthMeasureMode &&
        heightMeasureMode == measurement.heightMeasureMode;

    if (!std::isnan(availableWidth) ||
        !std::isnan(measurement.availableWidth)) {
      isEqual = isEqual && availableWidth == measurement.availableWidth;
    }
    if (!std::isnan(availableHeight) ||
        !std::isnan(measurement.availableHeight)) {
      isEqual = isEqual && availableHeight == measurement.availableHeight;
    }
    if (!std::isnan(computedWidth) || !std::isnan(measurement.computedWidth)) {
      isEqual = isEqual && computedWidth == measurement.computedWidth;
    }
    if (!std::isnan(computedHeight) ||
        !std::isnan(measurement.computedHeight)) {
      isEqual = isEqual && computedHeight == measurement.computedHeight;
    }

    return isEqual;
  }
};

// This value was chosen based on empiracle data. Even the most complicated
// layouts should not require more than 16 entries to fit within the cache.
#define FILT_MAX_CACHED_RESULT_COUNT 16

struct FILTConfig {
  bool experimentalFeatures[FILTExperimentalFeatureCount + 1];
  bool useWebDefaults;
  bool useLegacyStretchBehaviour;
  bool shouldDiffLayoutWithoutLegacyStretchBehaviour;
  float pointScaleFactor;
  FILTLogger logger;
  FILTNodeClonedFunc cloneNodeCallback;
  void* context;
};

static const float kDefaultFlexGrow = 0.0f;
static const float kDefaultFlexShrink = 0.0f;
static const float kWebDefaultFlexShrink = 1.0f;

extern bool FILTFloatsEqual(const float a, const float b);
extern bool FILTValueEqual(const FILTValue a, const FILTValue b);
extern const FILTValue* FILTComputedEdgeValue(
    const std::array<FILTValue, FILTEdgeCount>& edges,
    const FILTEdge edge,
    const FILTValue* const defaultValue);
