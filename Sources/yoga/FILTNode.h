/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <stdio.h>
#include "FILTLayout.h"
#include "FILTStyle.h"
#include "Filates-internal.h"

struct FILTNode {
 private:
  void* context_;
  FILTPrintFunc print_;
  bool hasNewLayout_;
  FILTNodeType nodeType_;
  FILTMeasureFunc measure_;
  FILTBaselineFunc baseline_;
  FILTDirtiedFunc dirtied_;
  FILTStyle style_;
  FILTLayout layout_;
  uint32_t lineIndex_;
  FILTNodeRef parent_;
  FILTVector children_;
  FILTNodeRef nextChild_;
  FILTConfigRef config_;
  bool isDirty_;
  std::array<FILTValue, 2> resolvedDimensions_;

  float relativePosition(const FILTFlexDirection axis, const float axisSize);

 public:
  FILTNode();
  ~FILTNode();
  explicit FILTNode(const FILTConfigRef newConfig);
  FILTNode(const FILTNode& node);
  FILTNode& operator=(const FILTNode& node);
  FILTNode(
      void* context,
      FILTPrintFunc print,
      bool hasNewLayout,
      FILTNodeType nodeType,
      FILTMeasureFunc measure,
      FILTBaselineFunc baseline,
      FILTDirtiedFunc dirtied,
      FILTStyle style,
      FILTLayout layout,
      uint32_t lineIndex,
      FILTNodeRef parent,
      FILTVector children,
      FILTNodeRef nextChild,
      FILTConfigRef config,
      bool isDirty,
      std::array<FILTValue, 2> resolvedDimensions);

  // Getters
  void* getContext() const;
  FILTPrintFunc getPrintFunc() const;
  bool getHasNewLayout() const;
  FILTNodeType getNodeType() const;
  FILTMeasureFunc getMeasure() const;
  FILTBaselineFunc getBaseline() const;
  FILTDirtiedFunc getDirtied() const;
  // For Performance reasons passing as reference.
  FILTStyle& getStyle();
  // For Performance reasons passing as reference.
  FILTLayout& getLayout();
  uint32_t getLineIndex() const;
  FILTNodeRef getParent() const;
  FILTVector getChildren() const;
  uint32_t getChildrenCount() const;
  FILTNodeRef getChild(uint32_t index) const;
  FILTNodeRef getNextChild() const;
  FILTConfigRef getConfig() const;
  bool isDirty() const;
  std::array<FILTValue, 2> getResolvedDimensions() const;
  FILTValue getResolvedDimension(int index);

  // Methods related to positions, margin, padding and border
  float getLeadingPosition(const FILTFlexDirection axis, const float axisSize);
  bool isLeadingPositionDefined(const FILTFlexDirection axis);
  bool isTrailingPosDefined(const FILTFlexDirection axis);
  float getTrailingPosition(const FILTFlexDirection axis, const float axisSize);
  float getLeadingMargin(const FILTFlexDirection axis, const float widthSize);
  float getTrailingMargin(const FILTFlexDirection axis, const float widthSize);
  float getLeadingBorder(const FILTFlexDirection flexDirection);
  float getTrailingBorder(const FILTFlexDirection flexDirection);
  float getLeadingPadding(const FILTFlexDirection axis, const float widthSize);
  float getTrailingPadding(const FILTFlexDirection axis, const float widthSize);
  float getLeadingPaddingAndBorder(
      const FILTFlexDirection axis,
      const float widthSize);
  float getTrailingPaddingAndBorder(
      const FILTFlexDirection axis,
      const float widthSize);
  float getMarginForAxis(const FILTFlexDirection axis, const float widthSize);
  // Setters

  void setContext(void* context);
  void setPrintFunc(FILTPrintFunc printFunc);
  void setHasNewLayout(bool hasNewLayout);
  void setNodeType(FILTNodeType nodeTye);
  void setMeasureFunc(FILTMeasureFunc measureFunc);
  void setBaseLineFunc(FILTBaselineFunc baseLineFunc);
  void setDirtiedFunc(FILTDirtiedFunc dirtiedFunc);
  void setStyle(FILTStyle style);
  void setStyleFlexDirection(FILTFlexDirection direction);
  void setStyleAlignContent(FILTAlign alignContent);
  void setLayout(FILTLayout layout);
  void setLineIndex(uint32_t lineIndex);
  void setParent(FILTNodeRef parent);
  void setChildren(FILTVector children);
  void setNextChild(FILTNodeRef nextChild);
  void setConfig(FILTConfigRef config);
  void setDirty(bool isDirty);
  void setLayoutLastParentDirection(FILTDirection direction);
  void setLayoutComputedFlexBasis(float computedFlexBasis);
  void setLayoutComputedFlexBasisGeneration(
      uint32_t computedFlexBasisGeneration);
  void setLayoutMeasuredDimension(float measuredDimension, int index);
  void setLayoutHadOverflow(bool hadOverflow);
  void setLayoutDimension(float dimension, int index);
  void setLayoutDirection(FILTDirection direction);
  void setLayoutMargin(float margin, int index);
  void setLayoutBorder(float border, int index);
  void setLayoutPadding(float padding, int index);
  void setLayoutPosition(float position, int index);
  void setPosition(
      const FILTDirection direction,
      const float mainSize,
      const float crossSize,
      const float parentWidth);
  void setAndPropogateUseLegacyFlag(bool useLegacyFlag);
  void setLayoutDoesLegacyFlagAffectsLayout(bool doesLegacyFlagAffectsLayout);
  void setLayoutDidUseLegacyFlag(bool didUseLegacyFlag);
  void markDirtyAndPropogateDownwards();

  // Other methods
  FILTValue marginLeadingValue(const FILTFlexDirection axis) const;
  FILTValue marginTrailingValue(const FILTFlexDirection axis) const;
  FILTValue resolveFlexBasisPtr() const;
  void resolveDimension();
  FILTDirection resolveDirection(const FILTDirection parentDirection);
  void clearChildren();
  /// Replaces the occurrences of oldChild with newChild
  void replaceChild(FILTNodeRef oldChild, FILTNodeRef newChild);
  void replaceChild(FILTNodeRef child, uint32_t index);
  void insertChild(FILTNodeRef child, uint32_t index);
  /// Removes the first occurrence of child
  bool removeChild(FILTNodeRef child);
  void removeChild(uint32_t index);

  void cloneChildrenIfNeeded();
  void markDirtyAndPropogate();
  float resolveFlexGrow();
  float resolveFlexShrink();
  bool isNodeFlexible();
  bool didUseLegacyFlag();
  bool isLayoutTreeEqualToNode(const FILTNode& node) const;
};
