// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Created by Luc Dion on 2017-07-17.

import UIKit

import FlexLayoutFilates

extension FILTFlexDirection {
    static let column = FILTFlexDirectionColumn
    static let columnReverse = FILTFlexDirectionColumnReverse
    static let row = FILTFlexDirectionRow
    static let rowReverse = FILTFlexDirectionRowReverse
}

extension FILTJustify {
    static let flexStart = FILTJustifyFlexStart
    static let center = FILTJustifyCenter
    static let flexEnd = FILTJustifyFlexEnd
    static let spaceBetween = FILTJustifySpaceBetween
    static let spaceAround = FILTJustifySpaceAround
    static let spaceEvenly = FILTJustifySpaceEvenly
}

extension FILTAlign {
    static let auto = FILTAlignAuto
    static let baseline = FILTAlignBaseline
    static let stretch = FILTAlignStretch
    static let flexStart = FILTAlignFlexStart
    static let center = FILTAlignCenter
    static let flexEnd = FILTAlignFlexEnd
    static let spaceBetween = FILTAlignSpaceBetween
    static let spaceAround = FILTAlignSpaceAround
}

extension FILTWrap {
    static let noWrap = FILTWrapNoWrap
    static let wrap = FILTWrapWrap
    static let wrapReverse = FILTWrapWrapReverse
}

extension FILTPositionType {
    static let relative = FILTPositionTypeRelative
    static let absolute = FILTPositionTypeAbsolute
}

extension FILTDirection {
    static let LTR = FILTDirectionLTR
    static let RTL = FILTDirectionRTL
    static let inherit = FILTDirectionInherit
}

extension FILTDisplay {
    static let flex = FILTDisplayFlex
    static let none = FILTDisplayNone
}

extension FILTUnit {
    static let percent = FILTUnitPercent
    static let point = FILTUnitPoint
}

extension Flex.Direction {
    var filatesValue: FILTFlexDirection {
        switch self {
        case .column:        return FILTFlexDirection.column
        case .columnReverse: return FILTFlexDirection.columnReverse
        case .row:           return FILTFlexDirection.row
        case .rowReverse:    return FILTFlexDirection.rowReverse
        }
    }
}

extension Flex.JustifyContent {
    var filatesValue: FILTJustify {
        switch self {
        case .start:        return FILTJustify.flexStart
        case .center:       return FILTJustify.center
        case .end:          return FILTJustify.flexEnd
        case .spaceBetween: return FILTJustify.spaceBetween
        case .spaceAround:  return FILTJustify.spaceAround
        case .spaceEvenly:  return FILTJustify.spaceEvenly
        }
    }
}

extension Flex.AlignContent {
    var filatesValue: FILTAlign {
        switch self {
        case .stretch:      return FILTAlign.stretch
        case .start:        return FILTAlign.flexStart
        case .center:       return FILTAlign.center
        case .end:          return FILTAlign.flexEnd
        case .spaceBetween: return FILTAlign.spaceBetween
        case .spaceAround:  return FILTAlign.spaceAround
        }
    }
}

extension Flex.AlignItems {
    var filatesValue: FILTAlign {
        switch self {
        case .stretch:      return FILTAlign.stretch
        case .start:        return FILTAlign.flexStart
        case .center:       return FILTAlign.center
        case .end:          return FILTAlign.flexEnd
        case .baseline:     return FILTAlign.baseline
        }
    }
}

extension Flex.AlignSelf {
    var filatesValue: FILTAlign {
        switch self {
        case .auto:         return FILTAlign.auto
        case .stretch:      return FILTAlign.stretch
        case .start:        return FILTAlign.flexStart
        case .center:       return FILTAlign.center
        case .end:          return FILTAlign.flexEnd
        case .baseline:     return FILTAlign.baseline
        }
    }
}

extension Flex.Wrap {
    var filatesValue: FILTWrap {
        switch self {
        case .noWrap:      return FILTWrap.noWrap
        case .wrap:        return FILTWrap.wrap
        case .wrapReverse: return FILTWrap.wrapReverse
        }
    }
}

extension Flex.Position {
    var filatesValue: FILTPositionType {
        switch self {
        case .relative: return FILTPositionType.relative
        case .absolute: return FILTPositionType.absolute
        }
    }
}

extension Flex.LayoutDirection {
    var filatesValue: FILTDirection {
        switch self {
        case .ltr: return FILTDirection.LTR
        case .rtl: return FILTDirection.RTL
        default:   return FILTDirection.inherit
        }
    }
}

extension Flex.Display {
    var filatesValue: FILTDisplay {
        switch self {
        case .flex: return FILTDisplay.flex
        case .none: return FILTDisplay.none
        }
    }
}

/*extension Flex.Overflow {
    var filatesValue: FILTOverflow {
        switch self {
        case .visible: return FILTOverflow.visible
        case .hidden:  return FILTOverflow.hidden
        case .scroll:  return FILTOverflow.scroll
        }
    }
}*/
