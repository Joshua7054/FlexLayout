/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import CoreGraphics
import FlexLayoutFilates

postfix operator %

extension Int {
    public static postfix func % (value: Int) -> FILTValue {
        return FILTValue(value: Float(value), unit: .percent)
    }
}

extension Float {
    public static postfix func % (value: Float) -> FILTValue {
        return FILTValue(value: value, unit: .percent)
    }
}

extension CGFloat {
    public static postfix func % (value: CGFloat) -> FILTValue {
        return FILTValue(value: Float(value), unit: .percent)
    }
}

extension FILTValue: ExpressibleByIntegerLiteral, ExpressibleByFloatLiteral {
    public init(integerLiteral value: Int) {
        self = FILTValue(value: Float(value), unit: .point)
    }
    
    public init(floatLiteral value: Float) {
        self = FILTValue(value: value, unit: .point)
    }
  
    public init(_ value: Float) {
        self = FILTValue(value: value, unit: .point)
    }
  
    public init(_ value: CGFloat) {
        self = FILTValue(value: Float(value), unit: .point)
    }
}
