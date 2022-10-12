/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "FILTEnums.h"

const char *FILTAlignToString(const FILTAlign value){
  switch(value){
    case FILTAlignAuto:
      return "auto";
    case FILTAlignFlexStart:
      return "flex-start";
    case FILTAlignCenter:
      return "center";
    case FILTAlignFlexEnd:
      return "flex-end";
    case FILTAlignStretch:
      return "stretch";
    case FILTAlignBaseline:
      return "baseline";
    case FILTAlignSpaceBetween:
      return "space-between";
    case FILTAlignSpaceAround:
      return "space-around";
  }
  return "unknown";
}

const char *FILTDimensionToString(const FILTDimension value){
  switch(value){
    case FILTDimensionWidth:
      return "width";
    case FILTDimensionHeight:
      return "height";
  }
  return "unknown";
}

const char *FILTDirectionToString(const FILTDirection value){
  switch(value){
    case FILTDirectionInherit:
      return "inherit";
    case FILTDirectionLTR:
      return "ltr";
    case FILTDirectionRTL:
      return "rtl";
  }
  return "unknown";
}

const char *FILTDisplayToString(const FILTDisplay value){
  switch(value){
    case FILTDisplayFlex:
      return "flex";
    case FILTDisplayNone:
      return "none";
  }
  return "unknown";
}

const char *FILTEdgeToString(const FILTEdge value){
  switch(value){
    case FILTEdgeLeft:
      return "left";
    case FILTEdgeTop:
      return "top";
    case FILTEdgeRight:
      return "right";
    case FILTEdgeBottom:
      return "bottom";
    case FILTEdgeStart:
      return "start";
    case FILTEdgeEnd:
      return "end";
    case FILTEdgeHorizontal:
      return "horizontal";
    case FILTEdgeVertical:
      return "vertical";
    case FILTEdgeAll:
      return "all";
  }
  return "unknown";
}

const char *FILTExperimentalFeatureToString(const FILTExperimentalFeature value){
  switch(value){
    case FILTExperimentalFeatureWebFlexBasis:
      return "web-flex-basis";
  }
  return "unknown";
}

const char *FILTFlexDirectionToString(const FILTFlexDirection value){
  switch(value){
    case FILTFlexDirectionColumn:
      return "column";
    case FILTFlexDirectionColumnReverse:
      return "column-reverse";
    case FILTFlexDirectionRow:
      return "row";
    case FILTFlexDirectionRowReverse:
      return "row-reverse";
  }
  return "unknown";
}

const char *FILTJustifyToString(const FILTJustify value){
  switch(value){
    case FILTJustifyFlexStart:
      return "flex-start";
    case FILTJustifyCenter:
      return "center";
    case FILTJustifyFlexEnd:
      return "flex-end";
    case FILTJustifySpaceBetween:
      return "space-between";
    case FILTJustifySpaceAround:
      return "space-around";
    case FILTJustifySpaceEvenly:
      return "space-evenly";
  }
  return "unknown";
}

const char *FILTLogLevelToString(const FILTLogLevel value){
  switch(value){
    case FILTLogLevelError:
      return "error";
    case FILTLogLevelWarn:
      return "warn";
    case FILTLogLevelInfo:
      return "info";
    case FILTLogLevelDebug:
      return "debug";
    case FILTLogLevelVerbose:
      return "verbose";
    case FILTLogLevelFatal:
      return "fatal";
  }
  return "unknown";
}

const char *FILTMeasureModeToString(const FILTMeasureMode value){
  switch(value){
    case FILTMeasureModeUndefined:
      return "undefined";
    case FILTMeasureModeExactly:
      return "exactly";
    case FILTMeasureModeAtMost:
      return "at-most";
  }
  return "unknown";
}

const char *FILTNodeTypeToString(const FILTNodeType value){
  switch(value){
    case FILTNodeTypeDefault:
      return "default";
    case FILTNodeTypeText:
      return "text";
  }
  return "unknown";
}

const char *FILTOverflowToString(const FILTOverflow value){
  switch(value){
    case FILTOverflowVisible:
      return "visible";
    case FILTOverflowHidden:
      return "hidden";
    case FILTOverflowScroll:
      return "scroll";
  }
  return "unknown";
}

const char *FILTPositionTypeToString(const FILTPositionType value){
  switch(value){
    case FILTPositionTypeRelative:
      return "relative";
    case FILTPositionTypeAbsolute:
      return "absolute";
  }
  return "unknown";
}

const char *FILTPrintOptionsToString(const FILTPrintOptions value){
  switch(value){
    case FILTPrintOptionsLayout:
      return "layout";
    case FILTPrintOptionsStyle:
      return "style";
    case FILTPrintOptionsChildren:
      return "children";
  }
  return "unknown";
}

const char *FILTUnitToString(const FILTUnit value){
  switch(value){
    case FILTUnitUndefined:
      return "undefined";
    case FILTUnitPoint:
      return "point";
    case FILTUnitPercent:
      return "percent";
    case FILTUnitAuto:
      return "auto";
  }
  return "unknown";
}

const char *FILTWrapToString(const FILTWrap value){
  switch(value){
    case FILTWrapNoWrap:
      return "no-wrap";
    case FILTWrapWrap:
      return "wrap";
    case FILTWrapWrapReverse:
      return "wrap-reverse";
  }
  return "unknown";
}
