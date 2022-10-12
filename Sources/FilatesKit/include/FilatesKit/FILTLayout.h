/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import <UIKit/UIKit.h>

#if FLEXLAYOUT_SWIFT_PACKAGE
#import <filates/FILTEnums.h>
#import <filates/Filates.h>
#import <filates/FILTMacros.h>
#else
#import "FILTEnums.h"
#import "Filates.h"
#import "FILTMacros.h"
#endif

FILT_EXTERN_C_BEGIN

extern FILTValue FILTPointValue(CGFloat value)
    NS_SWIFT_UNAVAILABLE("Use the swift Int and FloatingPoint extensions instead");
extern FILTValue FILTPercentValue(CGFloat value)
    NS_SWIFT_UNAVAILABLE("Use the swift Int and FloatingPoint extensions instead");

FILT_EXTERN_C_END

typedef NS_OPTIONS(NSInteger, FILTDimensionFlexibility) {
  FILTDimensionFlexibilityFlexibleWidth = 1 << 0,
  FILTDimensionFlexibilityFlexibleHeight = 1 << 1,
};

@interface FILTLayout : NSObject

/**
 Make default init unavailable, as it will not initialise FILTNode which is
 required for the setters and getters of FILTLayout's properties to work properly.
*/
- (instancetype)init
    __attribute__((unavailable("you are not meant to initialise FILTLayout")));

/**
  The property that decides if we should include this view when calculating
  layout. Defaults to YES.
 */
@property (nonatomic, readwrite, assign, setter=setIncludedInLayout:) BOOL isIncludedInLayout;

/**
 The property that decides during layout/sizing whether or not styling properties should be applied.
 Defaults to NO.
 */
@property (nonatomic, readwrite, assign, setter=setEnabled:) BOOL isEnabled;

@property (nonatomic, readwrite, assign) FILTDirection direction;
@property (nonatomic, readwrite, assign) FILTFlexDirection flexDirection;
@property (nonatomic, readwrite, assign) FILTJustify justifyContent;
@property (nonatomic, readwrite, assign) FILTAlign alignContent;
@property (nonatomic, readwrite, assign) FILTAlign alignItems;
@property (nonatomic, readwrite, assign) FILTAlign alignSelf;
@property (nonatomic, readwrite, assign) FILTPositionType position;
@property (nonatomic, readwrite, assign) FILTWrap flexWrap;
@property (nonatomic, readwrite, assign) FILTOverflow overflow;
@property (nonatomic, readwrite, assign) FILTDisplay display;

@property (nonatomic, readwrite, assign) CGFloat flexGrow;
@property (nonatomic, readwrite, assign) CGFloat flexShrink;
@property (nonatomic, readwrite, assign) FILTValue flexBasis;

@property (nonatomic, readwrite, assign) FILTValue left;
@property (nonatomic, readwrite, assign) FILTValue top;
@property (nonatomic, readwrite, assign) FILTValue right;
@property (nonatomic, readwrite, assign) FILTValue bottom;
@property (nonatomic, readwrite, assign) FILTValue start;
@property (nonatomic, readwrite, assign) FILTValue end;

@property (nonatomic, readwrite, assign) FILTValue marginLeft;
@property (nonatomic, readwrite, assign) FILTValue marginTop;
@property (nonatomic, readwrite, assign) FILTValue marginRight;
@property (nonatomic, readwrite, assign) FILTValue marginBottom;
@property (nonatomic, readwrite, assign) FILTValue marginStart;
@property (nonatomic, readwrite, assign) FILTValue marginEnd;
@property (nonatomic, readwrite, assign) FILTValue marginHorizontal;
@property (nonatomic, readwrite, assign) FILTValue marginVertical;
@property (nonatomic, readwrite, assign) FILTValue margin;

@property (nonatomic, readwrite, assign) FILTValue paddingLeft;
@property (nonatomic, readwrite, assign) FILTValue paddingTop;
@property (nonatomic, readwrite, assign) FILTValue paddingRight;
@property (nonatomic, readwrite, assign) FILTValue paddingBottom;
@property (nonatomic, readwrite, assign) FILTValue paddingStart;
@property (nonatomic, readwrite, assign) FILTValue paddingEnd;
@property (nonatomic, readwrite, assign) FILTValue paddingHorizontal;
@property (nonatomic, readwrite, assign) FILTValue paddingVertical;
@property (nonatomic, readwrite, assign) FILTValue padding;

@property (nonatomic, readwrite, assign) CGFloat borderLeftWidth;
@property (nonatomic, readwrite, assign) CGFloat borderTopWidth;
@property (nonatomic, readwrite, assign) CGFloat borderRightWidth;
@property (nonatomic, readwrite, assign) CGFloat borderBottomWidth;
@property (nonatomic, readwrite, assign) CGFloat borderStartWidth;
@property (nonatomic, readwrite, assign) CGFloat borderEndWidth;
@property (nonatomic, readwrite, assign) CGFloat borderWidth;

@property (nonatomic, readwrite, assign) FILTValue width;
@property (nonatomic, readwrite, assign) FILTValue height;
@property (nonatomic, readwrite, assign) FILTValue minWidth;
@property (nonatomic, readwrite, assign) FILTValue minHeight;
@property (nonatomic, readwrite, assign) FILTValue maxWidth;
@property (nonatomic, readwrite, assign) FILTValue maxHeight;

// Filates specific properties, not compatible with flexbox specification
@property (nonatomic, readwrite, assign) CGFloat aspectRatio;

/**
 Get the resolved direction of this node. This won't be FILTDirectionInherit
 */
@property (nonatomic, readonly, assign) FILTDirection resolvedDirection;

/**
 Perform a layout calculation and update the frames of the views in the hierarchy with the results.
 If the origin is not preserved, the root view's layout results will applied from {0,0}.
 */
- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
    NS_SWIFT_NAME(applyLayout(preservingOrigin:));

/**
 Perform a layout calculation and update the frames of the views in the hierarchy with the results.
 If the origin is not preserved, the root view's layout results will applied from {0,0}.
 */
- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
               dimensionFlexibility:(FILTDimensionFlexibility)dimensionFlexibility
    NS_SWIFT_NAME(applyLayout(preservingOrigin:dimensionFlexibility:));

/**
 Returns the size of the view if no constraints were given. This could equivalent to calling [self
 sizeThatFits:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX)];
 */
@property (nonatomic, readonly, assign) CGSize intrinsicSize;

/**
  Returns the size of the view based on provided constraints. Pass NaN for an unconstrained dimension.
 */
- (CGSize)calculateLayoutWithSize:(CGSize)size
    NS_SWIFT_NAME(calculateLayout(with:));

/**
 Returns the number of children that are using Flexbox.
 */
@property (nonatomic, readonly, assign) NSUInteger numberOfChildren;

/**
 Return a BOOL indiciating whether or not we this node contains any subviews that are included in
 Filates's layout.
 */
@property (nonatomic, readonly, assign) BOOL isLeaf;

/**
 Return's a BOOL indicating if a view is dirty. When a node is dirty
 it usually indicates that it will be remeasured on the next layout pass.
 */
@property (nonatomic, readonly, assign) BOOL isDirty;

/**
 Mark that a view's layout needs to be recalculated. Only works for leaf views.
 */
- (void)markDirty;

@end
