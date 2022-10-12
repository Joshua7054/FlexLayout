/*
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "FILTNodePrint.h"
#include <stdarg.h>
#include "FILTEnums.h"
#include "FILTNode.h"
#include "Filates-internal.h"

namespace facebook {
namespace filates {
typedef std::string string;

static void indent(string* base, uint32_t level) {
  for (uint32_t i = 0; i < level; ++i) {
    base->append("  ");
  }
}

static bool areFourValuesEqual(const std::array<FILTValue, FILTEdgeCount>& four) {
  return FILTValueEqual(four[0], four[1]) && FILTValueEqual(four[0], four[2]) &&
      FILTValueEqual(four[0], four[3]);
}

static void appendFormatedString(string* str, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  va_list argsCopy;
  va_copy(argsCopy, args);
  std::vector<char> buf(1 + vsnprintf(NULL, 0, fmt, args));
  va_end(args);
  vsnprintf(buf.data(), buf.size(), fmt, argsCopy);
  va_end(argsCopy);
  string result = string(buf.begin(), buf.end() - 1);
  str->append(result);
}

static void
appendFloatIfNotUndefined(string* base, const string key, const float num) {
  if (!FILTFloatIsUndefined(num)) {
    appendFormatedString(base, "%s: %g; ", key.c_str(), num);
  }
}

static void appendNumberIfNotUndefined(
    string* base,
    const string key,
    const FILTValue number) {
  if (number.unit != FILTUnitUndefined) {
    if (number.unit == FILTUnitAuto) {
      base->append(key + ": auto; ");
    } else {
      string unit = number.unit == FILTUnitPoint ? "px" : "%%";
      appendFormatedString(
          base, "%s: %g%s; ", key.c_str(), number.value, unit.c_str());
    }
  }
}

static void
appendNumberIfNotAuto(string* base, const string& key, const FILTValue number) {
  if (number.unit != FILTUnitAuto) {
    appendNumberIfNotUndefined(base, key, number);
  }
}

static void
appendNumberIfNotZero(string* base, const string& str, const FILTValue number) {
  if (!FILTFloatsEqual(number.value, 0)) {
    appendNumberIfNotUndefined(base, str, number);
  }
}

static void appendEdges(
    string* base,
    const string& key,
    const std::array<FILTValue, FILTEdgeCount>& edges) {
  if (areFourValuesEqual(edges)) {
    appendNumberIfNotZero(base, key, edges[FILTEdgeLeft]);
  } else {
    for (int edge = FILTEdgeLeft; edge != FILTEdgeAll; ++edge) {
      string str = key + "-" + FILTEdgeToString(static_cast<FILTEdge>(edge));
      appendNumberIfNotZero(base, str, edges[edge]);
    }
  }
}

static void appendEdgeIfNotUndefined(
    string* base,
    const string& str,
    const std::array<FILTValue, FILTEdgeCount>& edges,
    const FILTEdge edge) {
  appendNumberIfNotUndefined(
      base, str, *FILTComputedEdgeValue(edges, edge, &FILTValueUndefined));
}

void FILTNodeToString(
    std::string* str,
    FILTNodeRef node,
    FILTPrintOptions options,
    uint32_t level) {
  indent(str, level);
  appendFormatedString(str, "<div ");
  if (node->getPrintFunc() != nullptr) {
    node->getPrintFunc()(node);
  }

  if (options & FILTPrintOptionsLayout) {
    appendFormatedString(str, "layout=\"");
    appendFormatedString(
        str, "width: %g; ", node->getLayout().dimensions[FILTDimensionWidth]);
    appendFormatedString(
        str, "height: %g; ", node->getLayout().dimensions[FILTDimensionHeight]);
    appendFormatedString(
        str, "top: %g; ", node->getLayout().position[FILTEdgeTop]);
    appendFormatedString(
        str, "left: %g;", node->getLayout().position[FILTEdgeLeft]);
    appendFormatedString(str, "\" ");
  }

  if (options & FILTPrintOptionsStyle) {
    appendFormatedString(str, "style=\"");
    if (node->getStyle().flexDirection != FILTNode().getStyle().flexDirection) {
      appendFormatedString(
          str,
          "flex-direction: %s; ",
          FILTFlexDirectionToString(node->getStyle().flexDirection));
    }
    if (node->getStyle().justifyContent != FILTNode().getStyle().justifyContent) {
      appendFormatedString(
          str,
          "justify-content: %s; ",
          FILTJustifyToString(node->getStyle().justifyContent));
    }
    if (node->getStyle().alignItems != FILTNode().getStyle().alignItems) {
      appendFormatedString(
          str,
          "align-items: %s; ",
          FILTAlignToString(node->getStyle().alignItems));
    }
    if (node->getStyle().alignContent != FILTNode().getStyle().alignContent) {
      appendFormatedString(
          str,
          "align-content: %s; ",
          FILTAlignToString(node->getStyle().alignContent));
    }
    if (node->getStyle().alignSelf != FILTNode().getStyle().alignSelf) {
      appendFormatedString(
          str, "align-self: %s; ", FILTAlignToString(node->getStyle().alignSelf));
    }
    appendFloatIfNotUndefined(str, "flex-grow", node->getStyle().flexGrow);
    appendFloatIfNotUndefined(str, "flex-shrink", node->getStyle().flexShrink);
    appendNumberIfNotAuto(str, "flex-basis", node->getStyle().flexBasis);
    appendFloatIfNotUndefined(str, "flex", node->getStyle().flex);

    if (node->getStyle().flexWrap != FILTNode().getStyle().flexWrap) {
      appendFormatedString(
          str, "flexWrap: %s; ", FILTWrapToString(node->getStyle().flexWrap));
    }

    if (node->getStyle().overflow != FILTNode().getStyle().overflow) {
      appendFormatedString(
          str, "overflow: %s; ", FILTOverflowToString(node->getStyle().overflow));
    }

    if (node->getStyle().display != FILTNode().getStyle().display) {
      appendFormatedString(
          str, "display: %s; ", FILTDisplayToString(node->getStyle().display));
    }
    appendEdges(str, "margin", node->getStyle().margin);
    appendEdges(str, "padding", node->getStyle().padding);
    appendEdges(str, "border", node->getStyle().border);

    appendNumberIfNotAuto(
        str, "width", node->getStyle().dimensions[FILTDimensionWidth]);
    appendNumberIfNotAuto(
        str, "height", node->getStyle().dimensions[FILTDimensionHeight]);
    appendNumberIfNotAuto(
        str, "max-width", node->getStyle().maxDimensions[FILTDimensionWidth]);
    appendNumberIfNotAuto(
        str, "max-height", node->getStyle().maxDimensions[FILTDimensionHeight]);
    appendNumberIfNotAuto(
        str, "min-width", node->getStyle().minDimensions[FILTDimensionWidth]);
    appendNumberIfNotAuto(
        str, "min-height", node->getStyle().minDimensions[FILTDimensionHeight]);

    if (node->getStyle().positionType != FILTNode().getStyle().positionType) {
      appendFormatedString(
          str,
          "position: %s; ",
          FILTPositionTypeToString(node->getStyle().positionType));
    }

    appendEdgeIfNotUndefined(
        str, "left", node->getStyle().position, FILTEdgeLeft);
    appendEdgeIfNotUndefined(
        str, "right", node->getStyle().position, FILTEdgeRight);
    appendEdgeIfNotUndefined(str, "top", node->getStyle().position, FILTEdgeTop);
    appendEdgeIfNotUndefined(
        str, "bottom", node->getStyle().position, FILTEdgeBottom);
    appendFormatedString(str, "\" ");

    if (node->getMeasure() != nullptr) {
      appendFormatedString(str, "has-custom-measure=\"true\"");
    }
  }
  appendFormatedString(str, ">");

  const uint32_t childCount = static_cast<uint32_t>(node->getChildren().size());
  if (options & FILTPrintOptionsChildren && childCount > 0) {
    for (uint32_t i = 0; i < childCount; i++) {
      appendFormatedString(str, "\n");
      FILTNodeToString(str, FILTNodeGetChild(node, i), options, level + 1);
    }
    appendFormatedString(str, "\n");
    indent(str, level);
  }
  appendFormatedString(str, "</div>");
}
} // namespace filates
} // namespace facebook
