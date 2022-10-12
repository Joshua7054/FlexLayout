/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "FILTNode.h"
#include <iostream>
#include "Utils.h"

void* FILTNode::getContext() const {
  return context_;
}

FILTPrintFunc FILTNode::getPrintFunc() const {
  return print_;
}

bool FILTNode::getHasNewLayout() const {
  return hasNewLayout_;
}

FILTNodeType FILTNode::getNodeType() const {
  return nodeType_;
}

FILTMeasureFunc FILTNode::getMeasure() const {
  return measure_;
}

FILTBaselineFunc FILTNode::getBaseline() const {
  return baseline_;
}

FILTDirtiedFunc FILTNode::getDirtied() const {
  return dirtied_;
}

FILTStyle& FILTNode::getStyle() {
  return style_;
}

FILTLayout& FILTNode::getLayout() {
  return layout_;
}

uint32_t FILTNode::getLineIndex() const {
  return lineIndex_;
}

FILTNodeRef FILTNode::getParent() const {
  return parent_;
}

FILTVector FILTNode::getChildren() const {
  return children_;
}

uint32_t FILTNode::getChildrenCount() const {
  return static_cast<uint32_t>(children_.size());
}

FILTNodeRef FILTNode::getChild(uint32_t index) const {
  return children_.at(index);
}

FILTNodeRef FILTNode::getNextChild() const {
  return nextChild_;
}

FILTConfigRef FILTNode::getConfig() const {
  return config_;
}

bool FILTNode::isDirty() const {
  return isDirty_;
}

FILTValue FILTNode::getResolvedDimension(int index) {
  return resolvedDimensions_[index];
}

std::array<FILTValue, 2> FILTNode::getResolvedDimensions() const {
  return resolvedDimensions_;
}

float FILTNode::getLeadingPosition(
    const FILTFlexDirection axis,
    const float axisSize) {
  if (FILTFlexDirectionIsRow(axis)) {
    const FILTValue* leadingPosition =
        FILTComputedEdgeValue(style_.position, FILTEdgeStart, &FILTValueUndefined);
    if (leadingPosition->unit != FILTUnitUndefined) {
      return FILTResolveValue(*leadingPosition, axisSize);
    }
  }

  const FILTValue* leadingPosition =
      FILTComputedEdgeValue(style_.position, leading[axis], &FILTValueUndefined);

  return leadingPosition->unit == FILTUnitUndefined
      ? 0.0f
      : FILTResolveValue(*leadingPosition, axisSize);
}

float FILTNode::getTrailingPosition(
    const FILTFlexDirection axis,
    const float axisSize) {
  if (FILTFlexDirectionIsRow(axis)) {
    const FILTValue* trailingPosition =
        FILTComputedEdgeValue(style_.position, FILTEdgeEnd, &FILTValueUndefined);
    if (trailingPosition->unit != FILTUnitUndefined) {
      return FILTResolveValue(*trailingPosition, axisSize);
    }
  }

  const FILTValue* trailingPosition =
      FILTComputedEdgeValue(style_.position, trailing[axis], &FILTValueUndefined);

  return trailingPosition->unit == FILTUnitUndefined
      ? 0.0f
      : FILTResolveValue(*trailingPosition, axisSize);
}

bool FILTNode::isLeadingPositionDefined(const FILTFlexDirection axis) {
  return (FILTFlexDirectionIsRow(axis) &&
          FILTComputedEdgeValue(style_.position, FILTEdgeStart, &FILTValueUndefined)
                  ->unit != FILTUnitUndefined) ||
      FILTComputedEdgeValue(style_.position, leading[axis], &FILTValueUndefined)
          ->unit != FILTUnitUndefined;
}

bool FILTNode::isTrailingPosDefined(const FILTFlexDirection axis) {
  return (FILTFlexDirectionIsRow(axis) &&
          FILTComputedEdgeValue(style_.position, FILTEdgeEnd, &FILTValueUndefined)
                  ->unit != FILTUnitUndefined) ||
      FILTComputedEdgeValue(style_.position, trailing[axis], &FILTValueUndefined)
          ->unit != FILTUnitUndefined;
}

float FILTNode::getLeadingMargin(
    const FILTFlexDirection axis,
    const float widthSize) {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.margin[FILTEdgeStart].unit != FILTUnitUndefined) {
    return FILTResolveValueMargin(style_.margin[FILTEdgeStart], widthSize);
  }

  return FILTResolveValueMargin(
      *FILTComputedEdgeValue(style_.margin, leading[axis], &FILTValueZero),
      widthSize);
}

float FILTNode::getTrailingMargin(
    const FILTFlexDirection axis,
    const float widthSize) {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.margin[FILTEdgeEnd].unit != FILTUnitUndefined) {
    return FILTResolveValueMargin(style_.margin[FILTEdgeEnd], widthSize);
  }

  return FILTResolveValueMargin(
      *FILTComputedEdgeValue(style_.margin, trailing[axis], &FILTValueZero),
      widthSize);
}

float FILTNode::getMarginForAxis(
    const FILTFlexDirection axis,
    const float widthSize) {
  return getLeadingMargin(axis, widthSize) + getTrailingMargin(axis, widthSize);
}

// Setters

void FILTNode::setContext(void* context) {
  context_ = context;
}

void FILTNode::setPrintFunc(FILTPrintFunc printFunc) {
  print_ = printFunc;
}

void FILTNode::setHasNewLayout(bool hasNewLayout) {
  hasNewLayout_ = hasNewLayout;
}

void FILTNode::setNodeType(FILTNodeType nodeType) {
  nodeType_ = nodeType;
}

void FILTNode::setStyleFlexDirection(FILTFlexDirection direction) {
  style_.flexDirection = direction;
}

void FILTNode::setStyleAlignContent(FILTAlign alignContent) {
  style_.alignContent = alignContent;
}

void FILTNode::setMeasureFunc(FILTMeasureFunc measureFunc) {
  if (measureFunc == nullptr) {
    measure_ = nullptr;
    // TODO: t18095186 Move nodeType to opt-in function and mark appropriate
    // places in Litho
    nodeType_ = FILTNodeTypeDefault;
  } else {
    FILTAssertWithNode(
        this,
        children_.size() == 0,
        "Cannot set measure function: Nodes with measure functions cannot have children.");
    measure_ = measureFunc;
    // TODO: t18095186 Move nodeType to opt-in function and mark appropriate
    // places in Litho
    setNodeType(FILTNodeTypeText);
  }

  measure_ = measureFunc;
}

void FILTNode::setBaseLineFunc(FILTBaselineFunc baseLineFunc) {
  baseline_ = baseLineFunc;
}

void FILTNode::setDirtiedFunc(FILTDirtiedFunc dirtiedFunc) {
  dirtied_ = dirtiedFunc;
}

void FILTNode::setStyle(FILTStyle style) {
  style_ = style;
}

void FILTNode::setLayout(FILTLayout layout) {
  layout_ = layout;
}

void FILTNode::setLineIndex(uint32_t lineIndex) {
  lineIndex_ = lineIndex;
}

void FILTNode::setParent(FILTNodeRef parent) {
  parent_ = parent;
}

void FILTNode::setChildren(FILTVector children) {
  children_ = children;
}

void FILTNode::setNextChild(FILTNodeRef nextChild) {
  nextChild_ = nextChild;
}

void FILTNode::replaceChild(FILTNodeRef child, uint32_t index) {
  children_[index] = child;
}

void FILTNode::replaceChild(FILTNodeRef oldChild, FILTNodeRef newChild) {
  std::replace(children_.begin(), children_.end(), oldChild, newChild);
}

void FILTNode::insertChild(FILTNodeRef child, uint32_t index) {
  children_.insert(children_.begin() + index, child);
}

void FILTNode::setConfig(FILTConfigRef config) {
  config_ = config;
}

void FILTNode::setDirty(bool isDirty) {
  if (isDirty == isDirty_) {
    return;
  }
  isDirty_ = isDirty;
  if (isDirty && dirtied_) {
    dirtied_(this);
  }
}

bool FILTNode::removeChild(FILTNodeRef child) {
  std::vector<FILTNodeRef>::iterator p =
      std::find(children_.begin(), children_.end(), child);
  if (p != children_.end()) {
    children_.erase(p);
    return true;
  }
  return false;
}

void FILTNode::removeChild(uint32_t index) {
  children_.erase(children_.begin() + index);
}

void FILTNode::setLayoutDirection(FILTDirection direction) {
  layout_.direction = direction;
}

void FILTNode::setLayoutMargin(float margin, int index) {
  layout_.margin[index] = margin;
}

void FILTNode::setLayoutBorder(float border, int index) {
  layout_.border[index] = border;
}

void FILTNode::setLayoutPadding(float padding, int index) {
  layout_.padding[index] = padding;
}

void FILTNode::setLayoutLastParentDirection(FILTDirection direction) {
  layout_.lastParentDirection = direction;
}

void FILTNode::setLayoutComputedFlexBasis(float computedFlexBasis) {
  layout_.computedFlexBasis = computedFlexBasis;
}

void FILTNode::setLayoutPosition(float position, int index) {
  layout_.position[index] = position;
}

void FILTNode::setLayoutComputedFlexBasisGeneration(
    uint32_t computedFlexBasisGeneration) {
  layout_.computedFlexBasisGeneration = computedFlexBasisGeneration;
}

void FILTNode::setLayoutMeasuredDimension(float measuredDimension, int index) {
  layout_.measuredDimensions[index] = measuredDimension;
}

void FILTNode::setLayoutHadOverflow(bool hadOverflow) {
  layout_.hadOverflow = hadOverflow;
}

void FILTNode::setLayoutDimension(float dimension, int index) {
  layout_.dimensions[index] = dimension;
}

// If both left and right are defined, then use left. Otherwise return
// +left or -right depending on which is defined.
float FILTNode::relativePosition(
    const FILTFlexDirection axis,
    const float axisSize) {
  return isLeadingPositionDefined(axis) ? getLeadingPosition(axis, axisSize)
                                        : -getTrailingPosition(axis, axisSize);
}

void FILTNode::setPosition(
    const FILTDirection direction,
    const float mainSize,
    const float crossSize,
    const float parentWidth) {
  /* Root nodes should be always layouted as LTR, so we don't return negative
   * values. */
  const FILTDirection directionRespectingRoot =
      parent_ != nullptr ? direction : FILTDirectionLTR;
  const FILTFlexDirection mainAxis =
      FILTResolveFlexDirection(style_.flexDirection, directionRespectingRoot);
  const FILTFlexDirection crossAxis =
      FILTFlexDirectionCross(mainAxis, directionRespectingRoot);

  const float relativePositionMain = relativePosition(mainAxis, mainSize);
  const float relativePositionCross = relativePosition(crossAxis, crossSize);

  setLayoutPosition(
      getLeadingMargin(mainAxis, parentWidth) + relativePositionMain,
      leading[mainAxis]);
  setLayoutPosition(
      getTrailingMargin(mainAxis, parentWidth) + relativePositionMain,
      trailing[mainAxis]);
  setLayoutPosition(
      getLeadingMargin(crossAxis, parentWidth) + relativePositionCross,
      leading[crossAxis]);
  setLayoutPosition(
      getTrailingMargin(crossAxis, parentWidth) + relativePositionCross,
      trailing[crossAxis]);
}

FILTNode::FILTNode()
    : context_(nullptr),
      print_(nullptr),
      hasNewLayout_(true),
      nodeType_(FILTNodeTypeDefault),
      measure_(nullptr),
      baseline_(nullptr),
      dirtied_(nullptr),
      style_(FILTStyle()),
      layout_(FILTLayout()),
      lineIndex_(0),
      parent_(nullptr),
      children_(FILTVector()),
      nextChild_(nullptr),
      config_(nullptr),
      isDirty_(false),
      resolvedDimensions_({{FILTValueUndefined, FILTValueUndefined}}) {}

FILTNode::FILTNode(const FILTNode& node)
    : context_(node.context_),
      print_(node.print_),
      hasNewLayout_(node.hasNewLayout_),
      nodeType_(node.nodeType_),
      measure_(node.measure_),
      baseline_(node.baseline_),
      dirtied_(node.dirtied_),
      style_(node.style_),
      layout_(node.layout_),
      lineIndex_(node.lineIndex_),
      parent_(node.parent_),
      children_(node.children_),
      nextChild_(node.nextChild_),
      config_(node.config_),
      isDirty_(node.isDirty_),
      resolvedDimensions_(node.resolvedDimensions_) {}

FILTNode::FILTNode(const FILTConfigRef newConfig) : FILTNode() {
  config_ = newConfig;
}

FILTNode::FILTNode(
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
    std::array<FILTValue, 2> resolvedDimensions)
    : context_(context),
      print_(print),
      hasNewLayout_(hasNewLayout),
      nodeType_(nodeType),
      measure_(measure),
      baseline_(baseline),
      dirtied_(dirtied),
      style_(style),
      layout_(layout),
      lineIndex_(lineIndex),
      parent_(parent),
      children_(children),
      nextChild_(nextChild),
      config_(config),
      isDirty_(isDirty),
      resolvedDimensions_(resolvedDimensions) {}

FILTNode& FILTNode::operator=(const FILTNode& node) {
  if (&node == this) {
    return *this;
  }

  for (auto child : children_) {
    delete child;
  }

  context_ = node.getContext();
  print_ = node.getPrintFunc();
  hasNewLayout_ = node.getHasNewLayout();
  nodeType_ = node.getNodeType();
  measure_ = node.getMeasure();
  baseline_ = node.getBaseline();
  dirtied_ = node.getDirtied();
  style_ = node.style_;
  layout_ = node.layout_;
  lineIndex_ = node.getLineIndex();
  parent_ = node.getParent();
  children_ = node.getChildren();
  nextChild_ = node.getNextChild();
  config_ = node.getConfig();
  isDirty_ = node.isDirty();
  resolvedDimensions_ = node.getResolvedDimensions();

  return *this;
}

FILTValue FILTNode::marginLeadingValue(const FILTFlexDirection axis) const {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.margin[FILTEdgeStart].unit != FILTUnitUndefined) {
    return style_.margin[FILTEdgeStart];
  } else {
    return style_.margin[leading[axis]];
  }
}

FILTValue FILTNode::marginTrailingValue(const FILTFlexDirection axis) const {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.margin[FILTEdgeEnd].unit != FILTUnitUndefined) {
    return style_.margin[FILTEdgeEnd];
  } else {
    return style_.margin[trailing[axis]];
  }
}

FILTValue FILTNode::resolveFlexBasisPtr() const {
  FILTValue flexBasis = style_.flexBasis;
  if (flexBasis.unit != FILTUnitAuto && flexBasis.unit != FILTUnitUndefined) {
    return flexBasis;
  }
  if (!FILTFloatIsUndefined(style_.flex) && style_.flex > 0.0f) {
    return config_->useWebDefaults ? FILTValueAuto : FILTValueZero;
  }
  return FILTValueAuto;
}

void FILTNode::resolveDimension() {
  for (uint32_t dim = FILTDimensionWidth; dim < FILTDimensionCount; dim++) {
    if (getStyle().maxDimensions[dim].unit != FILTUnitUndefined &&
        FILTValueEqual(
            getStyle().maxDimensions[dim], style_.minDimensions[dim])) {
      resolvedDimensions_[dim] = style_.maxDimensions[dim];
    } else {
      resolvedDimensions_[dim] = style_.dimensions[dim];
    }
  }
}

FILTDirection FILTNode::resolveDirection(const FILTDirection parentDirection) {
  if (style_.direction == FILTDirectionInherit) {
    return parentDirection > FILTDirectionInherit ? parentDirection
                                                : FILTDirectionLTR;
  } else {
    return style_.direction;
  }
}

void FILTNode::clearChildren() {
  children_.clear();
  children_.shrink_to_fit();
}

FILTNode::~FILTNode() {
  // All the member variables are deallocated externally, so no need to
  // deallocate here
}

// Other Methods

void FILTNode::cloneChildrenIfNeeded() {
  // FILTNodeRemoveChild in filates.cpp has a forked variant of this algorithm
  // optimized for deletions.

  const uint32_t childCount = static_cast<uint32_t>(children_.size());
  if (childCount == 0) {
    // This is an empty set. Nothing to clone.
    return;
  }

  const FILTNodeRef firstChild = children_.front();
  if (firstChild->getParent() == this) {
    // If the first child has this node as its parent, we assume that it is
    // already unique. We can do this because if we have it has a child, that
    // means that its parent was at some point cloned which made that subtree
    // immutable. We also assume that all its sibling are cloned as well.
    return;
  }

  const FILTNodeClonedFunc cloneNodeCallback = config_->cloneNodeCallback;
  for (uint32_t i = 0; i < childCount; ++i) {
    const FILTNodeRef oldChild = children_[i];
    const FILTNodeRef newChild = FILTNodeClone(oldChild);
    replaceChild(newChild, i);
    newChild->setParent(this);
    if (cloneNodeCallback) {
      cloneNodeCallback(oldChild, newChild, this, i);
    }
  }
}

void FILTNode::markDirtyAndPropogate() {
  if (!isDirty_) {
    setDirty(true);
    setLayoutComputedFlexBasis(FILTUndefined);
    if (parent_) {
      parent_->markDirtyAndPropogate();
    }
  }
}

void FILTNode::markDirtyAndPropogateDownwards() {
  isDirty_ = true;
  for_each(children_.begin(), children_.end(), [](FILTNodeRef childNode) {
    childNode->markDirtyAndPropogateDownwards();
  });
}

float FILTNode::resolveFlexGrow() {
  // Root nodes flexGrow should always be 0
  if (parent_ == nullptr) {
    return 0.0;
  }
  if (!FILTFloatIsUndefined(style_.flexGrow)) {
    return style_.flexGrow;
  }
  if (!FILTFloatIsUndefined(style_.flex) && style_.flex > 0.0f) {
    return style_.flex;
  }
  return kDefaultFlexGrow;
}

float FILTNode::resolveFlexShrink() {
  if (parent_ == nullptr) {
    return 0.0;
  }
  if (!FILTFloatIsUndefined(style_.flexShrink)) {
    return style_.flexShrink;
  }
  if (!config_->useWebDefaults && !FILTFloatIsUndefined(style_.flex) &&
      style_.flex < 0.0f) {
    return -style_.flex;
  }
  return config_->useWebDefaults ? kWebDefaultFlexShrink : kDefaultFlexShrink;
}

bool FILTNode::isNodeFlexible() {
  return (
      (style_.positionType == FILTPositionTypeRelative) &&
      (resolveFlexGrow() != 0 || resolveFlexShrink() != 0));
}

float FILTNode::getLeadingBorder(const FILTFlexDirection axis) {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.border[FILTEdgeStart].unit != FILTUnitUndefined &&
      style_.border[FILTEdgeStart].value >= 0.0f) {
    return style_.border[FILTEdgeStart].value;
  }

  return fmaxf(
      FILTComputedEdgeValue(style_.border, leading[axis], &FILTValueZero)->value,
      0.0f);
}

float FILTNode::getTrailingBorder(const FILTFlexDirection flexDirection) {
  if (FILTFlexDirectionIsRow(flexDirection) &&
      style_.border[FILTEdgeEnd].unit != FILTUnitUndefined &&
      style_.border[FILTEdgeEnd].value >= 0.0f) {
    return style_.border[FILTEdgeEnd].value;
  }

  return fmaxf(
      FILTComputedEdgeValue(style_.border, trailing[flexDirection], &FILTValueZero)
          ->value,
      0.0f);
}

float FILTNode::getLeadingPadding(
    const FILTFlexDirection axis,
    const float widthSize) {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.padding[FILTEdgeStart].unit != FILTUnitUndefined &&
      FILTResolveValue(style_.padding[FILTEdgeStart], widthSize) >= 0.0f) {
    return FILTResolveValue(style_.padding[FILTEdgeStart], widthSize);
  }
  return fmaxf(
      FILTResolveValue(
          *FILTComputedEdgeValue(style_.padding, leading[axis], &FILTValueZero),
          widthSize),
      0.0f);
}

float FILTNode::getTrailingPadding(
    const FILTFlexDirection axis,
    const float widthSize) {
  if (FILTFlexDirectionIsRow(axis) &&
      style_.padding[FILTEdgeEnd].unit != FILTUnitUndefined &&
      FILTResolveValue(style_.padding[FILTEdgeEnd], widthSize) >= 0.0f) {
    return FILTResolveValue(style_.padding[FILTEdgeEnd], widthSize);
  }
  return fmaxf(
      FILTResolveValue(
          *FILTComputedEdgeValue(style_.padding, trailing[axis], &FILTValueZero),
          widthSize),
      0.0f);
}

float FILTNode::getLeadingPaddingAndBorder(
    const FILTFlexDirection axis,
    const float widthSize) {
  return getLeadingPadding(axis, widthSize) + getLeadingBorder(axis);
}

float FILTNode::getTrailingPaddingAndBorder(
    const FILTFlexDirection axis,
    const float widthSize) {
  return getTrailingPadding(axis, widthSize) + getTrailingBorder(axis);
}

bool FILTNode::didUseLegacyFlag() {
  bool didUseLegacyFlag = layout_.didUseLegacyFlag;
  if (didUseLegacyFlag) {
    return true;
  }
  for (const auto& child : children_) {
    if (child->layout_.didUseLegacyFlag) {
      didUseLegacyFlag = true;
      break;
    }
  }
  return didUseLegacyFlag;
}

void FILTNode::setAndPropogateUseLegacyFlag(bool useLegacyFlag) {
  config_->useLegacyStretchBehaviour = useLegacyFlag;
  for_each(children_.begin(), children_.end(), [=](FILTNodeRef childNode) {
    childNode->getConfig()->useLegacyStretchBehaviour = useLegacyFlag;
  });
}

void FILTNode::setLayoutDoesLegacyFlagAffectsLayout(
    bool doesLegacyFlagAffectsLayout) {
  layout_.doesLegacyStretchFlagAffectsLayout = doesLegacyFlagAffectsLayout;
}

void FILTNode::setLayoutDidUseLegacyFlag(bool didUseLegacyFlag) {
  layout_.didUseLegacyFlag = didUseLegacyFlag;
}

bool FILTNode::isLayoutTreeEqualToNode(const FILTNode& node) const {
  if (children_.size() != node.children_.size()) {
    return false;
  }
  if (layout_ != node.layout_) {
    return false;
  }
  if (children_.size() == 0) {
    return true;
  }

  bool isLayoutTreeEqual = true;
  FILTNodeRef otherNodeChildren = nullptr;
  for (std::vector<FILTNodeRef>::size_type i = 0; i < children_.size(); ++i) {
    otherNodeChildren = node.children_[i];
    isLayoutTreeEqual =
        children_[i]->isLayoutTreeEqualToNode(*otherNodeChildren);
    if (!isLayoutTreeEqual) {
      return false;
    }
  }
  return isLayoutTreeEqual;
}
