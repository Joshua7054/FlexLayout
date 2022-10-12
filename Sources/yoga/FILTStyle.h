/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once
#include "Filates-internal.h"
#include "Filates.h"

struct FILTStyle {
  FILTDirection direction;
  FILTFlexDirection flexDirection;
  FILTJustify justifyContent;
  FILTAlign alignContent;
  FILTAlign alignItems;
  FILTAlign alignSelf;
  FILTPositionType positionType;
  FILTWrap flexWrap;
  FILTOverflow overflow;
  FILTDisplay display;
  float flex;
  float flexGrow;
  float flexShrink;
  FILTValue flexBasis;
  std::array<FILTValue, FILTEdgeCount> margin;
  std::array<FILTValue, FILTEdgeCount> position;
  std::array<FILTValue, FILTEdgeCount> padding;
  std::array<FILTValue, FILTEdgeCount> border;
  std::array<FILTValue, 2> dimensions;
  std::array<FILTValue, 2> minDimensions;
  std::array<FILTValue, 2> maxDimensions;
  float aspectRatio;

  FILTStyle();
  // Yoga specific properties, not compatible with flexbox specification
  bool operator==(const FILTStyle& style);

  bool operator!=(FILTStyle style);
  ~FILTStyle();
};
