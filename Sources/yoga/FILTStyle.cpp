/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#include "FILTStyle.h"

const FILTValue kFILTValueUndefined = {FILTUndefined, FILTUnitUndefined};

const FILTValue kFILTValueAuto = {FILTUndefined, FILTUnitAuto};

const std::array<FILTValue, FILTEdgeCount> kFILTDefaultEdgeValuesUnit = {
    {kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined,
     kFILTValueUndefined}};

const std::array<FILTValue, 2> kFILTDefaultDimensionValuesAutoUnit = {
    {kFILTValueAuto, kFILTValueAuto}};

const std::array<FILTValue, 2> kFILTDefaultDimensionValuesUnit = {
    {kFILTValueUndefined, kFILTValueUndefined}};

FILTStyle::FILTStyle()
    : direction(FILTDirectionInherit),
      flexDirection(FILTFlexDirectionColumn),
      justifyContent(FILTJustifyFlexStart),
      alignContent(FILTAlignFlexStart),
      alignItems(FILTAlignStretch),
      alignSelf(FILTAlignAuto),
      positionType(FILTPositionTypeRelative),
      flexWrap(FILTWrapNoWrap),
      overflow(FILTOverflowVisible),
      display(FILTDisplayFlex),
      flex(FILTUndefined),
      flexGrow(FILTUndefined),
      flexShrink(FILTUndefined),
      flexBasis(kFILTValueAuto),
      margin(kFILTDefaultEdgeValuesUnit),
      position(kFILTDefaultEdgeValuesUnit),
      padding(kFILTDefaultEdgeValuesUnit),
      border(kFILTDefaultEdgeValuesUnit),
      dimensions(kFILTDefaultDimensionValuesAutoUnit),
      minDimensions(kFILTDefaultDimensionValuesUnit),
      maxDimensions(kFILTDefaultDimensionValuesUnit),
      aspectRatio(FILTUndefined) {}

// Yoga specific properties, not compatible with flexbox specification
bool FILTStyle::operator==(const FILTStyle& style) {
  bool areNonFloatValuesEqual = direction == style.direction &&
      flexDirection == style.flexDirection &&
      justifyContent == style.justifyContent &&
      alignContent == style.alignContent && alignItems == style.alignItems &&
      alignSelf == style.alignSelf && positionType == style.positionType &&
      flexWrap == style.flexWrap && overflow == style.overflow &&
      display == style.display && FILTValueEqual(flexBasis, style.flexBasis) &&
      FILTValueArrayEqual(margin, style.margin) &&
      FILTValueArrayEqual(position, style.position) &&
      FILTValueArrayEqual(padding, style.padding) &&
      FILTValueArrayEqual(border, style.border) &&
      FILTValueArrayEqual(dimensions, style.dimensions) &&
      FILTValueArrayEqual(minDimensions, style.minDimensions) &&
      FILTValueArrayEqual(maxDimensions, style.maxDimensions);

  if (!(std::isnan(flex) && std::isnan(style.flex))) {
    areNonFloatValuesEqual = areNonFloatValuesEqual && flex == style.flex;
  }

  if (!(std::isnan(flexGrow) && std::isnan(style.flexGrow))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && flexGrow == style.flexGrow;
  }

  if (!(std::isnan(flexShrink) && std::isnan(style.flexShrink))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && flexShrink == style.flexShrink;
  }

  if (!(std::isnan(aspectRatio) && std::isnan(style.aspectRatio))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && aspectRatio == style.aspectRatio;
  }

  return areNonFloatValuesEqual;
}

bool FILTStyle::operator!=(FILTStyle style) {
  return !(*this == style);
}

FILTStyle::~FILTStyle() {}
