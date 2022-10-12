/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

// Not defined in MSVC++
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *) __nan)
#endif

#define FILTUndefined NAN

#include "FILTEnums.h"
#include "FILTMacros.h"

FILT_EXTERN_C_BEGIN

typedef struct FILTSize {
  float width;
  float height;
} FILTSize;

typedef struct FILTValue {
  float value;
  FILTUnit unit;
} FILTValue;

extern const FILTValue FILTValueUndefined;
extern const FILTValue FILTValueAuto;

typedef struct FILTConfig *FILTConfigRef;

typedef struct FILTNode* FILTNodeRef;

typedef FILTSize (*FILTMeasureFunc)(FILTNodeRef node,
                                float width,
                                FILTMeasureMode widthMode,
                                float height,
                                FILTMeasureMode heightMode);
typedef float (*FILTBaselineFunc)(FILTNodeRef node, const float width, const float height);
typedef void (*FILTDirtiedFunc)(FILTNodeRef node);
typedef void (*FILTPrintFunc)(FILTNodeRef node);
typedef int (*FILTLogger)(const FILTConfigRef config,
                        const FILTNodeRef node,
                        FILTLogLevel level,
                        const char *format,
                        va_list args);
typedef void (*FILTNodeClonedFunc)(FILTNodeRef oldNode,
                                 FILTNodeRef newNode,
                                 FILTNodeRef parent,
                                 int childIndex);

// FILTNode
WIN_EXPORT FILTNodeRef FILTNodeNew(void);
WIN_EXPORT FILTNodeRef FILTNodeNewWithConfig(const FILTConfigRef config);
WIN_EXPORT FILTNodeRef FILTNodeClone(const FILTNodeRef node);
WIN_EXPORT void FILTNodeFree(const FILTNodeRef node);
WIN_EXPORT void FILTNodeFreeRecursive(const FILTNodeRef node);
WIN_EXPORT void FILTNodeReset(const FILTNodeRef node);
WIN_EXPORT int32_t FILTNodeGetInstanceCount(void);

WIN_EXPORT void FILTNodeInsertChild(const FILTNodeRef node,
                                  const FILTNodeRef child,
                                  const uint32_t index);
WIN_EXPORT void FILTNodeRemoveChild(const FILTNodeRef node, const FILTNodeRef child);
WIN_EXPORT void FILTNodeRemoveAllChildren(const FILTNodeRef node);
WIN_EXPORT FILTNodeRef FILTNodeGetChild(const FILTNodeRef node, const uint32_t index);
WIN_EXPORT FILTNodeRef FILTNodeGetParent(const FILTNodeRef node);
WIN_EXPORT uint32_t FILTNodeGetChildCount(const FILTNodeRef node);

WIN_EXPORT void FILTNodeCalculateLayout(const FILTNodeRef node,
                                      const float availableWidth,
                                      const float availableHeight,
                                      const FILTDirection parentDirection);

// Mark a node as dirty. Only valid for nodes with a custom measure function
// set.
// FILT knows when to mark all other nodes as dirty but because nodes with
// measure functions
// depends on information not known to FILT they must perform this dirty
// marking manually.
WIN_EXPORT void FILTNodeMarkDirty(const FILTNodeRef node);

// This function marks the current node and all its descendants as dirty. This function is added to test filates benchmarks.
// This function is not expected to be used in production as calling `FILTCalculateLayout` will cause the recalculation of each and every node.
WIN_EXPORT void FILTNodeMarkDirtyAndPropogateToDescendants(const FILTNodeRef node);

WIN_EXPORT void FILTNodePrint(const FILTNodeRef node, const FILTPrintOptions options);

WIN_EXPORT bool FILTFloatIsUndefined(const float value);

WIN_EXPORT bool FILTNodeCanUseCachedMeasurement(const FILTMeasureMode widthMode,
                                              const float width,
                                              const FILTMeasureMode heightMode,
                                              const float height,
                                              const FILTMeasureMode lastWidthMode,
                                              const float lastWidth,
                                              const FILTMeasureMode lastHeightMode,
                                              const float lastHeight,
                                              const float lastComputedWidth,
                                              const float lastComputedHeight,
                                              const float marginRow,
                                              const float marginColumn,
                                              const FILTConfigRef config);

WIN_EXPORT void FILTNodeCopyStyle(const FILTNodeRef dstNode, const FILTNodeRef srcNode);

#define FILT_NODE_PROPERTY(type, name, paramName)                          \
  WIN_EXPORT void FILTNodeSet##name(const FILTNodeRef node, type paramName); \
  WIN_EXPORT type FILTNodeGet##name(const FILTNodeRef node);

#define FILT_NODE_STYLE_PROPERTY(type, name, paramName)                               \
  WIN_EXPORT void FILTNodeStyleSet##name(const FILTNodeRef node, const type paramName); \
  WIN_EXPORT type FILTNodeStyleGet##name(const FILTNodeRef node);

#define FILT_NODE_STYLE_PROPERTY_UNIT(type, name, paramName)                                    \
  WIN_EXPORT void FILTNodeStyleSet##name(const FILTNodeRef node, const float paramName);          \
  WIN_EXPORT void FILTNodeStyleSet##name##Percent(const FILTNodeRef node, const float paramName); \
  WIN_EXPORT type FILTNodeStyleGet##name(const FILTNodeRef node);

#define FILT_NODE_STYLE_PROPERTY_UNIT_AUTO(type, name, paramName) \
  FILT_NODE_STYLE_PROPERTY_UNIT(type, name, paramName)            \
  WIN_EXPORT void FILTNodeStyleSet##name##Auto(const FILTNodeRef node);

#define FILT_NODE_STYLE_EDGE_PROPERTY(type, name, paramName)    \
  WIN_EXPORT void FILTNodeStyleSet##name(const FILTNodeRef node,  \
                                       const FILTEdge edge,     \
                                       const type paramName); \
  WIN_EXPORT type FILTNodeStyleGet##name(const FILTNodeRef node, const FILTEdge edge);

#define FILT_NODE_STYLE_EDGE_PROPERTY_UNIT(type, name, paramName)         \
  WIN_EXPORT void FILTNodeStyleSet##name(const FILTNodeRef node,            \
                                       const FILTEdge edge,               \
                                       const float paramName);          \
  WIN_EXPORT void FILTNodeStyleSet##name##Percent(const FILTNodeRef node,   \
                                                const FILTEdge edge,      \
                                                const float paramName); \
  WIN_EXPORT WIN_STRUCT(type) FILTNodeStyleGet##name(const FILTNodeRef node, const FILTEdge edge);

#define FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO(type, name) \
  WIN_EXPORT void FILTNodeStyleSet##name##Auto(const FILTNodeRef node, const FILTEdge edge);

#define FILT_NODE_LAYOUT_PROPERTY(type, name) \
  WIN_EXPORT type FILTNodeLayoutGet##name(const FILTNodeRef node);

#define FILT_NODE_LAYOUT_EDGE_PROPERTY(type, name) \
  WIN_EXPORT type FILTNodeLayoutGet##name(const FILTNodeRef node, const FILTEdge edge);

void* FILTNodeGetContext(FILTNodeRef node);
void FILTNodeSetContext(FILTNodeRef node, void* context);
FILTMeasureFunc FILTNodeGetMeasureFunc(FILTNodeRef node);
void FILTNodeSetMeasureFunc(FILTNodeRef node, FILTMeasureFunc measureFunc);
FILTBaselineFunc FILTNodeGetBaselineFunc(FILTNodeRef node);
void FILTNodeSetBaselineFunc(FILTNodeRef node, FILTBaselineFunc baselineFunc);
FILTDirtiedFunc FILTNodeGetDirtiedFunc(FILTNodeRef node);
void FILTNodeSetDirtiedFunc(FILTNodeRef node, FILTDirtiedFunc dirtiedFunc);
FILTPrintFunc FILTNodeGetPrintFunc(FILTNodeRef node);
void FILTNodeSetPrintFunc(FILTNodeRef node, FILTPrintFunc printFunc);
bool FILTNodeGetHasNewLayout(FILTNodeRef node);
void FILTNodeSetHasNewLayout(FILTNodeRef node, bool hasNewLayout);
FILTNodeType FILTNodeGetNodeType(FILTNodeRef node);
void FILTNodeSetNodeType(FILTNodeRef node, FILTNodeType nodeType);
bool FILTNodeIsDirty(FILTNodeRef node);
bool FILTNodeLayoutGetDidUseLegacyFlag(const FILTNodeRef node);

FILT_NODE_STYLE_PROPERTY(FILTDirection, Direction, direction);
FILT_NODE_STYLE_PROPERTY(FILTFlexDirection, FlexDirection, flexDirection);
FILT_NODE_STYLE_PROPERTY(FILTJustify, JustifyContent, justifyContent);
FILT_NODE_STYLE_PROPERTY(FILTAlign, AlignContent, alignContent);
FILT_NODE_STYLE_PROPERTY(FILTAlign, AlignItems, alignItems);
FILT_NODE_STYLE_PROPERTY(FILTAlign, AlignSelf, alignSelf);
FILT_NODE_STYLE_PROPERTY(FILTPositionType, PositionType, positionType);
FILT_NODE_STYLE_PROPERTY(FILTWrap, FlexWrap, flexWrap);
FILT_NODE_STYLE_PROPERTY(FILTOverflow, Overflow, overflow);
FILT_NODE_STYLE_PROPERTY(FILTDisplay, Display, display);

FILT_NODE_STYLE_PROPERTY(float, Flex, flex);
FILT_NODE_STYLE_PROPERTY(float, FlexGrow, flexGrow);
FILT_NODE_STYLE_PROPERTY(float, FlexShrink, flexShrink);
FILT_NODE_STYLE_PROPERTY_UNIT_AUTO(FILTValue, FlexBasis, flexBasis);

FILT_NODE_STYLE_EDGE_PROPERTY_UNIT(FILTValue, Position, position);
FILT_NODE_STYLE_EDGE_PROPERTY_UNIT(FILTValue, Margin, margin);
FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO(FILTValue, Margin);
FILT_NODE_STYLE_EDGE_PROPERTY_UNIT(FILTValue, Padding, padding);
FILT_NODE_STYLE_EDGE_PROPERTY(float, Border, border);

FILT_NODE_STYLE_PROPERTY_UNIT_AUTO(FILTValue, Width, width);
FILT_NODE_STYLE_PROPERTY_UNIT_AUTO(FILTValue, Height, height);
FILT_NODE_STYLE_PROPERTY_UNIT(FILTValue, MinWidth, minWidth);
FILT_NODE_STYLE_PROPERTY_UNIT(FILTValue, MinHeight, minHeight);
FILT_NODE_STYLE_PROPERTY_UNIT(FILTValue, MaxWidth, maxWidth);
FILT_NODE_STYLE_PROPERTY_UNIT(FILTValue, MaxHeight, maxHeight);

// Filates specific properties, not compatible with flexbox specification
// Aspect ratio control the size of the undefined dimension of a node.
// Aspect ratio is encoded as a floating point value width/height. e.g. A value of 2 leads to a node
// with a width twice the size of its height while a value of 0.5 gives the opposite effect.
//
// - On a node with a set width/height aspect ratio control the size of the unset dimension
// - On a node with a set flex basis aspect ratio controls the size of the node in the cross axis if
// unset
// - On a node with a measure function aspect ratio works as though the measure function measures
// the flex basis
// - On a node with flex grow/shrink aspect ratio controls the size of the node in the cross axis if
// unset
// - Aspect ratio takes min/max dimensions into account
FILT_NODE_STYLE_PROPERTY(float, AspectRatio, aspectRatio);

FILT_NODE_LAYOUT_PROPERTY(float, Left);
FILT_NODE_LAYOUT_PROPERTY(float, Top);
FILT_NODE_LAYOUT_PROPERTY(float, Right);
FILT_NODE_LAYOUT_PROPERTY(float, Bottom);
FILT_NODE_LAYOUT_PROPERTY(float, Width);
FILT_NODE_LAYOUT_PROPERTY(float, Height);
FILT_NODE_LAYOUT_PROPERTY(FILTDirection, Direction);
FILT_NODE_LAYOUT_PROPERTY(bool, HadOverflow);

// Get the computed values for these nodes after performing layout. If they were set using
// point values then the returned value will be the same as FILTNodeStyleGetXXX. However if
// they were set using a percentage value then the returned value is the computed value used
// during layout.
FILT_NODE_LAYOUT_EDGE_PROPERTY(float, Margin);
FILT_NODE_LAYOUT_EDGE_PROPERTY(float, Border);
FILT_NODE_LAYOUT_EDGE_PROPERTY(float, Padding);

WIN_EXPORT void FILTConfigSetLogger(const FILTConfigRef config, FILTLogger logger);
WIN_EXPORT void FILTLog(const FILTNodeRef node, FILTLogLevel level, const char *message, ...);
WIN_EXPORT void FILTLogWithConfig(const FILTConfigRef config, FILTLogLevel level, const char *format, ...);
WIN_EXPORT void FILTAssert(const bool condition, const char *message);
WIN_EXPORT void FILTAssertWithNode(const FILTNodeRef node, const bool condition, const char *message);
WIN_EXPORT void FILTAssertWithConfig(const FILTConfigRef config,
                                   const bool condition,
                                   const char *message);

// Set this to number of pixels in 1 point to round calculation results
// If you want to avoid rounding - set PointScaleFactor to 0
WIN_EXPORT void FILTConfigSetPointScaleFactor(const FILTConfigRef config, const float pixelsInPoint);

// Filates previously had an error where containers would take the maximum space possible instead of
// the minimum
// like they are supposed to. In practice this resulted in implicit behaviour similar to align-self:
// stretch;
// Because this was such a long-standing bug we must allow legacy users to switch back to this
// behaviour.
WIN_EXPORT void FILTConfigSetUseLegacyStretchBehaviour(const FILTConfigRef config,
                                                     const bool useLegacyStretchBehaviour);

// FILTConfig
WIN_EXPORT FILTConfigRef FILTConfigNew(void);
WIN_EXPORT void FILTConfigFree(const FILTConfigRef config);
WIN_EXPORT void FILTConfigCopy(const FILTConfigRef dest, const FILTConfigRef src);
WIN_EXPORT int32_t FILTConfigGetInstanceCount(void);

WIN_EXPORT void FILTConfigSetExperimentalFeatureEnabled(const FILTConfigRef config,
                                                      const FILTExperimentalFeature feature,
                                                      const bool enabled);
WIN_EXPORT bool FILTConfigIsExperimentalFeatureEnabled(const FILTConfigRef config,
                                                     const FILTExperimentalFeature feature);

// Using the web defaults is the prefered configuration for new projects.
// Usage of non web defaults should be considered as legacy.
WIN_EXPORT void FILTConfigSetUseWebDefaults(const FILTConfigRef config, const bool enabled);
WIN_EXPORT bool FILTConfigGetUseWebDefaults(const FILTConfigRef config);

WIN_EXPORT void FILTConfigSetNodeClonedFunc(const FILTConfigRef config,
                                          const FILTNodeClonedFunc callback);

// Export only for C#
WIN_EXPORT FILTConfigRef FILTConfigGetDefault(void);

WIN_EXPORT void FILTConfigSetContext(const FILTConfigRef config, void *context);
WIN_EXPORT void *FILTConfigGetContext(const FILTConfigRef config);

WIN_EXPORT float FILTRoundValueToPixelGrid(
    const float value,
    const float pointScaleFactor,
    const bool forceCeil,
    const bool forceFloor);

FILT_EXTERN_C_END
