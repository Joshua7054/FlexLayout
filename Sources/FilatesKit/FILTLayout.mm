/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "FILTLayout+Private.h"
#import "UIView+Filates.h"

#define FILT_PROPERTY(type, lowercased_name, capitalized_name)    \
- (type)lowercased_name                                         \
{                                                               \
  return FILTNodeStyleGet##capitalized_name(self.node);           \
}                                                               \
                                                                \
- (void)set##capitalized_name:(type)lowercased_name             \
{                                                               \
  FILTNodeStyleSet##capitalized_name(self.node, lowercased_name); \
}

#define FILT_VALUE_PROPERTY(lowercased_name, capitalized_name)                       \
- (FILTValue)lowercased_name                                                         \
{                                                                                  \
  return FILTNodeStyleGet##capitalized_name(self.node);                              \
}                                                                                  \
                                                                                   \
- (void)set##capitalized_name:(FILTValue)lowercased_name                             \
{                                                                                  \
  switch (lowercased_name.unit) {                                                  \
     case FILTUnitUndefined:                                                         \
      FILTNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case FILTUnitPoint:                                                              \
      FILTNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case FILTUnitPercent:                                                            \
      FILTNodeStyleSet##capitalized_name##Percent(self.node, lowercased_name.value); \
      break;                                                                       \
    default:                                                                       \
      NSAssert(NO, @"Not implemented");                                            \
  }                                                                                \
}

#define FILT_AUTO_VALUE_PROPERTY(lowercased_name, capitalized_name)                  \
- (FILTValue)lowercased_name                                                         \
{                                                                                  \
  return FILTNodeStyleGet##capitalized_name(self.node);                              \
}                                                                                  \
                                                                                   \
- (void)set##capitalized_name:(FILTValue)lowercased_name                             \
{                                                                                  \
  switch (lowercased_name.unit) {                                                  \
    case FILTUnitPoint:                                                              \
      FILTNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case FILTUnitPercent:                                                            \
      FILTNodeStyleSet##capitalized_name##Percent(self.node, lowercased_name.value); \
      break;                                                                       \
    case FILTUnitAuto:                                                               \
      FILTNodeStyleSet##capitalized_name##Auto(self.node);                           \
      break;                                                                       \
    default:                                                                       \
      NSAssert(NO, @"Not implemented");                                            \
  }                                                                                \
}

#define FILT_EDGE_PROPERTY_GETTER(type, lowercased_name, capitalized_name, property, edge) \
- (type)lowercased_name                                                                  \
{                                                                                        \
  return FILTNodeStyleGet##property(self.node, edge);                                      \
}

#define FILT_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge) \
- (void)set##capitalized_name:(CGFloat)lowercased_name                             \
{                                                                                  \
  FILTNodeStyleSet##property(self.node, edge, lowercased_name);                      \
}

#define FILT_EDGE_PROPERTY(lowercased_name, capitalized_name, property, edge)         \
FILT_EDGE_PROPERTY_GETTER(CGFloat, lowercased_name, capitalized_name, property, edge) \
FILT_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge)

#define FILT_VALUE_EDGE_PROPERTY_SETTER(objc_lowercased_name, objc_capitalized_name, c_name, edge) \
- (void)set##objc_capitalized_name:(FILTValue)objc_lowercased_name                                 \
{                                                                                                \
  switch (objc_lowercased_name.unit) {                                                           \
    case FILTUnitUndefined:                                                                        \
      FILTNodeStyleSet##c_name(self.node, edge, objc_lowercased_name.value);                       \
      break;                                                                                     \
    case FILTUnitPoint:                                                                            \
      FILTNodeStyleSet##c_name(self.node, edge, objc_lowercased_name.value);                       \
      break;                                                                                     \
    case FILTUnitPercent:                                                                          \
      FILTNodeStyleSet##c_name##Percent(self.node, edge, objc_lowercased_name.value);              \
      break;                                                                                     \
    default:                                                                                     \
      NSAssert(NO, @"Not implemented");                                                          \
  }                                                                                              \
}

#define FILT_VALUE_EDGE_PROPERTY(lowercased_name, capitalized_name, property, edge)   \
FILT_EDGE_PROPERTY_GETTER(FILTValue, lowercased_name, capitalized_name, property, edge) \
FILT_VALUE_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge)

#define FILT_VALUE_EDGES_PROPERTIES(lowercased_name, capitalized_name)                                                  \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Left, capitalized_name##Left, capitalized_name, FILTEdgeLeft)                   \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Top, capitalized_name##Top, capitalized_name, FILTEdgeTop)                      \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Right, capitalized_name##Right, capitalized_name, FILTEdgeRight)                \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Bottom, capitalized_name##Bottom, capitalized_name, FILTEdgeBottom)             \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Start, capitalized_name##Start, capitalized_name, FILTEdgeStart)                \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##End, capitalized_name##End, capitalized_name, FILTEdgeEnd)                      \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Horizontal, capitalized_name##Horizontal, capitalized_name, FILTEdgeHorizontal) \
FILT_VALUE_EDGE_PROPERTY(lowercased_name##Vertical, capitalized_name##Vertical, capitalized_name, FILTEdgeVertical)       \
FILT_VALUE_EDGE_PROPERTY(lowercased_name, capitalized_name, capitalized_name, FILTEdgeAll)

FILTValue FILTPointValue(CGFloat value)
{
  return (FILTValue) { .value = (float) value, .unit = (FILTUnit) FILTUnitPoint };
}

FILTValue FILTPercentValue(CGFloat value)
{
  return (FILTValue) { .value = (float) value, .unit = FILTUnitPercent };
}

static FILTConfigRef globalConfig;

@interface FILTLayout ()

@property (nonatomic, weak, readonly) UIView *view;

@end

@implementation FILTLayout

@synthesize isEnabled=_isEnabled;
@synthesize isIncludedInLayout=_isIncludedInLayout;
@synthesize node=_node;

+ (void)initialize
{
  globalConfig = FILTConfigNew();
  FILTConfigSetExperimentalFeatureEnabled(globalConfig, FILTExperimentalFeatureWebFlexBasis, true);
  FILTConfigSetPointScaleFactor(globalConfig, [UIScreen mainScreen].scale);
}

- (instancetype)initWithView:(UIView*)view
{
  if (self = [super init]) {
    _view = view;
    _node = FILTNodeNewWithConfig(globalConfig);
    FILTNodeSetContext(_node, (__bridge void *) view);
    _isEnabled = NO;
    _isIncludedInLayout = YES;

    if ([view isKindOfClass:[UILabel class]]) {
      if (FILTNodeGetBaselineFunc(_node) == NULL) {
        FILTNodeSetBaselineFunc(_node, FILTMeasureBaselineLabel);
      }
    }

    if ([view isKindOfClass:[UITextView class]]) {
      if (FILTNodeGetBaselineFunc(_node) == NULL) {
        FILTNodeSetBaselineFunc(_node, FILTMeasureBaselineTextView);
      }
    }

    if ([view isKindOfClass:[UITextField class]]) {
      if (FILTNodeGetBaselineFunc(_node) == NULL) {
        FILTNodeSetBaselineFunc(_node, FILTMeasureBaselineTextField);
      }
    }
  }

  return self;
}

- (void)dealloc
{
  FILTNodeFree(self.node);
}

- (BOOL)isDirty
{
  return FILTNodeIsDirty(self.node);
}

- (void)markDirty
{
  if (self.isDirty || !self.isLeaf) {
    return;
  }

  // Filates is not happy if we try to mark a node as "dirty" before we have set
  // the measure function. Since we already know that this is a leaf,
  // this *should* be fine. Forgive me Hack Gods.
  const FILTNodeRef node = self.node;
  if (FILTNodeGetMeasureFunc(node) == NULL) {
    FILTNodeSetMeasureFunc(node, FILTMeasureView);
  }

  FILTNodeMarkDirty(node);
}

- (NSUInteger)numberOfChildren
{
  return FILTNodeGetChildCount(self.node);
}

- (BOOL)isLeaf
{
  NSAssert([NSThread isMainThread], @"This method must be called on the main thread.");
  if (self.isEnabled) {
    for (UIView *subview in self.view.subviews) {
      FILTLayout *const filates = subview.filates;
      if (filates.isEnabled && filates.isIncludedInLayout) {
        return NO;
      }
    }
  }

  return YES;
}

#pragma mark - Style

- (FILTPositionType)position
{
  return FILTNodeStyleGetPositionType(self.node);
}

- (void)setPosition:(FILTPositionType)position
{
  FILTNodeStyleSetPositionType(self.node, position);
}

FILT_PROPERTY(FILTDirection, direction, Direction)
FILT_PROPERTY(FILTFlexDirection, flexDirection, FlexDirection)
FILT_PROPERTY(FILTJustify, justifyContent, JustifyContent)
FILT_PROPERTY(FILTAlign, alignContent, AlignContent)
FILT_PROPERTY(FILTAlign, alignItems, AlignItems)
FILT_PROPERTY(FILTAlign, alignSelf, AlignSelf)
FILT_PROPERTY(FILTWrap, flexWrap, FlexWrap)
FILT_PROPERTY(FILTOverflow, overflow, Overflow)
FILT_PROPERTY(FILTDisplay, display, Display)

FILT_PROPERTY(CGFloat, flexGrow, FlexGrow)
FILT_PROPERTY(CGFloat, flexShrink, FlexShrink)
FILT_AUTO_VALUE_PROPERTY(flexBasis, FlexBasis)

FILT_VALUE_EDGE_PROPERTY(left, Left, Position, FILTEdgeLeft)
FILT_VALUE_EDGE_PROPERTY(top, Top, Position, FILTEdgeTop)
FILT_VALUE_EDGE_PROPERTY(right, Right, Position, FILTEdgeRight)
FILT_VALUE_EDGE_PROPERTY(bottom, Bottom, Position, FILTEdgeBottom)
FILT_VALUE_EDGE_PROPERTY(start, Start, Position, FILTEdgeStart)
FILT_VALUE_EDGE_PROPERTY(end, End, Position, FILTEdgeEnd)
FILT_VALUE_EDGES_PROPERTIES(margin, Margin)
FILT_VALUE_EDGES_PROPERTIES(padding, Padding)

FILT_EDGE_PROPERTY(borderLeftWidth, BorderLeftWidth, Border, FILTEdgeLeft)
FILT_EDGE_PROPERTY(borderTopWidth, BorderTopWidth, Border, FILTEdgeTop)
FILT_EDGE_PROPERTY(borderRightWidth, BorderRightWidth, Border, FILTEdgeRight)
FILT_EDGE_PROPERTY(borderBottomWidth, BorderBottomWidth, Border, FILTEdgeBottom)
FILT_EDGE_PROPERTY(borderStartWidth, BorderStartWidth, Border, FILTEdgeStart)
FILT_EDGE_PROPERTY(borderEndWidth, BorderEndWidth, Border, FILTEdgeEnd)
FILT_EDGE_PROPERTY(borderWidth, BorderWidth, Border, FILTEdgeAll)

FILT_AUTO_VALUE_PROPERTY(width, Width)
FILT_AUTO_VALUE_PROPERTY(height, Height)
FILT_VALUE_PROPERTY(minWidth, MinWidth)
FILT_VALUE_PROPERTY(minHeight, MinHeight)
FILT_VALUE_PROPERTY(maxWidth, MaxWidth)
FILT_VALUE_PROPERTY(maxHeight, MaxHeight)
FILT_PROPERTY(CGFloat, aspectRatio, AspectRatio)

#pragma mark - Layout and Sizing

- (FILTDirection)resolvedDirection
{
  return FILTNodeLayoutGetDirection(self.node);
}

- (void)applyLayout
{
  [self calculateLayoutWithSize:self.view.bounds.size];
  FILTApplyLayoutToViewHierarchy(self.view, NO);
}

- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
{
  [self calculateLayoutWithSize:self.view.bounds.size];
  FILTApplyLayoutToViewHierarchy(self.view, preserveOrigin);
}

- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin dimensionFlexibility:(FILTDimensionFlexibility)dimensionFlexibility
{
  CGSize size = self.view.bounds.size;
  if (dimensionFlexibility & FILTDimensionFlexibilityFlexibleWidth) {
    size.width = FILTUndefined;
  }
  if (dimensionFlexibility & FILTDimensionFlexibilityFlexibleHeight) {
    size.height = FILTUndefined;
  }
  [self calculateLayoutWithSize:size];
  FILTApplyLayoutToViewHierarchy(self.view, preserveOrigin);
}


- (CGSize)intrinsicSize
{
  const CGSize constrainedSize = {
    .width = FILTUndefined,
    .height = FILTUndefined,
  };
  return [self calculateLayoutWithSize:constrainedSize];
}

- (CGSize)calculateLayoutWithSize:(CGSize)size
{
  NSAssert([NSThread isMainThread], @"Filates calculation must be done on main.");
  NSAssert(self.isEnabled, @"Filates is not enabled for this view.");

  FILTAttachNodesFromViewHierachy(self.view);

  const FILTNodeRef node = self.node;
  FILTNodeCalculateLayout(
    node,
    size.width,
    size.height,
    FILTNodeStyleGetDirection(node));

  return (CGSize) {
    .width = FILTNodeLayoutGetWidth(node),
    .height = FILTNodeLayoutGetHeight(node),
  };
}

#pragma mark - Private

static float FILTMeasureBaselineLabel(
  FILTNodeRef node,
  const float width,
  const float height) {

  UILabel* view = (__bridge UILabel*) FILTNodeGetContext(node);
  return view.font.ascender; // height + view.font.ascender for lastBaseline
}

static float FILTMeasureBaselineTextView(
  FILTNodeRef node,
  const float width,
  const float height) {

  UITextView* view = (__bridge UITextView*) FILTNodeGetContext(node);
  return view.font.ascender + view.contentInset.top + view.textContainerInset.top;
}

static float FILTMeasureBaselineTextField(
  FILTNodeRef node,
  const float width,
  const float height) {

  UITextField* view = (__bridge UITextField*) FILTNodeGetContext(node);

  switch (view.borderStyle) {
  case UITextBorderStyleNone:
    return view.font.ascender;
  case UITextBorderStyleLine:
    return view.font.ascender + 4;
  case UITextBorderStyleBezel:
  case UITextBorderStyleRoundedRect:
    return view.font.ascender + 7;
  }
}

static FILTSize FILTMeasureView(
  FILTNodeRef node,
  float width,
  FILTMeasureMode widthMode,
  float height,
  FILTMeasureMode heightMode)
{
  const CGFloat constrainedWidth = (widthMode == FILTMeasureModeUndefined) ? CGFLOAT_MAX : width;
  const CGFloat constrainedHeight = (heightMode == FILTMeasureModeUndefined) ? CGFLOAT_MAX: height;

  UIView *view = (__bridge UIView*) FILTNodeGetContext(node);
  const CGSize sizeThatFits = [view sizeThatFits:(CGSize) {
    .width = constrainedWidth,
    .height = constrainedHeight,
  }];

  return (FILTSize) {
    .width = (float) FILTSanitizeMeasurement(constrainedWidth, sizeThatFits.width, widthMode),
    .height = (float) FILTSanitizeMeasurement(constrainedHeight, sizeThatFits.height, heightMode),
  };
}

static CGFloat FILTSanitizeMeasurement(
  CGFloat constrainedSize,
  CGFloat measuredSize,
  FILTMeasureMode measureMode)
{
  CGFloat result;
  if (measureMode == FILTMeasureModeExactly) {
    result = constrainedSize;
  } else if (measureMode == FILTMeasureModeAtMost) {
    result = MIN(constrainedSize, measuredSize);
  } else {
    result = measuredSize;
  }

  return result;
}

static BOOL FILTNodeHasExactSameChildren(const FILTNodeRef node, NSArray<UIView *> *subviews)
{
  if (FILTNodeGetChildCount(node) != subviews.count) {
    return NO;
  }

  for (int i=0; i<subviews.count; i++) {
    if (FILTNodeGetChild(node, i) != subviews[i].filates.node) {
      return NO;
    }
  }

  return YES;
}

static void FILTAttachNodesFromViewHierachy(UIView *const view)
{
  FILTLayout *const filates = view.filates;
  const FILTNodeRef node = filates.node;

  // Only leaf nodes should have a measure function
  if (filates.isLeaf) {
    FILTRemoveAllChildren(node);
    FILTNodeSetMeasureFunc(node, FILTMeasureView);
  } else {
    FILTNodeSetMeasureFunc(node, NULL);

    NSMutableArray<UIView *> *subviewsToInclude = [[NSMutableArray alloc] initWithCapacity:view.subviews.count];
    for (UIView *subview in view.subviews) {
      if (subview.filates.isIncludedInLayout) {
        [subviewsToInclude addObject:subview];
      }
    }

    if (!FILTNodeHasExactSameChildren(node, subviewsToInclude)) {
      FILTRemoveAllChildren(node);
      for (int i=0; i<subviewsToInclude.count; i++) {
        FILTNodeRef child = subviewsToInclude[i].filates.node;
        FILTNodeRef parent = FILTNodeGetParent(child);
        if (parent != NULL) {
          FILTNodeRemoveChild(parent, child);
        }
        FILTNodeInsertChild(node, child, i);
      }
    }

    for (UIView *const subview in subviewsToInclude) {
      FILTAttachNodesFromViewHierachy(subview);
    }
  }
}

static void FILTRemoveAllChildren(const FILTNodeRef node)
{
  if (node == NULL) {
    return;
  }

  while (FILTNodeGetChildCount(node) > 0) {
    FILTNodeRemoveChild(node, FILTNodeGetChild(node, FILTNodeGetChildCount(node) - 1));
  }
}

static CGFloat FILTRoundPixelValue(CGFloat value)
{
  static CGFloat scale;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^(){
    scale = [UIScreen mainScreen].scale;
  });

  return roundf(value * scale) / scale;
}

static void FILTApplyLayoutToViewHierarchy(UIView *view, BOOL preserveOrigin)
{
  NSCAssert([NSThread isMainThread], @"Framesetting should only be done on the main thread.");

  const FILTLayout *filates = view.filates;

  if (!filates.isIncludedInLayout) {
     return;
  }

  FILTNodeRef node = filates.node;
  const CGPoint topLeft = {
    FILTNodeLayoutGetLeft(node),
    FILTNodeLayoutGetTop(node),
  };

  const CGPoint bottomRight = {
    topLeft.x + FILTNodeLayoutGetWidth(node),
    topLeft.y + FILTNodeLayoutGetHeight(node),
  };

  const CGPoint origin = preserveOrigin ? view.frame.origin : CGPointZero;
  view.frame = (CGRect) {
    .origin = {
      .x = FILTRoundPixelValue(topLeft.x + origin.x),
      .y = FILTRoundPixelValue(topLeft.y + origin.y),
    },
    .size = {
      .width = FILTRoundPixelValue(bottomRight.x) - FILTRoundPixelValue(topLeft.x),
      .height = FILTRoundPixelValue(bottomRight.y) - FILTRoundPixelValue(topLeft.y),
    },
  };

  if (!filates.isLeaf) {
    for (NSUInteger i=0; i<view.subviews.count; i++) {
      FILTApplyLayoutToViewHierarchy(view.subviews[i], NO);
    }
  }
}

@end
