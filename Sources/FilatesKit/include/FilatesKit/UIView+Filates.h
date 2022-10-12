/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "FILTLayout.h"
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^FILTLayoutConfigurationBlock)(FILTLayout *layout);

@interface UIView (Filates)

/**
 The FILTLayout that is attached to this view. It is lazily created.
 */
@property (nonatomic, readonly, strong) FILTLayout *filates;
/**
 Indicates whether or not Filates is enabled
 */
@property (nonatomic, readonly, assign) BOOL isFilatesEnabled;

/**
 In ObjC land, every time you access `view.filates.*` you are adding another `objc_msgSend`
 to your code. If you plan on making multiple changes to FILTLayout, it's more performant
 to use this method, which uses a single objc_msgSend call.
 */
- (void)configureLayoutWithBlock:(FILTLayoutConfigurationBlock)block
    NS_SWIFT_NAME(configureLayout(block:));

@end

NS_ASSUME_NONNULL_END
