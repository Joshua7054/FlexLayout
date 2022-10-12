//
//  FlexLayout+Private.swift
//  FlexLayout
//
//  Created by DION, Luc (MTL) on 2017-11-23.
//  Copyright © 2017 Mirego. All rights reserved.
//

import UIKit
import FlexLayoutFilates

extension Flex {
    func valueOrUndefined(_ value: CGFloat?) -> FILTValue {
        if let value = value {
            return FILTValue(value)
        } else {
            return FILTValueUndefined
        }
    }
    
    func valueOrAuto(_ value: CGFloat?) -> FILTValue {
        if let value = value {
            return FILTValue(value)
        } else {
            return FILTValueAuto
        }
    }
}
