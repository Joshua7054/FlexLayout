/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "FILTLayout.h"
#if FLEXLAYOUT_SWIFT_PACKAGE
#import <filates/Filates.h>
#else
#import "Filates.h"
#endif

@interface FILTLayout ()

@property (nonatomic, assign, readonly) FILTNodeRef node;

- (instancetype)initWithView:(UIView *)view;

@end
