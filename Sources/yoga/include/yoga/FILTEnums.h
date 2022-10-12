/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "FILTMacros.h"

FILT_EXTERN_C_BEGIN

#define FILTAlignCount 8
typedef FILT_ENUM_BEGIN(FILTAlign) {
  FILTAlignAuto,
  FILTAlignFlexStart,
  FILTAlignCenter,
  FILTAlignFlexEnd,
  FILTAlignStretch,
  FILTAlignBaseline,
  FILTAlignSpaceBetween,
  FILTAlignSpaceAround,
} FILT_ENUM_END(FILTAlign);
WIN_EXPORT const char *FILTAlignToString(const FILTAlign value);

#define FILTDimensionCount 2
typedef FILT_ENUM_BEGIN(FILTDimension) {
  FILTDimensionWidth,
  FILTDimensionHeight,
} FILT_ENUM_END(FILTDimension);
WIN_EXPORT const char *FILTDimensionToString(const FILTDimension value);

#define FILTDirectionCount 3
typedef FILT_ENUM_BEGIN(FILTDirection) {
  FILTDirectionInherit,
  FILTDirectionLTR,
  FILTDirectionRTL,
} FILT_ENUM_END(FILTDirection);
WIN_EXPORT const char *FILTDirectionToString(const FILTDirection value);

#define FILTDisplayCount 2
typedef FILT_ENUM_BEGIN(FILTDisplay) {
  FILTDisplayFlex,
  FILTDisplayNone,
} FILT_ENUM_END(FILTDisplay);
WIN_EXPORT const char *FILTDisplayToString(const FILTDisplay value);

#define FILTEdgeCount 9
typedef FILT_ENUM_BEGIN(FILTEdge) {
  FILTEdgeLeft,
  FILTEdgeTop,
  FILTEdgeRight,
  FILTEdgeBottom,
  FILTEdgeStart,
  FILTEdgeEnd,
  FILTEdgeHorizontal,
  FILTEdgeVertical,
  FILTEdgeAll,
} FILT_ENUM_END(FILTEdge);
WIN_EXPORT const char *FILTEdgeToString(const FILTEdge value);

#define FILTExperimentalFeatureCount 1
typedef FILT_ENUM_BEGIN(FILTExperimentalFeature) {
  FILTExperimentalFeatureWebFlexBasis,
} FILT_ENUM_END(FILTExperimentalFeature);
WIN_EXPORT const char *FILTExperimentalFeatureToString(const FILTExperimentalFeature value);

#define FILTFlexDirectionCount 4
typedef FILT_ENUM_BEGIN(FILTFlexDirection) {
  FILTFlexDirectionColumn,
  FILTFlexDirectionColumnReverse,
  FILTFlexDirectionRow,
  FILTFlexDirectionRowReverse,
} FILT_ENUM_END(FILTFlexDirection);
WIN_EXPORT const char *FILTFlexDirectionToString(const FILTFlexDirection value);

#define FILTJustifyCount 6
typedef FILT_ENUM_BEGIN(FILTJustify){
    FILTJustifyFlexStart,
    FILTJustifyCenter,
    FILTJustifyFlexEnd,
    FILTJustifySpaceBetween,
    FILTJustifySpaceAround,
    FILTJustifySpaceEvenly,
} FILT_ENUM_END(FILTJustify);
WIN_EXPORT const char *FILTJustifyToString(const FILTJustify value);

#define FILTLogLevelCount 6
typedef FILT_ENUM_BEGIN(FILTLogLevel) {
  FILTLogLevelError,
  FILTLogLevelWarn,
  FILTLogLevelInfo,
  FILTLogLevelDebug,
  FILTLogLevelVerbose,
  FILTLogLevelFatal,
} FILT_ENUM_END(FILTLogLevel);
WIN_EXPORT const char *FILTLogLevelToString(const FILTLogLevel value);

#define FILTMeasureModeCount 3
typedef FILT_ENUM_BEGIN(FILTMeasureMode) {
  FILTMeasureModeUndefined,
  FILTMeasureModeExactly,
  FILTMeasureModeAtMost,
} FILT_ENUM_END(FILTMeasureMode);
WIN_EXPORT const char *FILTMeasureModeToString(const FILTMeasureMode value);

#define FILTNodeTypeCount 2
typedef FILT_ENUM_BEGIN(FILTNodeType) {
  FILTNodeTypeDefault,
  FILTNodeTypeText,
} FILT_ENUM_END(FILTNodeType);
WIN_EXPORT const char *FILTNodeTypeToString(const FILTNodeType value);

#define FILTOverflowCount 3
typedef FILT_ENUM_BEGIN(FILTOverflow) {
  FILTOverflowVisible,
  FILTOverflowHidden,
  FILTOverflowScroll,
} FILT_ENUM_END(FILTOverflow);
WIN_EXPORT const char *FILTOverflowToString(const FILTOverflow value);

#define FILTPositionTypeCount 2
typedef FILT_ENUM_BEGIN(FILTPositionType) {
  FILTPositionTypeRelative,
  FILTPositionTypeAbsolute,
} FILT_ENUM_END(FILTPositionType);
WIN_EXPORT const char *FILTPositionTypeToString(const FILTPositionType value);

#define FILTPrintOptionsCount 3
typedef FILT_ENUM_BEGIN(FILTPrintOptions) {
  FILTPrintOptionsLayout = 1,
  FILTPrintOptionsStyle = 2,
  FILTPrintOptionsChildren = 4,
} FILT_ENUM_END(FILTPrintOptions);
WIN_EXPORT const char *FILTPrintOptionsToString(const FILTPrintOptions value);

#define FILTUnitCount 4
typedef FILT_ENUM_BEGIN(FILTUnit) {
  FILTUnitUndefined,
  FILTUnitPoint,
  FILTUnitPercent,
  FILTUnitAuto,
} FILT_ENUM_END(FILTUnit);
WIN_EXPORT const char *FILTUnitToString(const FILTUnit value);

#define FILTWrapCount 3
typedef FILT_ENUM_BEGIN(FILTWrap) {
  FILTWrapNoWrap,
  FILTWrapWrap,
  FILTWrapWrapReverse,
} FILT_ENUM_END(FILTWrap);
WIN_EXPORT const char *FILTWrapToString(const FILTWrap value);

FILT_EXTERN_C_END
