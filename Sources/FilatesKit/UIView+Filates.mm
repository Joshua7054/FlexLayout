/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "UIView+Filates.h"
#import "FILTLayout+Private.h"
#import <objc/runtime.h>

static const void *kFILTFilatesAssociatedKey = &kFILTFilatesAssociatedKey;

@implementation UIView (FilatesKit)

- (FILTLayout *)filates
{
  FILTLayout *filates = objc_getAssociatedObject(self, kFILTFilatesAssociatedKey);
  if (!filates) {
    filates = [[FILTLayout alloc] initWithView:self];
    objc_setAssociatedObject(self, kFILTFilatesAssociatedKey, filates, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  }

  return filates;
}

- (BOOL)isFilatesEnabled
{
  return objc_getAssociatedObject(self, kFILTFilatesAssociatedKey) != nil;
}

- (void)configureLayoutWithBlock:(FILTLayoutConfigurationBlock)block
{
  if (block != nil) {
    block(self.filates);
  }
}

@end
