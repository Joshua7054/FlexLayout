/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Filates.h"
#include <string.h>
#include <algorithm>
#include "Utils.h"
#include "FILTNode.h"
#include "FILTNodePrint.h"
#include "Filates-internal.h"

#ifdef _MSC_VER
#include <float.h>

/* define fmaxf if < VC12 */
#if _MSC_VER < 1800
__forceinline const float fmaxf(const float a, const float b) {
  return (a > b) ? a : b;
}
#endif
#endif

#ifdef ANDROID
static int FILTAndroidLog(const FILTConfigRef config,
                        const FILTNodeRef node,
                        FILTLogLevel level,
                        const char *format,
                        va_list args);
#else
static int FILTDefaultLog(const FILTConfigRef config,
                        const FILTNodeRef node,
                        FILTLogLevel level,
                        const char *format,
                        va_list args);
#endif

static FILTConfig gFILTConfigDefaults = {
    .experimentalFeatures =
        {
            [FILTExperimentalFeatureWebFlexBasis] = false,
        },
    .useWebDefaults = false,
    .useLegacyStretchBehaviour = false,
    .shouldDiffLayoutWithoutLegacyStretchBehaviour = false,
    .pointScaleFactor = 1.0f,
#ifdef ANDROID
    .logger = &FILTAndroidLog,
#else
    .logger = &FILTDefaultLog,
#endif
    .cloneNodeCallback = nullptr,
    .context = nullptr,
};

const FILTValue FILTValueZero = {.value = 0, .unit = FILTUnitPoint};
const FILTValue FILTValueUndefined = {FILTUndefined, FILTUnitUndefined};
const FILTValue FILTValueAuto = {FILTUndefined, FILTUnitAuto};

#ifdef ANDROID
#include <android/log.h>
static int FILTAndroidLog(const FILTConfigRef config,
                        const FILTNodeRef node,
                        FILTLogLevel level,
                        const char *format,
                        va_list args) {
  int androidLevel = FILTLogLevelDebug;
  switch (level) {
    case FILTLogLevelFatal:
      androidLevel = ANDROID_LOG_FATAL;
      break;
    case FILTLogLevelError:
      androidLevel = ANDROID_LOG_ERROR;
      break;
    case FILTLogLevelWarn:
      androidLevel = ANDROID_LOG_WARN;
      break;
    case FILTLogLevelInfo:
      androidLevel = ANDROID_LOG_INFO;
      break;
    case FILTLogLevelDebug:
      androidLevel = ANDROID_LOG_DEBUG;
      break;
    case FILTLogLevelVerbose:
      androidLevel = ANDROID_LOG_VERBOSE;
      break;
  }
  const int result = __android_log_vprint(androidLevel, "filates", format, args);
  return result;
}
#else
#define FILT_UNUSED(x) (void)(x);

static int FILTDefaultLog(const FILTConfigRef config,
                        const FILTNodeRef node,
                        FILTLogLevel level,
                        const char *format,
                        va_list args) {
  FILT_UNUSED(config);
  FILT_UNUSED(node);
  switch (level) {
    case FILTLogLevelError:
    case FILTLogLevelFatal:
      return vfprintf(stderr, format, args);
    case FILTLogLevelWarn:
    case FILTLogLevelInfo:
    case FILTLogLevelDebug:
    case FILTLogLevelVerbose:
    default:
      return vprintf(format, args);
  }
}

#undef FILT_UNUSED
#endif

bool FILTFloatIsUndefined(const float value) {
  return std::isnan(value);
}

const FILTValue* FILTComputedEdgeValue(
    const std::array<FILTValue, FILTEdgeCount>& edges,
    const FILTEdge edge,
    const FILTValue* const defaultValue) {
  if (edges[edge].unit != FILTUnitUndefined) {
    return &edges[edge];
  }

  if ((edge == FILTEdgeTop || edge == FILTEdgeBottom) &&
      edges[FILTEdgeVertical].unit != FILTUnitUndefined) {
    return &edges[FILTEdgeVertical];
  }

  if ((edge == FILTEdgeLeft || edge == FILTEdgeRight || edge == FILTEdgeStart || edge == FILTEdgeEnd) &&
      edges[FILTEdgeHorizontal].unit != FILTUnitUndefined) {
    return &edges[FILTEdgeHorizontal];
  }

  if (edges[FILTEdgeAll].unit != FILTUnitUndefined) {
    return &edges[FILTEdgeAll];
  }

  if (edge == FILTEdgeStart || edge == FILTEdgeEnd) {
    return &FILTValueUndefined;
  }

  return defaultValue;
}

void* FILTNodeGetContext(FILTNodeRef node) {
  return node->getContext();
}

void FILTNodeSetContext(FILTNodeRef node, void* context) {
  return node->setContext(context);
}

FILTMeasureFunc FILTNodeGetMeasureFunc(FILTNodeRef node) {
  return node->getMeasure();
}

void FILTNodeSetMeasureFunc(FILTNodeRef node, FILTMeasureFunc measureFunc) {
  node->setMeasureFunc(measureFunc);
}

FILTBaselineFunc FILTNodeGetBaselineFunc(FILTNodeRef node) {
  return node->getBaseline();
}

void FILTNodeSetBaselineFunc(FILTNodeRef node, FILTBaselineFunc baselineFunc) {
  node->setBaseLineFunc(baselineFunc);
}

FILTDirtiedFunc FILTNodeGetDirtiedFunc(FILTNodeRef node) {
  return node->getDirtied();
}

void FILTNodeSetDirtiedFunc(FILTNodeRef node, FILTDirtiedFunc dirtiedFunc) {
  node->setDirtiedFunc(dirtiedFunc);
}

FILTPrintFunc FILTNodeGetPrintFunc(FILTNodeRef node) {
  return node->getPrintFunc();
}

void FILTNodeSetPrintFunc(FILTNodeRef node, FILTPrintFunc printFunc) {
  node->setPrintFunc(printFunc);
}

bool FILTNodeGetHasNewLayout(FILTNodeRef node) {
  return node->getHasNewLayout();
}

void FILTNodeSetHasNewLayout(FILTNodeRef node, bool hasNewLayout) {
  node->setHasNewLayout(hasNewLayout);
}

FILTNodeType FILTNodeGetNodeType(FILTNodeRef node) {
  return node->getNodeType();
}

void FILTNodeSetNodeType(FILTNodeRef node, FILTNodeType nodeType) {
  return node->setNodeType(nodeType);
}

bool FILTNodeIsDirty(FILTNodeRef node) {
  return node->isDirty();
}

bool FILTNodeLayoutGetDidUseLegacyFlag(const FILTNodeRef node) {
  return node->didUseLegacyFlag();
}

void FILTNodeMarkDirtyAndPropogateToDescendants(const FILTNodeRef node) {
  return node->markDirtyAndPropogateDownwards();
}

int32_t gNodeInstanceCount = 0;
int32_t gConfigInstanceCount = 0;

WIN_EXPORT FILTNodeRef FILTNodeNewWithConfig(const FILTConfigRef config) {
  const FILTNodeRef node = new FILTNode();
  FILTAssertWithConfig(
      config, node != nullptr, "Could not allocate memory for node");
  gNodeInstanceCount++;

  if (config->useWebDefaults) {
    node->setStyleFlexDirection(FILTFlexDirectionRow);
    node->setStyleAlignContent(FILTAlignStretch);
  }
  node->setConfig(config);
  return node;
}

FILTNodeRef FILTNodeNew(void) {
  return FILTNodeNewWithConfig(&gFILTConfigDefaults);
}

FILTNodeRef FILTNodeClone(FILTNodeRef oldNode) {
  FILTNodeRef node = new FILTNode(*oldNode);
  FILTAssertWithConfig(
      oldNode->getConfig(),
      node != nullptr,
      "Could not allocate memory for node");
  gNodeInstanceCount++;
  node->setParent(nullptr);
  return node;
}

static FILTConfigRef FILTConfigClone(const FILTConfig& oldConfig) {
  const FILTConfigRef config = new FILTConfig(oldConfig);
  FILTAssert(config != nullptr, "Could not allocate memory for config");
  if (config == nullptr) {
    abort();
  }
  gConfigInstanceCount++;
  return config;
}

static FILTNodeRef FILTNodeDeepClone(FILTNodeRef oldNode) {
  FILTNodeRef node = FILTNodeClone(oldNode);
  FILTVector vec = FILTVector();
  vec.reserve(oldNode->getChildren().size());
  FILTNodeRef childNode = nullptr;
  for (auto& item : oldNode->getChildren()) {
    childNode = FILTNodeDeepClone(item);
    childNode->setParent(node);
    vec.push_back(childNode);
  }
  node->setChildren(vec);

  if (oldNode->getConfig() != nullptr) {
    node->setConfig(FILTConfigClone(*(oldNode->getConfig())));
  }

  if (oldNode->getNextChild() != nullptr) {
    node->setNextChild(FILTNodeDeepClone(oldNode->getNextChild()));
  }

  return node;
}

void FILTNodeFree(const FILTNodeRef node) {
  if (node->getParent()) {
    node->getParent()->removeChild(node);
    node->setParent(nullptr);
  }

  const uint32_t childCount = FILTNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const FILTNodeRef child = FILTNodeGetChild(node, i);
    child->setParent(nullptr);
  }

  node->clearChildren();
  delete node;
  gNodeInstanceCount--;
}

static void FILTConfigFreeRecursive(const FILTNodeRef root) {
  if (root->getConfig() != nullptr) {
    gConfigInstanceCount--;
    delete root->getConfig();
  }
  // Delete configs recursively for childrens
  for (uint32_t i = 0; i < root->getChildrenCount(); ++i) {
    FILTConfigFreeRecursive(root->getChild(i));
  }
}

void FILTNodeFreeRecursive(const FILTNodeRef root) {
  while (FILTNodeGetChildCount(root) > 0) {
    const FILTNodeRef child = FILTNodeGetChild(root, 0);
    if (child->getParent() != root) {
      // Don't free shared nodes that we don't own.
      break;
    }
    FILTNodeRemoveChild(root, child);
    FILTNodeFreeRecursive(child);
  }
  FILTNodeFree(root);
}

void FILTNodeReset(const FILTNodeRef node) {
  FILTAssertWithNode(node,
                   FILTNodeGetChildCount(node) == 0,
                   "Cannot reset a node which still has children attached");
  FILTAssertWithNode(
      node,
      node->getParent() == nullptr,
      "Cannot reset a node still attached to a parent");

  node->clearChildren();

  const FILTConfigRef config = node->getConfig();
  *node = FILTNode();
  if (config->useWebDefaults) {
    node->setStyleFlexDirection(FILTFlexDirectionRow);
    node->setStyleAlignContent(FILTAlignStretch);
  }
  node->setConfig(config);
}

int32_t FILTNodeGetInstanceCount(void) {
  return gNodeInstanceCount;
}

int32_t FILTConfigGetInstanceCount(void) {
  return gConfigInstanceCount;
}

// Export only for C#
FILTConfigRef FILTConfigGetDefault() {
  return &gFILTConfigDefaults;
}

FILTConfigRef FILTConfigNew(void) {
  const FILTConfigRef config = (const FILTConfigRef)malloc(sizeof(FILTConfig));
  FILTAssert(config != nullptr, "Could not allocate memory for config");
  if (config == nullptr) {
    abort();
  }
  gConfigInstanceCount++;
  memcpy(config, &gFILTConfigDefaults, sizeof(FILTConfig));
  return config;
}

void FILTConfigFree(const FILTConfigRef config) {
  free(config);
  gConfigInstanceCount--;
}

void FILTConfigCopy(const FILTConfigRef dest, const FILTConfigRef src) {
  memcpy(dest, src, sizeof(FILTConfig));
}

void FILTNodeInsertChild(const FILTNodeRef node, const FILTNodeRef child, const uint32_t index) {
  FILTAssertWithNode(
      node,
      child->getParent() == nullptr,
      "Child already has a parent, it must be removed first.");
  FILTAssertWithNode(
      node,
      node->getMeasure() == nullptr,
      "Cannot add child: Nodes with measure functions cannot have children.");

  node->cloneChildrenIfNeeded();
  node->insertChild(child, index);
  child->setParent(node);
  node->markDirtyAndPropogate();
}

void FILTNodeRemoveChild(const FILTNodeRef parent, const FILTNodeRef excludedChild) {
  // This algorithm is a forked variant from cloneChildrenIfNeeded in FILTNode
  // that excludes a child.
  const uint32_t childCount = FILTNodeGetChildCount(parent);

  if (childCount == 0) {
    // This is an empty set. Nothing to remove.
    return;
  }
  const FILTNodeRef firstChild = FILTNodeGetChild(parent, 0);
  if (firstChild->getParent() == parent) {
    // If the first child has this node as its parent, we assume that it is already unique.
    // We can now try to delete a child in this list.
    if (parent->removeChild(excludedChild)) {
      excludedChild->setLayout(
          FILTNode().getLayout()); // layout is no longer valid
      excludedChild->setParent(nullptr);
      parent->markDirtyAndPropogate();
    }
    return;
  }
  // Otherwise we have to clone the node list except for the child we're trying to delete.
  // We don't want to simply clone all children, because then the host will need to free
  // the clone of the child that was just deleted.
  const FILTNodeClonedFunc cloneNodeCallback =
      parent->getConfig()->cloneNodeCallback;
  uint32_t nextInsertIndex = 0;
  for (uint32_t i = 0; i < childCount; i++) {
    const FILTNodeRef oldChild = parent->getChild(i);
    if (excludedChild == oldChild) {
      // Ignore the deleted child. Don't reset its layout or parent since it is still valid
      // in the other parent. However, since this parent has now changed, we need to mark it
      // as dirty.
      parent->markDirtyAndPropogate();
      continue;
    }
    const FILTNodeRef newChild = FILTNodeClone(oldChild);
    parent->replaceChild(newChild, nextInsertIndex);
    newChild->setParent(parent);
    if (cloneNodeCallback) {
      cloneNodeCallback(oldChild, newChild, parent, nextInsertIndex);
    }
    nextInsertIndex++;
  }
  while (nextInsertIndex < childCount) {
    parent->removeChild(nextInsertIndex);
    nextInsertIndex++;
  }
}

void FILTNodeRemoveAllChildren(const FILTNodeRef parent) {
  const uint32_t childCount = FILTNodeGetChildCount(parent);
  if (childCount == 0) {
    // This is an empty set already. Nothing to do.
    return;
  }
  const FILTNodeRef firstChild = FILTNodeGetChild(parent, 0);
  if (firstChild->getParent() == parent) {
    // If the first child has this node as its parent, we assume that this child set is unique.
    for (uint32_t i = 0; i < childCount; i++) {
      const FILTNodeRef oldChild = FILTNodeGetChild(parent, i);
      oldChild->setLayout(FILTNode().getLayout()); // layout is no longer valid
      oldChild->setParent(nullptr);
    }
    parent->clearChildren();
    parent->markDirtyAndPropogate();
    return;
  }
  // Otherwise, we are not the owner of the child set. We don't have to do anything to clear it.
  parent->setChildren(FILTVector());
  parent->markDirtyAndPropogate();
}

FILTNodeRef FILTNodeGetChild(const FILTNodeRef node, const uint32_t index) {
  if (index < node->getChildren().size()) {
    return node->getChild(index);
  }
  return nullptr;
}

uint32_t FILTNodeGetChildCount(const FILTNodeRef node) {
  return static_cast<uint32_t>(node->getChildren().size());
}

FILTNodeRef FILTNodeGetParent(const FILTNodeRef node) {
  return node->getParent();
}

void FILTNodeMarkDirty(const FILTNodeRef node) {
  FILTAssertWithNode(
      node,
      node->getMeasure() != nullptr,
      "Only leaf nodes with custom measure functions"
      "should manually mark themselves as dirty");

  node->markDirtyAndPropogate();
}

void FILTNodeCopyStyle(const FILTNodeRef dstNode, const FILTNodeRef srcNode) {
  if (!(dstNode->getStyle() == srcNode->getStyle())) {
    dstNode->setStyle(srcNode->getStyle());
    dstNode->markDirtyAndPropogate();
  }
}

float FILTNodeStyleGetFlexGrow(const FILTNodeRef node) {
  return FILTFloatIsUndefined(node->getStyle().flexGrow)
      ? kDefaultFlexGrow
      : node->getStyle().flexGrow;
}

float FILTNodeStyleGetFlexShrink(const FILTNodeRef node) {
  return FILTFloatIsUndefined(node->getStyle().flexShrink)
      ? (node->getConfig()->useWebDefaults ? kWebDefaultFlexShrink
                                           : kDefaultFlexShrink)
      : node->getStyle().flexShrink;
}

#define FILT_NODE_STYLE_PROPERTY_SETTER_IMPL(                               \
    type, name, paramName, instanceName)                                  \
  void FILTNodeStyleSet##name(const FILTNodeRef node, const type paramName) { \
    if (node->getStyle().instanceName != paramName) {                     \
      FILTStyle style = node->getStyle();                                   \
      style.instanceName = paramName;                                     \
      node->setStyle(style);                                              \
      node->markDirtyAndPropogate();                                      \
    }                                                                     \
  }

#define FILT_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(                               \
    type, name, paramName, instanceName)                                       \
  void FILTNodeStyleSet##name(const FILTNodeRef node, const type paramName) {      \
    FILTValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = FILTFloatIsUndefined(paramName) ? FILTUnitUndefined : FILTUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != FILTUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void FILTNodeStyleSet##name##Percent(                                          \
      const FILTNodeRef node, const type paramName) {                            \
    FILTValue value = {                                                          \
        .value = paramName,                                                    \
        .unit =                                                                \
            FILTFloatIsUndefined(paramName) ? FILTUnitUndefined : FILTUnitPercent,   \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != FILTUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
      FILTStyle style = node->getStyle();                                        \
                                                                               \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }

#define FILT_NODE_STYLE_PROPERTY_SETTER_UNIT_AUTO_IMPL(                          \
    type, name, paramName, instanceName)                                       \
  void FILTNodeStyleSet##name(const FILTNodeRef node, const type paramName) {      \
    FILTValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = FILTFloatIsUndefined(paramName) ? FILTUnitUndefined : FILTUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != FILTUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void FILTNodeStyleSet##name##Percent(                                          \
      const FILTNodeRef node, const type paramName) {                            \
    if (node->getStyle().instanceName.value != paramName ||                    \
        node->getStyle().instanceName.unit != FILTUnitPercent) {                 \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName.value = paramName;                                    \
      style.instanceName.unit =                                                \
          FILTFloatIsUndefined(paramName) ? FILTUnitAuto : FILTUnitPercent;          \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void FILTNodeStyleSet##name##Auto(const FILTNodeRef node) {                      \
    if (node->getStyle().instanceName.unit != FILTUnitAuto) {                    \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName.value = FILTUndefined;                                  \
      style.instanceName.unit = FILTUnitAuto;                                    \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }

#define FILT_NODE_STYLE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
  FILT_NODE_STYLE_PROPERTY_SETTER_IMPL(type, name, paramName, instanceName) \
                                                                          \
  type FILTNodeStyleGet##name(const FILTNodeRef node) {                       \
    return node->getStyle().instanceName;                                 \
  }

#define FILT_NODE_STYLE_PROPERTY_UNIT_IMPL(type, name, paramName, instanceName) \
  FILT_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(                                    \
      float, name, paramName, instanceName)                                   \
                                                                              \
  type FILTNodeStyleGet##name(const FILTNodeRef node) {                           \
    return node->getStyle().instanceName;                                     \
  }

#define FILT_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(      \
    type, name, paramName, instanceName)            \
  FILT_NODE_STYLE_PROPERTY_SETTER_UNIT_AUTO_IMPL(     \
      float, name, paramName, instanceName)         \
                                                    \
  type FILTNodeStyleGet##name(const FILTNodeRef node) { \
    return node->getStyle().instanceName;           \
  }

#define FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO_IMPL(type, name, instanceName) \
  void FILTNodeStyleSet##name##Auto(const FILTNodeRef node, const FILTEdge edge) { \
    if (node->getStyle().instanceName[edge].unit != FILTUnitAuto) {            \
      FILTStyle style = node->getStyle();                                      \
      style.instanceName[edge].value = FILTUndefined;                          \
      style.instanceName[edge].unit = FILTUnitAuto;                            \
      node->setStyle(style);                                                 \
      node->markDirtyAndPropogate();                                         \
    }                                                                        \
  }

#define FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(                                 \
    type, name, paramName, instanceName)                                       \
  void FILTNodeStyleSet##name(                                                   \
      const FILTNodeRef node, const FILTEdge edge, const float paramName) {        \
    FILTValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = FILTFloatIsUndefined(paramName) ? FILTUnitUndefined : FILTUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != FILTUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void FILTNodeStyleSet##name##Percent(                                          \
      const FILTNodeRef node, const FILTEdge edge, const float paramName) {        \
    FILTValue value = {                                                          \
        .value = paramName,                                                    \
        .unit =                                                                \
            FILTFloatIsUndefined(paramName) ? FILTUnitUndefined : FILTUnitPercent,   \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != FILTUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  WIN_STRUCT(type)                                                             \
  FILTNodeStyleGet##name(const FILTNodeRef node, const FILTEdge edge) {              \
    return WIN_STRUCT_REF(node->getStyle().instanceName[edge]);                \
  }

#define FILT_NODE_STYLE_EDGE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
  void FILTNodeStyleSet##name(                                                   \
      const FILTNodeRef node, const FILTEdge edge, const float paramName) {        \
    FILTValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = FILTFloatIsUndefined(paramName) ? FILTUnitUndefined : FILTUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != FILTUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
      FILTStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  float FILTNodeStyleGet##name(const FILTNodeRef node, const FILTEdge edge) {        \
    return node->getStyle().instanceName[edge].value;                          \
  }

#define FILT_NODE_LAYOUT_PROPERTY_IMPL(type, name, instanceName) \
  type FILTNodeLayoutGet##name(const FILTNodeRef node) {           \
    return node->getLayout().instanceName;                     \
  }

#define FILT_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(type, name, instanceName) \
  type FILTNodeLayoutGet##name(const FILTNodeRef node, const FILTEdge edge) { \
    FILTAssertWithNode(                                                   \
        node,                                                           \
        edge <= FILTEdgeEnd,                                              \
        "Cannot get layout properties of multi-edge shorthands");       \
                                                                        \
    if (edge == FILTEdgeLeft) {                                           \
      if (node->getLayout().direction == FILTDirectionRTL) {              \
        return node->getLayout().instanceName[FILTEdgeEnd];               \
      } else {                                                          \
        return node->getLayout().instanceName[FILTEdgeStart];             \
      }                                                                 \
    }                                                                   \
                                                                        \
    if (edge == FILTEdgeRight) {                                          \
      if (node->getLayout().direction == FILTDirectionRTL) {              \
        return node->getLayout().instanceName[FILTEdgeStart];             \
      } else {                                                          \
        return node->getLayout().instanceName[FILTEdgeEnd];               \
      }                                                                 \
    }                                                                   \
                                                                        \
    return node->getLayout().instanceName[edge];                        \
  }

// FILT_NODE_PROPERTY_IMPL(void *, Context, context, context);
// FILT_NODE_PROPERTY_IMPL(FILTPrintFunc, PrintFunc, printFunc, print);
// FILT_NODE_PROPERTY_IMPL(bool, HasNewLayout, hasNewLayout, hasNewLayout);
// FILT_NODE_PROPERTY_IMPL(FILTNodeType, NodeType, nodeType, nodeType);

FILT_NODE_STYLE_PROPERTY_IMPL(FILTDirection, Direction, direction, direction);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTFlexDirection, FlexDirection, flexDirection, flexDirection);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTJustify, JustifyContent, justifyContent, justifyContent);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTAlign, AlignContent, alignContent, alignContent);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTAlign, AlignItems, alignItems, alignItems);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTAlign, AlignSelf, alignSelf, alignSelf);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTPositionType, PositionType, positionType, positionType);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTWrap, FlexWrap, flexWrap, flexWrap);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTOverflow, Overflow, overflow, overflow);
FILT_NODE_STYLE_PROPERTY_IMPL(FILTDisplay, Display, display, display);

FILT_NODE_STYLE_PROPERTY_IMPL(float, Flex, flex, flex);
FILT_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexGrow, flexGrow, flexGrow);
FILT_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexShrink, flexShrink, flexShrink);
FILT_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(FILTValue, FlexBasis, flexBasis, flexBasis);

FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(FILTValue, Position, position, position);
FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(FILTValue, Margin, margin, margin);
FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO_IMPL(FILTValue, Margin, margin);
FILT_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(FILTValue, Padding, padding, padding);
FILT_NODE_STYLE_EDGE_PROPERTY_IMPL(float, Border, border, border);

FILT_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(FILTValue, Width, width, dimensions[FILTDimensionWidth]);
FILT_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(FILTValue, Height, height, dimensions[FILTDimensionHeight]);
FILT_NODE_STYLE_PROPERTY_UNIT_IMPL(FILTValue, MinWidth, minWidth, minDimensions[FILTDimensionWidth]);
FILT_NODE_STYLE_PROPERTY_UNIT_IMPL(FILTValue, MinHeight, minHeight, minDimensions[FILTDimensionHeight]);
FILT_NODE_STYLE_PROPERTY_UNIT_IMPL(FILTValue, MaxWidth, maxWidth, maxDimensions[FILTDimensionWidth]);
FILT_NODE_STYLE_PROPERTY_UNIT_IMPL(FILTValue, MaxHeight, maxHeight, maxDimensions[FILTDimensionHeight]);

// Filates specific properties, not compatible with flexbox specification
FILT_NODE_STYLE_PROPERTY_IMPL(float, AspectRatio, aspectRatio, aspectRatio);

FILT_NODE_LAYOUT_PROPERTY_IMPL(float, Left, position[FILTEdgeLeft]);
FILT_NODE_LAYOUT_PROPERTY_IMPL(float, Top, position[FILTEdgeTop]);
FILT_NODE_LAYOUT_PROPERTY_IMPL(float, Right, position[FILTEdgeRight]);
FILT_NODE_LAYOUT_PROPERTY_IMPL(float, Bottom, position[FILTEdgeBottom]);
FILT_NODE_LAYOUT_PROPERTY_IMPL(float, Width, dimensions[FILTDimensionWidth]);
FILT_NODE_LAYOUT_PROPERTY_IMPL(float, Height, dimensions[FILTDimensionHeight]);
FILT_NODE_LAYOUT_PROPERTY_IMPL(FILTDirection, Direction, direction);
FILT_NODE_LAYOUT_PROPERTY_IMPL(bool, HadOverflow, hadOverflow);

FILT_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Margin, margin);
FILT_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Border, border);
FILT_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Padding, padding);

uint32_t gCurrentGenerationCount = 0;

bool FILTLayoutNodeInternal(const FILTNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const FILTDirection parentDirection,
                          const FILTMeasureMode widthMeasureMode,
                          const FILTMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason,
                          const FILTConfigRef config);

bool FILTFloatsEqualWithPrecision(const float a, const float b, const float precision) {
  assert(precision > 0);

  if (FILTFloatIsUndefined(a)) {
    return FILTFloatIsUndefined(b);
  }
  return fabs(a - b) < precision;
}

bool FILTFloatsEqual(const float a, const float b) {
  return FILTFloatsEqualWithPrecision(a, b, 0.0001f);
}

static void FILTNodePrintInternal(const FILTNodeRef node,
                                const FILTPrintOptions options) {
  std::string str;
  facebook::filates::FILTNodeToString(&str, node, options, 0);
  FILTLog(node, FILTLogLevelDebug, str.c_str());
}

void FILTNodePrint(const FILTNodeRef node, const FILTPrintOptions options) {
  FILTNodePrintInternal(node, options);
}

const std::array<FILTEdge, 4> leading = {
    {FILTEdgeTop, FILTEdgeBottom, FILTEdgeLeft, FILTEdgeRight}};

const std::array<FILTEdge, 4> trailing = {
    {FILTEdgeBottom, FILTEdgeTop, FILTEdgeRight, FILTEdgeLeft}};
static const std::array<FILTEdge, 4> pos = {{
    FILTEdgeTop,
    FILTEdgeBottom,
    FILTEdgeLeft,
    FILTEdgeRight,
}};

static const std::array<FILTDimension, 4> dim = {
    {FILTDimensionHeight, FILTDimensionHeight, FILTDimensionWidth, FILTDimensionWidth}};

static inline float FILTNodePaddingAndBorderForAxis(const FILTNodeRef node,
                                                  const FILTFlexDirection axis,
                                                  const float widthSize) {
  return node->getLeadingPaddingAndBorder(axis, widthSize) +
      node->getTrailingPaddingAndBorder(axis, widthSize);
}

static inline FILTAlign FILTNodeAlignItem(const FILTNodeRef node, const FILTNodeRef child) {
  const FILTAlign align = child->getStyle().alignSelf == FILTAlignAuto
      ? node->getStyle().alignItems
      : child->getStyle().alignSelf;
  if (align == FILTAlignBaseline &&
      FILTFlexDirectionIsColumn(node->getStyle().flexDirection)) {
    return FILTAlignFlexStart;
  }
  return align;
}

static float FILTBaseline(const FILTNodeRef node) {
  if (node->getBaseline() != nullptr) {
    const float baseline = node->getBaseline()(
        node,
        node->getLayout().measuredDimensions[FILTDimensionWidth],
        node->getLayout().measuredDimensions[FILTDimensionHeight]);
    FILTAssertWithNode(node,
                     !FILTFloatIsUndefined(baseline),
                     "Expect custom baseline function to not return NaN");
    return baseline;
  }

  FILTNodeRef baselineChild = nullptr;
  const uint32_t childCount = FILTNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const FILTNodeRef child = FILTNodeGetChild(node, i);
    if (child->getLineIndex() > 0) {
      break;
    }
    if (child->getStyle().positionType == FILTPositionTypeAbsolute) {
      continue;
    }
    if (FILTNodeAlignItem(node, child) == FILTAlignBaseline) {
      baselineChild = child;
      break;
    }

    if (baselineChild == nullptr) {
      baselineChild = child;
    }
  }

  if (baselineChild == nullptr) {
    return node->getLayout().measuredDimensions[FILTDimensionHeight];
  }

  const float baseline = FILTBaseline(baselineChild);
  return baseline + baselineChild->getLayout().position[FILTEdgeTop];
}

static bool FILTIsBaselineLayout(const FILTNodeRef node) {
  if (FILTFlexDirectionIsColumn(node->getStyle().flexDirection)) {
    return false;
  }
  if (node->getStyle().alignItems == FILTAlignBaseline) {
    return true;
  }
  const uint32_t childCount = FILTNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const FILTNodeRef child = FILTNodeGetChild(node, i);
    if (child->getStyle().positionType == FILTPositionTypeRelative &&
        child->getStyle().alignSelf == FILTAlignBaseline) {
      return true;
    }
  }

  return false;
}

static inline float FILTNodeDimWithMargin(const FILTNodeRef node,
                                        const FILTFlexDirection axis,
                                        const float widthSize) {
  return node->getLayout().measuredDimensions[dim[axis]] +
      node->getLeadingMargin(axis, widthSize) +
      node->getTrailingMargin(axis, widthSize);
}

static inline bool FILTNodeIsStyleDimDefined(const FILTNodeRef node,
                                           const FILTFlexDirection axis,
                                           const float parentSize) {
  return !(
      node->getResolvedDimension(dim[axis]).unit == FILTUnitAuto ||
      node->getResolvedDimension(dim[axis]).unit == FILTUnitUndefined ||
      (node->getResolvedDimension(dim[axis]).unit == FILTUnitPoint &&
       node->getResolvedDimension(dim[axis]).value < 0.0f) ||
      (node->getResolvedDimension(dim[axis]).unit == FILTUnitPercent &&
       (node->getResolvedDimension(dim[axis]).value < 0.0f ||
        FILTFloatIsUndefined(parentSize))));
}

static inline bool FILTNodeIsLayoutDimDefined(const FILTNodeRef node, const FILTFlexDirection axis) {
  const float value = node->getLayout().measuredDimensions[dim[axis]];
  return !FILTFloatIsUndefined(value) && value >= 0.0f;
}

static float FILTNodeBoundAxisWithinMinAndMax(const FILTNodeRef node,
                                            const FILTFlexDirection axis,
                                            const float value,
                                            const float axisSize) {
  float min = FILTUndefined;
  float max = FILTUndefined;

  if (FILTFlexDirectionIsColumn(axis)) {
    min = FILTResolveValue(
        node->getStyle().minDimensions[FILTDimensionHeight], axisSize);
    max = FILTResolveValue(
        node->getStyle().maxDimensions[FILTDimensionHeight], axisSize);
  } else if (FILTFlexDirectionIsRow(axis)) {
    min = FILTResolveValue(
        node->getStyle().minDimensions[FILTDimensionWidth], axisSize);
    max = FILTResolveValue(
        node->getStyle().maxDimensions[FILTDimensionWidth], axisSize);
  }

  float boundValue = value;

  if (!FILTFloatIsUndefined(max) && max >= 0.0f && boundValue > max) {
    boundValue = max;
  }

  if (!FILTFloatIsUndefined(min) && min >= 0.0f && boundValue < min) {
    boundValue = min;
  }

  return boundValue;
}

// Like FILTNodeBoundAxisWithinMinAndMax but also ensures that the value doesn't go
// below the
// padding and border amount.
static inline float FILTNodeBoundAxis(const FILTNodeRef node,
                                    const FILTFlexDirection axis,
                                    const float value,
                                    const float axisSize,
                                    const float widthSize) {
  return fmaxf(FILTNodeBoundAxisWithinMinAndMax(node, axis, value, axisSize),
               FILTNodePaddingAndBorderForAxis(node, axis, widthSize));
}

static void FILTNodeSetChildTrailingPosition(const FILTNodeRef node,
                                           const FILTNodeRef child,
                                           const FILTFlexDirection axis) {
  const float size = child->getLayout().measuredDimensions[dim[axis]];
  child->setLayoutPosition(
      node->getLayout().measuredDimensions[dim[axis]] - size -
          child->getLayout().position[pos[axis]],
      trailing[axis]);
}

static void FILTConstrainMaxSizeForMode(const FILTNodeRef node,
                                      const enum FILTFlexDirection axis,
                                      const float parentAxisSize,
                                      const float parentWidth,
                                      FILTMeasureMode *mode,
                                      float *size) {
  const float maxSize =
      FILTResolveValue(
          node->getStyle().maxDimensions[dim[axis]], parentAxisSize) +
      node->getMarginForAxis(axis, parentWidth);
  switch (*mode) {
    case FILTMeasureModeExactly:
    case FILTMeasureModeAtMost:
      *size = (FILTFloatIsUndefined(maxSize) || *size < maxSize) ? *size : maxSize;
      break;
    case FILTMeasureModeUndefined:
      if (!FILTFloatIsUndefined(maxSize)) {
        *mode = FILTMeasureModeAtMost;
        *size = maxSize;
      }
      break;
  }
}

static void FILTNodeComputeFlexBasisForChild(const FILTNodeRef node,
                                           const FILTNodeRef child,
                                           const float width,
                                           const FILTMeasureMode widthMode,
                                           const float height,
                                           const float parentWidth,
                                           const float parentHeight,
                                           const FILTMeasureMode heightMode,
                                           const FILTDirection direction,
                                           const FILTConfigRef config) {
  const FILTFlexDirection mainAxis =
      FILTResolveFlexDirection(node->getStyle().flexDirection, direction);
  const bool isMainAxisRow = FILTFlexDirectionIsRow(mainAxis);
  const float mainAxisSize = isMainAxisRow ? width : height;
  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;

  float childWidth;
  float childHeight;
  FILTMeasureMode childWidthMeasureMode;
  FILTMeasureMode childHeightMeasureMode;

  const float resolvedFlexBasis =
      FILTResolveValue(child->resolveFlexBasisPtr(), mainAxisParentSize);
  const bool isRowStyleDimDefined = FILTNodeIsStyleDimDefined(child, FILTFlexDirectionRow, parentWidth);
  const bool isColumnStyleDimDefined =
      FILTNodeIsStyleDimDefined(child, FILTFlexDirectionColumn, parentHeight);

  if (!FILTFloatIsUndefined(resolvedFlexBasis) && !FILTFloatIsUndefined(mainAxisSize)) {
    if (FILTFloatIsUndefined(child->getLayout().computedFlexBasis) ||
        (FILTConfigIsExperimentalFeatureEnabled(
             child->getConfig(), FILTExperimentalFeatureWebFlexBasis) &&
         child->getLayout().computedFlexBasisGeneration !=
             gCurrentGenerationCount)) {
      child->setLayoutComputedFlexBasis(fmaxf(
          resolvedFlexBasis,
          FILTNodePaddingAndBorderForAxis(child, mainAxis, parentWidth)));
    }
  } else if (isMainAxisRow && isRowStyleDimDefined) {
    // The width is definite, so use that as the flex basis.
    child->setLayoutComputedFlexBasis(fmaxf(
        FILTResolveValue(
            child->getResolvedDimension(FILTDimensionWidth), parentWidth),
        FILTNodePaddingAndBorderForAxis(child, FILTFlexDirectionRow, parentWidth)));
  } else if (!isMainAxisRow && isColumnStyleDimDefined) {
    // The height is definite, so use that as the flex basis.
    child->setLayoutComputedFlexBasis(fmaxf(
        FILTResolveValue(
            child->getResolvedDimension(FILTDimensionHeight), parentHeight),
        FILTNodePaddingAndBorderForAxis(
            child, FILTFlexDirectionColumn, parentWidth)));
  } else {
    // Compute the flex basis and hypothetical main size (i.e. the clamped
    // flex basis).
    childWidth = FILTUndefined;
    childHeight = FILTUndefined;
    childWidthMeasureMode = FILTMeasureModeUndefined;
    childHeightMeasureMode = FILTMeasureModeUndefined;

    const float marginRow =
        child->getMarginForAxis(FILTFlexDirectionRow, parentWidth);
    const float marginColumn =
        child->getMarginForAxis(FILTFlexDirectionColumn, parentWidth);

    if (isRowStyleDimDefined) {
      childWidth =
          FILTResolveValue(
              child->getResolvedDimension(FILTDimensionWidth), parentWidth) +
          marginRow;
      childWidthMeasureMode = FILTMeasureModeExactly;
    }
    if (isColumnStyleDimDefined) {
      childHeight =
          FILTResolveValue(
              child->getResolvedDimension(FILTDimensionHeight), parentHeight) +
          marginColumn;
      childHeightMeasureMode = FILTMeasureModeExactly;
    }

    // The W3C spec doesn't say anything about the 'overflow' property,
    // but all major browsers appear to implement the following logic.
    if ((!isMainAxisRow && node->getStyle().overflow == FILTOverflowScroll) ||
        node->getStyle().overflow != FILTOverflowScroll) {
      if (FILTFloatIsUndefined(childWidth) && !FILTFloatIsUndefined(width)) {
        childWidth = width;
        childWidthMeasureMode = FILTMeasureModeAtMost;
      }
    }

    if ((isMainAxisRow && node->getStyle().overflow == FILTOverflowScroll) ||
        node->getStyle().overflow != FILTOverflowScroll) {
      if (FILTFloatIsUndefined(childHeight) && !FILTFloatIsUndefined(height)) {
        childHeight = height;
        childHeightMeasureMode = FILTMeasureModeAtMost;
      }
    }

    if (!FILTFloatIsUndefined(child->getStyle().aspectRatio)) {
      if (!isMainAxisRow && childWidthMeasureMode == FILTMeasureModeExactly) {
        childHeight = marginColumn +
            (childWidth - marginRow) / child->getStyle().aspectRatio;
        childHeightMeasureMode = FILTMeasureModeExactly;
      } else if (isMainAxisRow && childHeightMeasureMode == FILTMeasureModeExactly) {
        childWidth = marginRow +
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
        childWidthMeasureMode = FILTMeasureModeExactly;
      }
    }

    // If child has no defined size in the cross axis and is set to stretch,
    // set the cross
    // axis to be measured exactly with the available inner width

    const bool hasExactWidth = !FILTFloatIsUndefined(width) && widthMode == FILTMeasureModeExactly;
    const bool childWidthStretch = FILTNodeAlignItem(node, child) == FILTAlignStretch &&
                                   childWidthMeasureMode != FILTMeasureModeExactly;
    if (!isMainAxisRow && !isRowStyleDimDefined && hasExactWidth && childWidthStretch) {
      childWidth = width;
      childWidthMeasureMode = FILTMeasureModeExactly;
      if (!FILTFloatIsUndefined(child->getStyle().aspectRatio)) {
        childHeight = (childWidth - marginRow) / child->getStyle().aspectRatio;
        childHeightMeasureMode = FILTMeasureModeExactly;
      }
    }

    const bool hasExactHeight = !FILTFloatIsUndefined(height) && heightMode == FILTMeasureModeExactly;
    const bool childHeightStretch = FILTNodeAlignItem(node, child) == FILTAlignStretch &&
                                    childHeightMeasureMode != FILTMeasureModeExactly;
    if (isMainAxisRow && !isColumnStyleDimDefined && hasExactHeight && childHeightStretch) {
      childHeight = height;
      childHeightMeasureMode = FILTMeasureModeExactly;

      if (!FILTFloatIsUndefined(child->getStyle().aspectRatio)) {
        childWidth =
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
        childWidthMeasureMode = FILTMeasureModeExactly;
      }
    }

    FILTConstrainMaxSizeForMode(
        child, FILTFlexDirectionRow, parentWidth, parentWidth, &childWidthMeasureMode, &childWidth);
    FILTConstrainMaxSizeForMode(child,
                              FILTFlexDirectionColumn,
                              parentHeight,
                              parentWidth,
                              &childHeightMeasureMode,
                              &childHeight);

    // Measure the child
    FILTLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         parentWidth,
                         parentHeight,
                         false,
                         "measure",
                         config);

    child->setLayoutComputedFlexBasis(fmaxf(
        child->getLayout().measuredDimensions[dim[mainAxis]],
        FILTNodePaddingAndBorderForAxis(child, mainAxis, parentWidth)));
  }
  child->setLayoutComputedFlexBasisGeneration(gCurrentGenerationCount);
}

static void FILTNodeAbsoluteLayoutChild(const FILTNodeRef node,
                                      const FILTNodeRef child,
                                      const float width,
                                      const FILTMeasureMode widthMode,
                                      const float height,
                                      const FILTDirection direction,
                                      const FILTConfigRef config) {
  const FILTFlexDirection mainAxis =
      FILTResolveFlexDirection(node->getStyle().flexDirection, direction);
  const FILTFlexDirection crossAxis = FILTFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = FILTFlexDirectionIsRow(mainAxis);

  float childWidth = FILTUndefined;
  float childHeight = FILTUndefined;
  FILTMeasureMode childWidthMeasureMode = FILTMeasureModeUndefined;
  FILTMeasureMode childHeightMeasureMode = FILTMeasureModeUndefined;

  const float marginRow = child->getMarginForAxis(FILTFlexDirectionRow, width);
  const float marginColumn =
      child->getMarginForAxis(FILTFlexDirectionColumn, width);

  if (FILTNodeIsStyleDimDefined(child, FILTFlexDirectionRow, width)) {
    childWidth =
        FILTResolveValue(child->getResolvedDimension(FILTDimensionWidth), width) +
        marginRow;
  } else {
    // If the child doesn't have a specified width, compute the width based
    // on the left/right
    // offsets if they're defined.
    if (child->isLeadingPositionDefined(FILTFlexDirectionRow) &&
        child->isTrailingPosDefined(FILTFlexDirectionRow)) {
      childWidth = node->getLayout().measuredDimensions[FILTDimensionWidth] -
          (node->getLeadingBorder(FILTFlexDirectionRow) +
           node->getTrailingBorder(FILTFlexDirectionRow)) -
          (child->getLeadingPosition(FILTFlexDirectionRow, width) +
           child->getTrailingPosition(FILTFlexDirectionRow, width));
      childWidth = FILTNodeBoundAxis(child, FILTFlexDirectionRow, childWidth, width, width);
    }
  }

  if (FILTNodeIsStyleDimDefined(child, FILTFlexDirectionColumn, height)) {
    childHeight =
        FILTResolveValue(child->getResolvedDimension(FILTDimensionHeight), height) +
        marginColumn;
  } else {
    // If the child doesn't have a specified height, compute the height
    // based on the top/bottom
    // offsets if they're defined.
    if (child->isLeadingPositionDefined(FILTFlexDirectionColumn) &&
        child->isTrailingPosDefined(FILTFlexDirectionColumn)) {
      childHeight = node->getLayout().measuredDimensions[FILTDimensionHeight] -
          (node->getLeadingBorder(FILTFlexDirectionColumn) +
           node->getTrailingBorder(FILTFlexDirectionColumn)) -
          (child->getLeadingPosition(FILTFlexDirectionColumn, height) +
           child->getTrailingPosition(FILTFlexDirectionColumn, height));
      childHeight = FILTNodeBoundAxis(child, FILTFlexDirectionColumn, childHeight, height, width);
    }
  }

  // Exactly one dimension needs to be defined for us to be able to do aspect ratio
  // calculation. One dimension being the anchor and the other being flexible.
  if (FILTFloatIsUndefined(childWidth) ^ FILTFloatIsUndefined(childHeight)) {
    if (!FILTFloatIsUndefined(child->getStyle().aspectRatio)) {
      if (FILTFloatIsUndefined(childWidth)) {
        childWidth = marginRow +
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
      } else if (FILTFloatIsUndefined(childHeight)) {
        childHeight = marginColumn +
            (childWidth - marginRow) / child->getStyle().aspectRatio;
      }
    }
  }

  // If we're still missing one or the other dimension, measure the content.
  if (FILTFloatIsUndefined(childWidth) || FILTFloatIsUndefined(childHeight)) {
    childWidthMeasureMode =
        FILTFloatIsUndefined(childWidth) ? FILTMeasureModeUndefined : FILTMeasureModeExactly;
    childHeightMeasureMode =
        FILTFloatIsUndefined(childHeight) ? FILTMeasureModeUndefined : FILTMeasureModeExactly;

    // If the size of the parent is defined then try to constrain the absolute child to that size
    // as well. This allows text within the absolute child to wrap to the size of its parent.
    // This is the same behavior as many browsers implement.
    if (!isMainAxisRow && FILTFloatIsUndefined(childWidth) && widthMode != FILTMeasureModeUndefined &&
        width > 0) {
      childWidth = width;
      childWidthMeasureMode = FILTMeasureModeAtMost;
    }

    FILTLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         childWidth,
                         childHeight,
                         false,
                         "abs-measure",
                         config);
    childWidth = child->getLayout().measuredDimensions[FILTDimensionWidth] +
        child->getMarginForAxis(FILTFlexDirectionRow, width);
    childHeight = child->getLayout().measuredDimensions[FILTDimensionHeight] +
        child->getMarginForAxis(FILTFlexDirectionColumn, width);
  }

  FILTLayoutNodeInternal(child,
                       childWidth,
                       childHeight,
                       direction,
                       FILTMeasureModeExactly,
                       FILTMeasureModeExactly,
                       childWidth,
                       childHeight,
                       true,
                       "abs-layout",
                       config);

  if (child->isTrailingPosDefined(mainAxis) &&
      !child->isLeadingPositionDefined(mainAxis)) {
    child->setLayoutPosition(
        node->getLayout().measuredDimensions[dim[mainAxis]] -
            child->getLayout().measuredDimensions[dim[mainAxis]] -
            node->getTrailingBorder(mainAxis) -
            child->getTrailingMargin(mainAxis, width) -
            child->getTrailingPosition(
                mainAxis, isMainAxisRow ? width : height),
        leading[mainAxis]);
  } else if (
      !child->isLeadingPositionDefined(mainAxis) &&
      node->getStyle().justifyContent == FILTJustifyCenter) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[mainAxis]] -
         child->getLayout().measuredDimensions[dim[mainAxis]]) /
            2.0f,
        leading[mainAxis]);
  } else if (
      !child->isLeadingPositionDefined(mainAxis) &&
      node->getStyle().justifyContent == FILTJustifyFlexEnd) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[mainAxis]] -
         child->getLayout().measuredDimensions[dim[mainAxis]]),
        leading[mainAxis]);
  }

  if (child->isTrailingPosDefined(crossAxis) &&
      !child->isLeadingPositionDefined(crossAxis)) {
    child->setLayoutPosition(
        node->getLayout().measuredDimensions[dim[crossAxis]] -
            child->getLayout().measuredDimensions[dim[crossAxis]] -
            node->getTrailingBorder(crossAxis) -
            child->getTrailingMargin(crossAxis, width) -
            child->getTrailingPosition(
                crossAxis, isMainAxisRow ? height : width),
        leading[crossAxis]);

  } else if (
      !child->isLeadingPositionDefined(crossAxis) &&
      FILTNodeAlignItem(node, child) == FILTAlignCenter) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[crossAxis]] -
         child->getLayout().measuredDimensions[dim[crossAxis]]) /
            2.0f,
        leading[crossAxis]);
  } else if (
      !child->isLeadingPositionDefined(crossAxis) &&
      ((FILTNodeAlignItem(node, child) == FILTAlignFlexEnd) ^
       (node->getStyle().flexWrap == FILTWrapWrapReverse))) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[crossAxis]] -
         child->getLayout().measuredDimensions[dim[crossAxis]]),
        leading[crossAxis]);
  }
}

static void FILTNodeWithMeasureFuncSetMeasuredDimensions(const FILTNodeRef node,
                                                       const float availableWidth,
                                                       const float availableHeight,
                                                       const FILTMeasureMode widthMeasureMode,
                                                       const FILTMeasureMode heightMeasureMode,
                                                       const float parentWidth,
                                                       const float parentHeight) {
  FILTAssertWithNode(
      node,
      node->getMeasure() != nullptr,
      "Expected node to have custom measure function");

  const float paddingAndBorderAxisRow =
      FILTNodePaddingAndBorderForAxis(node, FILTFlexDirectionRow, availableWidth);
  const float paddingAndBorderAxisColumn =
      FILTNodePaddingAndBorderForAxis(node, FILTFlexDirectionColumn, availableWidth);
  const float marginAxisRow =
      node->getMarginForAxis(FILTFlexDirectionRow, availableWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(FILTFlexDirectionColumn, availableWidth);

  // We want to make sure we don't call measure with negative size
  const float innerWidth = FILTFloatIsUndefined(availableWidth)
      ? availableWidth
      : fmaxf(0, availableWidth - marginAxisRow - paddingAndBorderAxisRow);
  const float innerHeight = FILTFloatIsUndefined(availableHeight)
      ? availableHeight
      : fmaxf(
            0, availableHeight - marginAxisColumn - paddingAndBorderAxisColumn);

  if (widthMeasureMode == FILTMeasureModeExactly &&
      heightMeasureMode == FILTMeasureModeExactly) {
    // Don't bother sizing the text if both dimensions are already defined.
    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            FILTFlexDirectionRow,
            availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        FILTDimensionWidth);
    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            FILTFlexDirectionColumn,
            availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
        FILTDimensionHeight);
  } else {
    // Measure the text under the current constraints.
    const FILTSize measuredSize = node->getMeasure()(
        node, innerWidth, widthMeasureMode, innerHeight, heightMeasureMode);

    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            FILTFlexDirectionRow,
            (widthMeasureMode == FILTMeasureModeUndefined ||
             widthMeasureMode == FILTMeasureModeAtMost)
                ? measuredSize.width + paddingAndBorderAxisRow
                : availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        FILTDimensionWidth);

    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            FILTFlexDirectionColumn,
            (heightMeasureMode == FILTMeasureModeUndefined ||
             heightMeasureMode == FILTMeasureModeAtMost)
                ? measuredSize.height + paddingAndBorderAxisColumn
                : availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
        FILTDimensionHeight);
  }
}

// For nodes with no children, use the available values if they were provided,
// or the minimum size as indicated by the padding and border sizes.
static void FILTNodeEmptyContainerSetMeasuredDimensions(const FILTNodeRef node,
                                                      const float availableWidth,
                                                      const float availableHeight,
                                                      const FILTMeasureMode widthMeasureMode,
                                                      const FILTMeasureMode heightMeasureMode,
                                                      const float parentWidth,
                                                      const float parentHeight) {
  const float paddingAndBorderAxisRow =
      FILTNodePaddingAndBorderForAxis(node, FILTFlexDirectionRow, parentWidth);
  const float paddingAndBorderAxisColumn =
      FILTNodePaddingAndBorderForAxis(node, FILTFlexDirectionColumn, parentWidth);
  const float marginAxisRow =
      node->getMarginForAxis(FILTFlexDirectionRow, parentWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(FILTFlexDirectionColumn, parentWidth);

  node->setLayoutMeasuredDimension(
      FILTNodeBoundAxis(
          node,
          FILTFlexDirectionRow,
          (widthMeasureMode == FILTMeasureModeUndefined ||
           widthMeasureMode == FILTMeasureModeAtMost)
              ? paddingAndBorderAxisRow
              : availableWidth - marginAxisRow,
          parentWidth,
          parentWidth),
      FILTDimensionWidth);

  node->setLayoutMeasuredDimension(
      FILTNodeBoundAxis(
          node,
          FILTFlexDirectionColumn,
          (heightMeasureMode == FILTMeasureModeUndefined ||
           heightMeasureMode == FILTMeasureModeAtMost)
              ? paddingAndBorderAxisColumn
              : availableHeight - marginAxisColumn,
          parentHeight,
          parentWidth),
      FILTDimensionHeight);
}

static bool FILTNodeFixedSizeSetMeasuredDimensions(const FILTNodeRef node,
                                                 const float availableWidth,
                                                 const float availableHeight,
                                                 const FILTMeasureMode widthMeasureMode,
                                                 const FILTMeasureMode heightMeasureMode,
                                                 const float parentWidth,
                                                 const float parentHeight) {
  if ((widthMeasureMode == FILTMeasureModeAtMost && availableWidth <= 0.0f) ||
      (heightMeasureMode == FILTMeasureModeAtMost && availableHeight <= 0.0f) ||
      (widthMeasureMode == FILTMeasureModeExactly && heightMeasureMode == FILTMeasureModeExactly)) {
    const float marginAxisColumn =
        node->getMarginForAxis(FILTFlexDirectionColumn, parentWidth);
    const float marginAxisRow =
        node->getMarginForAxis(FILTFlexDirectionRow, parentWidth);

    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            FILTFlexDirectionRow,
            FILTFloatIsUndefined(availableWidth) ||
                    (widthMeasureMode == FILTMeasureModeAtMost &&
                     availableWidth < 0.0f)
                ? 0.0f
                : availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        FILTDimensionWidth);

    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            FILTFlexDirectionColumn,
            FILTFloatIsUndefined(availableHeight) ||
                    (heightMeasureMode == FILTMeasureModeAtMost &&
                     availableHeight < 0.0f)
                ? 0.0f
                : availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
        FILTDimensionHeight);
    return true;
  }

  return false;
}

static void FILTZeroOutLayoutRecursivly(const FILTNodeRef node) {
  memset(&(node->getLayout()), 0, sizeof(FILTLayout));
  node->setHasNewLayout(true);
  node->cloneChildrenIfNeeded();
  const uint32_t childCount = FILTNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const FILTNodeRef child = node->getChild(i);
    FILTZeroOutLayoutRecursivly(child);
  }
}

static float FILTNodeCalculateAvailableInnerDim(
    const FILTNodeRef node,
    FILTFlexDirection axis,
    float availableDim,
    float parentDim) {
  FILTFlexDirection direction =
      FILTFlexDirectionIsRow(axis) ? FILTFlexDirectionRow : FILTFlexDirectionColumn;
  FILTDimension dimension =
      FILTFlexDirectionIsRow(axis) ? FILTDimensionWidth : FILTDimensionHeight;

  const float margin = node->getMarginForAxis(direction, parentDim);
  const float paddingAndBorder =
      FILTNodePaddingAndBorderForAxis(node, direction, parentDim);

  float availableInnerDim = availableDim - margin - paddingAndBorder;
  // Max dimension overrides predefined dimension value; Min dimension in turn
  // overrides both of the above
  if (!FILTFloatIsUndefined(availableInnerDim)) {
    // We want to make sure our available height does not violate min and max
    // constraints
    const float minInnerDim =
        FILTResolveValue(node->getStyle().minDimensions[dimension], parentDim) -
        paddingAndBorder;
    const float maxInnerDim =
        FILTResolveValue(node->getStyle().maxDimensions[dimension], parentDim) -
        paddingAndBorder;
    availableInnerDim =
        fmaxf(fminf(availableInnerDim, maxInnerDim), minInnerDim);
  }

  return availableInnerDim;
}

static void FILTNodeComputeFlexBasisForChildren(
    const FILTNodeRef node,
    const float availableInnerWidth,
    const float availableInnerHeight,
    FILTMeasureMode widthMeasureMode,
    FILTMeasureMode heightMeasureMode,
    FILTDirection direction,
    FILTFlexDirection mainAxis,
    const FILTConfigRef config,
    bool performLayout,
    float& totalOuterFlexBasis) {
  FILTNodeRef singleFlexChild = nullptr;
  FILTVector children = node->getChildren();
  FILTMeasureMode measureModeMainDim =
      FILTFlexDirectionIsRow(mainAxis) ? widthMeasureMode : heightMeasureMode;
  // If there is only one child with flexGrow + flexShrink it means we can set
  // the computedFlexBasis to 0 instead of measuring and shrinking / flexing the
  // child to exactly match the remaining space
  if (measureModeMainDim == FILTMeasureModeExactly) {
    for (auto child : children) {
      if (singleFlexChild != nullptr) {
        if (child->isNodeFlexible()) {
          // There is already a flexible child, abort
          singleFlexChild = nullptr;
          break;
        }
      } else if (
          child->resolveFlexGrow() > 0.0f &&
          child->resolveFlexShrink() > 0.0f) {
        singleFlexChild = child;
      }
    }
  }

  for (auto child : children) {
    child->resolveDimension();
    if (child->getStyle().display == FILTDisplayNone) {
      FILTZeroOutLayoutRecursivly(child);
      child->setHasNewLayout(true);
      child->setDirty(false);
      continue;
    }
    if (performLayout) {
      // Set the initial position (relative to the parent).
      const FILTDirection childDirection = child->resolveDirection(direction);
      const float mainDim = FILTFlexDirectionIsRow(mainAxis)
          ? availableInnerWidth
          : availableInnerHeight;
      const float crossDim = FILTFlexDirectionIsRow(mainAxis)
          ? availableInnerHeight
          : availableInnerWidth;
      child->setPosition(
          childDirection, mainDim, crossDim, availableInnerWidth);
    }

    if (child->getStyle().positionType == FILTPositionTypeAbsolute) {
      continue;
    }
    if (child == singleFlexChild) {
      child->setLayoutComputedFlexBasisGeneration(gCurrentGenerationCount);
      child->setLayoutComputedFlexBasis(0);
    } else {
      FILTNodeComputeFlexBasisForChild(
          node,
          child,
          availableInnerWidth,
          widthMeasureMode,
          availableInnerHeight,
          availableInnerWidth,
          availableInnerHeight,
          heightMeasureMode,
          direction,
          config);
    }

    totalOuterFlexBasis += child->getLayout().computedFlexBasis +
        child->getMarginForAxis(mainAxis, availableInnerWidth);
  }
}

// This function assumes that all the children of node have their
// computedFlexBasis properly computed(To do this use
// FILTNodeComputeFlexBasisForChildren function).
// This function calculates FILTCollectFlexItemsRowMeasurement
static FILTCollectFlexItemsRowValues FILTCalculateCollectFlexItemsRowValues(
    const FILTNodeRef& node,
    const FILTDirection parentDirection,
    const float mainAxisParentSize,
    const float availableInnerWidth,
    const float availableInnerMainDim,
    const uint32_t startOfLineIndex,
    const uint32_t lineCount) {
  FILTCollectFlexItemsRowValues flexAlgoRowMeasurement = {};
  flexAlgoRowMeasurement.relativeChildren.reserve(node->getChildren().size());

  float sizeConsumedOnCurrentLineIncludingMinConstraint = 0;
  const FILTFlexDirection mainAxis = FILTResolveFlexDirection(
      node->getStyle().flexDirection, node->resolveDirection(parentDirection));
  const bool isNodeFlexWrap = node->getStyle().flexWrap != FILTWrapNoWrap;

  // Add items to the current line until it's full or we run out of items.
  uint32_t endOfLineIndex = startOfLineIndex;
  for (; endOfLineIndex < node->getChildrenCount(); endOfLineIndex++) {
    const FILTNodeRef child = node->getChild(endOfLineIndex);
    if (child->getStyle().display == FILTDisplayNone ||
        child->getStyle().positionType == FILTPositionTypeAbsolute) {
      continue;
    }
    child->setLineIndex(lineCount);
    const float childMarginMainAxis =
        child->getMarginForAxis(mainAxis, availableInnerWidth);
    const float flexBasisWithMinAndMaxConstraints =
        FILTNodeBoundAxisWithinMinAndMax(
            child,
            mainAxis,
            child->getLayout().computedFlexBasis,
            mainAxisParentSize);

    // If this is a multi-line flow and this item pushes us over the
    // available size, we've
    // hit the end of the current line. Break out of the loop and lay out
    // the current line.
    if (sizeConsumedOnCurrentLineIncludingMinConstraint +
                flexBasisWithMinAndMaxConstraints + childMarginMainAxis >
            availableInnerMainDim &&
        isNodeFlexWrap && flexAlgoRowMeasurement.itemsOnLine > 0) {
      break;
    }

    sizeConsumedOnCurrentLineIncludingMinConstraint +=
        flexBasisWithMinAndMaxConstraints + childMarginMainAxis;
    flexAlgoRowMeasurement.sizeConsumedOnCurrentLine +=
        flexBasisWithMinAndMaxConstraints + childMarginMainAxis;
    flexAlgoRowMeasurement.itemsOnLine++;

    if (child->isNodeFlexible()) {
      flexAlgoRowMeasurement.totalFlexGrowFactors += child->resolveFlexGrow();

      // Unlike the grow factor, the shrink factor is scaled relative to the
      // child dimension.
      flexAlgoRowMeasurement.totalFlexShrinkScaledFactors +=
          -child->resolveFlexShrink() * child->getLayout().computedFlexBasis;
    }

    flexAlgoRowMeasurement.relativeChildren.push_back(child);
  }

  // The total flex factor needs to be floored to 1.
  if (flexAlgoRowMeasurement.totalFlexGrowFactors > 0 &&
      flexAlgoRowMeasurement.totalFlexGrowFactors < 1) {
    flexAlgoRowMeasurement.totalFlexGrowFactors = 1;
  }

  // The total flex shrink factor needs to be floored to 1.
  if (flexAlgoRowMeasurement.totalFlexShrinkScaledFactors > 0 &&
      flexAlgoRowMeasurement.totalFlexShrinkScaledFactors < 1) {
    flexAlgoRowMeasurement.totalFlexShrinkScaledFactors = 1;
  }
  flexAlgoRowMeasurement.endOfLineIndex = endOfLineIndex;
  return flexAlgoRowMeasurement;
}

// It distributes the free space to the flexible items and ensures that the size
// of the flex items abide the min and max constraints. At the end of this
// function the child nodes would have proper size. Prior using this function
// please ensure that FILTDistributeFreeSpaceFirstPass is called.
static float FILTDistributeFreeSpaceSecondPass(
    FILTCollectFlexItemsRowValues& collectedFlexItemsValues,
    const FILTNodeRef node,
    const FILTFlexDirection mainAxis,
    const FILTFlexDirection crossAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerCrossDim,
    const float availableInnerWidth,
    const float availableInnerHeight,
    const bool flexBasisOverflows,
    const FILTMeasureMode measureModeCrossDim,
    const bool performLayout,
    const FILTConfigRef config) {
  float childFlexBasis = 0;
  float flexShrinkScaledFactor = 0;
  float flexGrowFactor = 0;
  float deltaFreeSpace = 0;
  const bool isMainAxisRow = FILTFlexDirectionIsRow(mainAxis);
  const bool isNodeFlexWrap = node->getStyle().flexWrap != FILTWrapNoWrap;

  for (auto currentRelativeChild : collectedFlexItemsValues.relativeChildren) {
    childFlexBasis = FILTNodeBoundAxisWithinMinAndMax(
        currentRelativeChild,
        mainAxis,
        currentRelativeChild->getLayout().computedFlexBasis,
        mainAxisParentSize);
    float updatedMainSize = childFlexBasis;

    if (collectedFlexItemsValues.remainingFreeSpace < 0) {
      flexShrinkScaledFactor =
          -currentRelativeChild->resolveFlexShrink() * childFlexBasis;
      // Is this child able to shrink?
      if (flexShrinkScaledFactor != 0) {
        float childSize;

        if (collectedFlexItemsValues.totalFlexShrinkScaledFactors == 0) {
          childSize = childFlexBasis + flexShrinkScaledFactor;
        } else {
          childSize = childFlexBasis +
              (collectedFlexItemsValues.remainingFreeSpace /
               collectedFlexItemsValues.totalFlexShrinkScaledFactors) *
                  flexShrinkScaledFactor;
        }

        updatedMainSize = FILTNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            childSize,
            availableInnerMainDim,
            availableInnerWidth);
      }
    } else if (collectedFlexItemsValues.remainingFreeSpace > 0) {
      flexGrowFactor = currentRelativeChild->resolveFlexGrow();

      // Is this child able to grow?
      if (flexGrowFactor != 0) {
        updatedMainSize = FILTNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            childFlexBasis +
                collectedFlexItemsValues.remainingFreeSpace /
                    collectedFlexItemsValues.totalFlexGrowFactors *
                    flexGrowFactor,
            availableInnerMainDim,
            availableInnerWidth);
      }
    }

    deltaFreeSpace += updatedMainSize - childFlexBasis;

    const float marginMain =
        currentRelativeChild->getMarginForAxis(mainAxis, availableInnerWidth);
    const float marginCross =
        currentRelativeChild->getMarginForAxis(crossAxis, availableInnerWidth);

    float childCrossSize;
    float childMainSize = updatedMainSize + marginMain;
    FILTMeasureMode childCrossMeasureMode;
    FILTMeasureMode childMainMeasureMode = FILTMeasureModeExactly;

    if (!FILTFloatIsUndefined(currentRelativeChild->getStyle().aspectRatio)) {
      childCrossSize = isMainAxisRow ? (childMainSize - marginMain) /
              currentRelativeChild->getStyle().aspectRatio
                                     : (childMainSize - marginMain) *
              currentRelativeChild->getStyle().aspectRatio;
      childCrossMeasureMode = FILTMeasureModeExactly;

      childCrossSize += marginCross;
    } else if (
        !FILTFloatIsUndefined(availableInnerCrossDim) &&
        !FILTNodeIsStyleDimDefined(
            currentRelativeChild, crossAxis, availableInnerCrossDim) &&
        measureModeCrossDim == FILTMeasureModeExactly &&
        !(isNodeFlexWrap && flexBasisOverflows) &&
        FILTNodeAlignItem(node, currentRelativeChild) == FILTAlignStretch &&
        currentRelativeChild->marginLeadingValue(crossAxis).unit !=
            FILTUnitAuto &&
        currentRelativeChild->marginTrailingValue(crossAxis).unit !=
            FILTUnitAuto) {
      childCrossSize = availableInnerCrossDim;
      childCrossMeasureMode = FILTMeasureModeExactly;
    } else if (!FILTNodeIsStyleDimDefined(
                   currentRelativeChild, crossAxis, availableInnerCrossDim)) {
      childCrossSize = availableInnerCrossDim;
      childCrossMeasureMode = FILTFloatIsUndefined(childCrossSize)
          ? FILTMeasureModeUndefined
          : FILTMeasureModeAtMost;
    } else {
      childCrossSize =
          FILTResolveValue(
              currentRelativeChild->getResolvedDimension(dim[crossAxis]),
              availableInnerCrossDim) +
          marginCross;
      const bool isLoosePercentageMeasurement =
          currentRelativeChild->getResolvedDimension(dim[crossAxis]).unit ==
              FILTUnitPercent &&
          measureModeCrossDim != FILTMeasureModeExactly;
      childCrossMeasureMode =
          FILTFloatIsUndefined(childCrossSize) || isLoosePercentageMeasurement
          ? FILTMeasureModeUndefined
          : FILTMeasureModeExactly;
    }

    FILTConstrainMaxSizeForMode(
        currentRelativeChild,
        mainAxis,
        availableInnerMainDim,
        availableInnerWidth,
        &childMainMeasureMode,
        &childMainSize);
    FILTConstrainMaxSizeForMode(
        currentRelativeChild,
        crossAxis,
        availableInnerCrossDim,
        availableInnerWidth,
        &childCrossMeasureMode,
        &childCrossSize);

    const bool requiresStretchLayout =
        !FILTNodeIsStyleDimDefined(
            currentRelativeChild, crossAxis, availableInnerCrossDim) &&
        FILTNodeAlignItem(node, currentRelativeChild) == FILTAlignStretch &&
        currentRelativeChild->marginLeadingValue(crossAxis).unit !=
            FILTUnitAuto &&
        currentRelativeChild->marginTrailingValue(crossAxis).unit != FILTUnitAuto;

    const float childWidth = isMainAxisRow ? childMainSize : childCrossSize;
    const float childHeight = !isMainAxisRow ? childMainSize : childCrossSize;

    const FILTMeasureMode childWidthMeasureMode =
        isMainAxisRow ? childMainMeasureMode : childCrossMeasureMode;
    const FILTMeasureMode childHeightMeasureMode =
        !isMainAxisRow ? childMainMeasureMode : childCrossMeasureMode;

    // Recursively call the layout algorithm for this child with the updated
    // main size.
    FILTLayoutNodeInternal(
        currentRelativeChild,
        childWidth,
        childHeight,
        node->getLayout().direction,
        childWidthMeasureMode,
        childHeightMeasureMode,
        availableInnerWidth,
        availableInnerHeight,
        performLayout && !requiresStretchLayout,
        "flex",
        config);
    node->setLayoutHadOverflow(
        node->getLayout().hadOverflow |
        currentRelativeChild->getLayout().hadOverflow);
  }
  return deltaFreeSpace;
}

// It distributes the free space to the flexible items.For those flexible items
// whose min and max constraints are triggered, those flex item's clamped size
// is removed from the remaingfreespace.
static void FILTDistributeFreeSpaceFirstPass(
    FILTCollectFlexItemsRowValues& collectedFlexItemsValues,
    const FILTFlexDirection mainAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerWidth) {
  float flexShrinkScaledFactor = 0;
  float flexGrowFactor = 0;
  float baseMainSize = 0;
  float boundMainSize = 0;
  float deltaFreeSpace = 0;

  for (auto currentRelativeChild : collectedFlexItemsValues.relativeChildren) {
    float childFlexBasis = FILTNodeBoundAxisWithinMinAndMax(
        currentRelativeChild,
        mainAxis,
        currentRelativeChild->getLayout().computedFlexBasis,
        mainAxisParentSize);

    if (collectedFlexItemsValues.remainingFreeSpace < 0) {
      flexShrinkScaledFactor =
          -currentRelativeChild->resolveFlexShrink() * childFlexBasis;

      // Is this child able to shrink?
      if (flexShrinkScaledFactor != 0) {
        baseMainSize = childFlexBasis +
            collectedFlexItemsValues.remainingFreeSpace /
                collectedFlexItemsValues.totalFlexShrinkScaledFactors *
                flexShrinkScaledFactor;
        boundMainSize = FILTNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            baseMainSize,
            availableInnerMainDim,
            availableInnerWidth);
        if (baseMainSize != boundMainSize) {
          // By excluding this item's size and flex factor from remaining,
          // this item's
          // min/max constraints should also trigger in the second pass
          // resulting in the
          // item's size calculation being identical in the first and second
          // passes.
          deltaFreeSpace += boundMainSize - childFlexBasis;
          collectedFlexItemsValues.totalFlexShrinkScaledFactors -=
              flexShrinkScaledFactor;
        }
      }
    } else if (collectedFlexItemsValues.remainingFreeSpace > 0) {
      flexGrowFactor = currentRelativeChild->resolveFlexGrow();

      // Is this child able to grow?
      if (flexGrowFactor != 0) {
        baseMainSize = childFlexBasis +
            collectedFlexItemsValues.remainingFreeSpace /
                collectedFlexItemsValues.totalFlexGrowFactors * flexGrowFactor;
        boundMainSize = FILTNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            baseMainSize,
            availableInnerMainDim,
            availableInnerWidth);

        if (baseMainSize != boundMainSize) {
          // By excluding this item's size and flex factor from remaining,
          // this item's
          // min/max constraints should also trigger in the second pass
          // resulting in the
          // item's size calculation being identical in the first and second
          // passes.
          deltaFreeSpace += boundMainSize - childFlexBasis;
          collectedFlexItemsValues.totalFlexGrowFactors -= flexGrowFactor;
        }
      }
    }
  }
  collectedFlexItemsValues.remainingFreeSpace -= deltaFreeSpace;
}

// Do two passes over the flex items to figure out how to distribute the
// remaining space.
// The first pass finds the items whose min/max constraints trigger,
// freezes them at those
// sizes, and excludes those sizes from the remaining space. The second
// pass sets the size
// of each flexible item. It distributes the remaining space amongst the
// items whose min/max
// constraints didn't trigger in pass 1. For the other items, it sets
// their sizes by forcing
// their min/max constraints to trigger again.
//
// This two pass approach for resolving min/max constraints deviates from
// the spec. The
// spec (https://www.w3.org/TR/FILT-flexbox-1/#resolve-flexible-lengths)
// describes a process
// that needs to be repeated a variable number of times. The algorithm
// implemented here
// won't handle all cases but it was simpler to implement and it mitigates
// performance
// concerns because we know exactly how many passes it'll do.
//
// At the end of this function the child nodes would have the proper size
// assigned to them.
//
static void FILTResolveFlexibleLength(
    const FILTNodeRef node,
    FILTCollectFlexItemsRowValues& collectedFlexItemsValues,
    const FILTFlexDirection mainAxis,
    const FILTFlexDirection crossAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerCrossDim,
    const float availableInnerWidth,
    const float availableInnerHeight,
    const bool flexBasisOverflows,
    const FILTMeasureMode measureModeCrossDim,
    const bool performLayout,
    const FILTConfigRef config) {
  const float originalFreeSpace = collectedFlexItemsValues.remainingFreeSpace;
  // First pass: detect the flex items whose min/max constraints trigger
  FILTDistributeFreeSpaceFirstPass(
      collectedFlexItemsValues,
      mainAxis,
      mainAxisParentSize,
      availableInnerMainDim,
      availableInnerWidth);

  // Second pass: resolve the sizes of the flexible items
  const float distributedFreeSpace = FILTDistributeFreeSpaceSecondPass(
      collectedFlexItemsValues,
      node,
      mainAxis,
      crossAxis,
      mainAxisParentSize,
      availableInnerMainDim,
      availableInnerCrossDim,
      availableInnerWidth,
      availableInnerHeight,
      flexBasisOverflows,
      measureModeCrossDim,
      performLayout,
      config);

  collectedFlexItemsValues.remainingFreeSpace =
      originalFreeSpace - distributedFreeSpace;
}

static void FILTJustifyMainAxis(
    const FILTNodeRef node,
    FILTCollectFlexItemsRowValues& collectedFlexItemsValues,
    const uint32_t& startOfLineIndex,
    const FILTFlexDirection& mainAxis,
    const FILTFlexDirection& crossAxis,
    const FILTMeasureMode& measureModeMainDim,
    const FILTMeasureMode& measureModeCrossDim,
    const float& mainAxisParentSize,
    const float& parentWidth,
    const float& availableInnerMainDim,
    const float& availableInnerCrossDim,
    const float& availableInnerWidth,
    const bool& performLayout) {
  const FILTStyle style = node->getStyle();

  // If we are using "at most" rules in the main axis. Calculate the remaining
  // space when constraint by the min size defined for the main axis.
  if (measureModeMainDim == FILTMeasureModeAtMost &&
      collectedFlexItemsValues.remainingFreeSpace > 0) {
    if (style.minDimensions[dim[mainAxis]].unit != FILTUnitUndefined &&
        FILTResolveValue(
            style.minDimensions[dim[mainAxis]], mainAxisParentSize) >= 0) {
      collectedFlexItemsValues.remainingFreeSpace = fmaxf(
          0,
          FILTResolveValue(
              style.minDimensions[dim[mainAxis]], mainAxisParentSize) -
              (availableInnerMainDim -
               collectedFlexItemsValues.remainingFreeSpace));
    } else {
      collectedFlexItemsValues.remainingFreeSpace = 0;
    }
  }

  int numberOfAutoMarginsOnCurrentLine = 0;
  for (uint32_t i = startOfLineIndex;
       i < collectedFlexItemsValues.endOfLineIndex;
       i++) {
    const FILTNodeRef child = node->getChild(i);
    if (child->getStyle().positionType == FILTPositionTypeRelative) {
      if (child->marginLeadingValue(mainAxis).unit == FILTUnitAuto) {
        numberOfAutoMarginsOnCurrentLine++;
      }
      if (child->marginTrailingValue(mainAxis).unit == FILTUnitAuto) {
        numberOfAutoMarginsOnCurrentLine++;
      }
    }
  }

  // In order to position the elements in the main axis, we have two
  // controls. The space between the beginning and the first element
  // and the space between each two elements.
  float leadingMainDim = 0;
  float betweenMainDim = 0;
  const FILTJustify justifyContent = node->getStyle().justifyContent;

  if (numberOfAutoMarginsOnCurrentLine == 0) {
    switch (justifyContent) {
      case FILTJustifyCenter:
        leadingMainDim = collectedFlexItemsValues.remainingFreeSpace / 2;
        break;
      case FILTJustifyFlexEnd:
        leadingMainDim = collectedFlexItemsValues.remainingFreeSpace;
        break;
      case FILTJustifySpaceBetween:
        if (collectedFlexItemsValues.itemsOnLine > 1) {
          betweenMainDim =
              fmaxf(collectedFlexItemsValues.remainingFreeSpace, 0) /
              (collectedFlexItemsValues.itemsOnLine - 1);
        } else {
          betweenMainDim = 0;
        }
        break;
      case FILTJustifySpaceEvenly:
        // Space is distributed evenly across all elements
        betweenMainDim = collectedFlexItemsValues.remainingFreeSpace /
            (collectedFlexItemsValues.itemsOnLine + 1);
        leadingMainDim = betweenMainDim;
        break;
      case FILTJustifySpaceAround:
        // Space on the edges is half of the space between elements
        betweenMainDim = collectedFlexItemsValues.remainingFreeSpace /
            collectedFlexItemsValues.itemsOnLine;
        leadingMainDim = betweenMainDim / 2;
        break;
      case FILTJustifyFlexStart:
        break;
    }
  }

  const float leadingPaddingAndBorderMain =
      node->getLeadingPaddingAndBorder(mainAxis, parentWidth);
  collectedFlexItemsValues.mainDim =
      leadingPaddingAndBorderMain + leadingMainDim;
  collectedFlexItemsValues.crossDim = 0;

  for (uint32_t i = startOfLineIndex;
       i < collectedFlexItemsValues.endOfLineIndex;
       i++) {
    const FILTNodeRef child = node->getChild(i);
    const FILTStyle childStyle = child->getStyle();
    const FILTLayout childLayout = child->getLayout();
    if (childStyle.display == FILTDisplayNone) {
      continue;
    }
    if (childStyle.positionType == FILTPositionTypeAbsolute &&
        child->isLeadingPositionDefined(mainAxis)) {
      if (performLayout) {
        // In case the child is position absolute and has left/top being
        // defined, we override the position to whatever the user said
        // (and margin/border).
        child->setLayoutPosition(
            child->getLeadingPosition(mainAxis, availableInnerMainDim) +
                node->getLeadingBorder(mainAxis) +
                child->getLeadingMargin(mainAxis, availableInnerWidth),
            pos[mainAxis]);
      }
    } else {
      // Now that we placed the element, we need to update the variables.
      // We need to do that only for relative elements. Absolute elements
      // do not take part in that phase.
      if (childStyle.positionType == FILTPositionTypeRelative) {
        if (child->marginLeadingValue(mainAxis).unit == FILTUnitAuto) {
          collectedFlexItemsValues.mainDim +=
              collectedFlexItemsValues.remainingFreeSpace /
              numberOfAutoMarginsOnCurrentLine;
        }

        if (performLayout) {
          child->setLayoutPosition(
              childLayout.position[pos[mainAxis]] +
                  collectedFlexItemsValues.mainDim,
              pos[mainAxis]);
        }

        if (child->marginTrailingValue(mainAxis).unit == FILTUnitAuto) {
          collectedFlexItemsValues.mainDim +=
              collectedFlexItemsValues.remainingFreeSpace /
              numberOfAutoMarginsOnCurrentLine;
        }
        bool canSkipFlex =
            !performLayout && measureModeCrossDim == FILTMeasureModeExactly;
        if (canSkipFlex) {
          // If we skipped the flex step, then we can't rely on the
          // measuredDims because
          // they weren't computed. This means we can't call
          // FILTNodeDimWithMargin.
          collectedFlexItemsValues.mainDim += betweenMainDim +
              child->getMarginForAxis(mainAxis, availableInnerWidth) +
              childLayout.computedFlexBasis;
          collectedFlexItemsValues.crossDim = availableInnerCrossDim;
        } else {
          // The main dimension is the sum of all the elements dimension plus
          // the spacing.
          collectedFlexItemsValues.mainDim += betweenMainDim +
              FILTNodeDimWithMargin(child, mainAxis, availableInnerWidth);

          // The cross dimension is the max of the elements dimension since
          // there can only be one element in that cross dimension.
          collectedFlexItemsValues.crossDim = fmaxf(
              collectedFlexItemsValues.crossDim,
              FILTNodeDimWithMargin(child, crossAxis, availableInnerWidth));
        }
      } else if (performLayout) {
        child->setLayoutPosition(
            childLayout.position[pos[mainAxis]] +
                node->getLeadingBorder(mainAxis) + leadingMainDim,
            pos[mainAxis]);
      }
    }
  }
  collectedFlexItemsValues.mainDim +=
      node->getTrailingPaddingAndBorder(mainAxis, parentWidth);
}

//
// This is the main routine that implements a subset of the flexbox layout
// algorithm
// described in the W3C FILT documentation: https://www.w3.org/TR/FILT3-flexbox/.
//
// Limitations of this algorithm, compared to the full standard:
//  * Display property is always assumed to be 'flex' except for Text nodes,
//  which
//    are assumed to be 'inline-flex'.
//  * The 'zIndex' property (or any form of z ordering) is not supported. Nodes
//  are
//    stacked in document order.
//  * The 'order' property is not supported. The order of flex items is always
//  defined
//    by document order.
//  * The 'visibility' property is always assumed to be 'visible'. Values of
//  'collapse'
//    and 'hidden' are not supported.
//  * There is no support for forced breaks.
//  * It does not support vertical inline directions (top-to-bottom or
//  bottom-to-top text).
//
// Deviations from standard:
//  * Section 4.5 of the spec indicates that all flex items have a default
//  minimum
//    main size. For text blocks, for example, this is the width of the widest
//    word.
//    Calculating the minimum width is expensive, so we forego it and assume a
//    default
//    minimum main size of 0.
//  * Min/Max sizes in the main axis are not honored when resolving flexible
//  lengths.
//  * The spec indicates that the default value for 'flexDirection' is 'row',
//  but
//    the algorithm below assumes a default of 'column'.
//
// Input parameters:
//    - node: current node to be sized and layed out
//    - availableWidth & availableHeight: available size to be used for sizing
//    the node
//      or FILTUndefined if the size is not available; interpretation depends on
//      layout
//      flags
//    - parentDirection: the inline (text) direction within the parent
//    (left-to-right or
//      right-to-left)
//    - widthMeasureMode: indicates the sizing rules for the width (see below
//    for explanation)
//    - heightMeasureMode: indicates the sizing rules for the height (see below
//    for explanation)
//    - performLayout: specifies whether the caller is interested in just the
//    dimensions
//      of the node or it requires the entire node and its subtree to be layed
//      out
//      (with final positions)
//
// Details:
//    This routine is called recursively to lay out subtrees of flexbox
//    elements. It uses the
//    information in node.style, which is treated as a read-only input. It is
//    responsible for
//    setting the layout.direction and layout.measuredDimensions fields for the
//    input node as well
//    as the layout.position and layout.lineIndex fields for its child nodes.
//    The
//    layout.measuredDimensions field includes any border or padding for the
//    node but does
//    not include margins.
//
//    The spec describes four different layout modes: "fill available", "max
//    content", "min
//    content",
//    and "fit content". Of these, we don't use "min content" because we don't
//    support default
//    minimum main sizes (see above for details). Each of our measure modes maps
//    to a layout mode
//    from the spec (https://www.w3.org/TR/FILT3-sizing/#terms):
//      - FILTMeasureModeUndefined: max content
//      - FILTMeasureModeExactly: fill available
//      - FILTMeasureModeAtMost: fit content
//
//    When calling FILTNodelayoutImpl and FILTLayoutNodeInternal, if the caller passes
//    an available size of
//    undefined then it must also pass a measure mode of FILTMeasureModeUndefined
//    in that dimension.
//
static void FILTNodelayoutImpl(const FILTNodeRef node,
                             const float availableWidth,
                             const float availableHeight,
                             const FILTDirection parentDirection,
                             const FILTMeasureMode widthMeasureMode,
                             const FILTMeasureMode heightMeasureMode,
                             const float parentWidth,
                             const float parentHeight,
                             const bool performLayout,
                             const FILTConfigRef config) {
  FILTAssertWithNode(node,
                   FILTFloatIsUndefined(availableWidth) ? widthMeasureMode == FILTMeasureModeUndefined
                                                      : true,
                   "availableWidth is indefinite so widthMeasureMode must be "
                   "FILTMeasureModeUndefined");
  FILTAssertWithNode(node,
                   FILTFloatIsUndefined(availableHeight) ? heightMeasureMode == FILTMeasureModeUndefined
                                                       : true,
                   "availableHeight is indefinite so heightMeasureMode must be "
                   "FILTMeasureModeUndefined");

  // Set the resolved resolution in the node's layout.
  const FILTDirection direction = node->resolveDirection(parentDirection);
  node->setLayoutDirection(direction);

  const FILTFlexDirection flexRowDirection = FILTResolveFlexDirection(FILTFlexDirectionRow, direction);
  const FILTFlexDirection flexColumnDirection =
      FILTResolveFlexDirection(FILTFlexDirectionColumn, direction);

  node->setLayoutMargin(
      node->getLeadingMargin(flexRowDirection, parentWidth), FILTEdgeStart);
  node->setLayoutMargin(
      node->getTrailingMargin(flexRowDirection, parentWidth), FILTEdgeEnd);
  node->setLayoutMargin(
      node->getLeadingMargin(flexColumnDirection, parentWidth), FILTEdgeTop);
  node->setLayoutMargin(
      node->getTrailingMargin(flexColumnDirection, parentWidth), FILTEdgeBottom);

  node->setLayoutBorder(node->getLeadingBorder(flexRowDirection), FILTEdgeStart);
  node->setLayoutBorder(node->getTrailingBorder(flexRowDirection), FILTEdgeEnd);
  node->setLayoutBorder(node->getLeadingBorder(flexColumnDirection), FILTEdgeTop);
  node->setLayoutBorder(
      node->getTrailingBorder(flexColumnDirection), FILTEdgeBottom);

  node->setLayoutPadding(
      node->getLeadingPadding(flexRowDirection, parentWidth), FILTEdgeStart);
  node->setLayoutPadding(
      node->getTrailingPadding(flexRowDirection, parentWidth), FILTEdgeEnd);
  node->setLayoutPadding(
      node->getLeadingPadding(flexColumnDirection, parentWidth), FILTEdgeTop);
  node->setLayoutPadding(
      node->getTrailingPadding(flexColumnDirection, parentWidth), FILTEdgeBottom);

  if (node->getMeasure() != nullptr) {
    FILTNodeWithMeasureFuncSetMeasuredDimensions(node,
                                               availableWidth,
                                               availableHeight,
                                               widthMeasureMode,
                                               heightMeasureMode,
                                               parentWidth,
                                               parentHeight);
    return;
  }

  const uint32_t childCount = FILTNodeGetChildCount(node);
  if (childCount == 0) {
    FILTNodeEmptyContainerSetMeasuredDimensions(node,
                                              availableWidth,
                                              availableHeight,
                                              widthMeasureMode,
                                              heightMeasureMode,
                                              parentWidth,
                                              parentHeight);
    return;
  }

  // If we're not being asked to perform a full layout we can skip the algorithm if we already know
  // the size
  if (!performLayout && FILTNodeFixedSizeSetMeasuredDimensions(node,
                                                             availableWidth,
                                                             availableHeight,
                                                             widthMeasureMode,
                                                             heightMeasureMode,
                                                             parentWidth,
                                                             parentHeight)) {
    return;
  }

  // At this point we know we're going to perform work. Ensure that each child has a mutable copy.
  node->cloneChildrenIfNeeded();
  // Reset layout flags, as they could have changed.
  node->setLayoutHadOverflow(false);

  // STEP 1: CALCULATE VALUES FOR REMAINDER OF ALGORITHM
  const FILTFlexDirection mainAxis =
      FILTResolveFlexDirection(node->getStyle().flexDirection, direction);
  const FILTFlexDirection crossAxis = FILTFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = FILTFlexDirectionIsRow(mainAxis);
  const bool isNodeFlexWrap = node->getStyle().flexWrap != FILTWrapNoWrap;

  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;
  const float crossAxisParentSize = isMainAxisRow ? parentHeight : parentWidth;

  const float leadingPaddingAndBorderCross =
      node->getLeadingPaddingAndBorder(crossAxis, parentWidth);
  const float paddingAndBorderAxisMain = FILTNodePaddingAndBorderForAxis(node, mainAxis, parentWidth);
  const float paddingAndBorderAxisCross =
      FILTNodePaddingAndBorderForAxis(node, crossAxis, parentWidth);

  FILTMeasureMode measureModeMainDim = isMainAxisRow ? widthMeasureMode : heightMeasureMode;
  FILTMeasureMode measureModeCrossDim = isMainAxisRow ? heightMeasureMode : widthMeasureMode;

  const float paddingAndBorderAxisRow =
      isMainAxisRow ? paddingAndBorderAxisMain : paddingAndBorderAxisCross;
  const float paddingAndBorderAxisColumn =
      isMainAxisRow ? paddingAndBorderAxisCross : paddingAndBorderAxisMain;

  const float marginAxisRow =
      node->getMarginForAxis(FILTFlexDirectionRow, parentWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(FILTFlexDirectionColumn, parentWidth);

  const float minInnerWidth =
      FILTResolveValue(
          node->getStyle().minDimensions[FILTDimensionWidth], parentWidth) -
      paddingAndBorderAxisRow;
  const float maxInnerWidth =
      FILTResolveValue(
          node->getStyle().maxDimensions[FILTDimensionWidth], parentWidth) -
      paddingAndBorderAxisRow;
  const float minInnerHeight =
      FILTResolveValue(
          node->getStyle().minDimensions[FILTDimensionHeight], parentHeight) -
      paddingAndBorderAxisColumn;
  const float maxInnerHeight =
      FILTResolveValue(
          node->getStyle().maxDimensions[FILTDimensionHeight], parentHeight) -
      paddingAndBorderAxisColumn;

  const float minInnerMainDim = isMainAxisRow ? minInnerWidth : minInnerHeight;
  const float maxInnerMainDim = isMainAxisRow ? maxInnerWidth : maxInnerHeight;

  // STEP 2: DETERMINE AVAILABLE SIZE IN MAIN AND CROSS DIRECTIONS

  float availableInnerWidth = FILTNodeCalculateAvailableInnerDim(
      node, FILTFlexDirectionRow, availableWidth, parentWidth);
  float availableInnerHeight = FILTNodeCalculateAvailableInnerDim(
      node, FILTFlexDirectionColumn, availableHeight, parentHeight);

  float availableInnerMainDim =
      isMainAxisRow ? availableInnerWidth : availableInnerHeight;
  const float availableInnerCrossDim =
      isMainAxisRow ? availableInnerHeight : availableInnerWidth;

  float totalOuterFlexBasis = 0;

  // STEP 3: DETERMINE FLEX BASIS FOR EACH ITEM

  FILTNodeComputeFlexBasisForChildren(
      node,
      availableInnerWidth,
      availableInnerHeight,
      widthMeasureMode,
      heightMeasureMode,
      direction,
      mainAxis,
      config,
      performLayout,
      totalOuterFlexBasis);

  const bool flexBasisOverflows = measureModeMainDim == FILTMeasureModeUndefined
      ? false
      : totalOuterFlexBasis > availableInnerMainDim;
  if (isNodeFlexWrap && flexBasisOverflows &&
      measureModeMainDim == FILTMeasureModeAtMost) {
    measureModeMainDim = FILTMeasureModeExactly;
  }
  // STEP 4: COLLECT FLEX ITEMS INTO FLEX LINES

  // Indexes of children that represent the first and last items in the line.
  uint32_t startOfLineIndex = 0;
  uint32_t endOfLineIndex = 0;

  // Number of lines.
  uint32_t lineCount = 0;

  // Accumulated cross dimensions of all lines so far.
  float totalLineCrossDim = 0;

  // Max main dimension of all the lines.
  float maxLineMainDim = 0;
  FILTCollectFlexItemsRowValues collectedFlexItemsValues;
  for (; endOfLineIndex < childCount;
       lineCount++, startOfLineIndex = endOfLineIndex) {
    collectedFlexItemsValues = FILTCalculateCollectFlexItemsRowValues(
        node,
        parentDirection,
        mainAxisParentSize,
        availableInnerWidth,
        availableInnerMainDim,
        startOfLineIndex,
        lineCount);
    endOfLineIndex = collectedFlexItemsValues.endOfLineIndex;

    // If we don't need to measure the cross axis, we can skip the entire flex
    // step.
    const bool canSkipFlex =
        !performLayout && measureModeCrossDim == FILTMeasureModeExactly;

    // STEP 5: RESOLVING FLEXIBLE LENGTHS ON MAIN AXIS
    // Calculate the remaining available space that needs to be allocated.
    // If the main dimension size isn't known, it is computed based on
    // the line length, so there's no more space left to distribute.

    bool sizeBasedOnContent = false;
    // If we don't measure with exact main dimension we want to ensure we don't violate min and max
    if (measureModeMainDim != FILTMeasureModeExactly) {
      if (!FILTFloatIsUndefined(minInnerMainDim) &&
          collectedFlexItemsValues.sizeConsumedOnCurrentLine <
              minInnerMainDim) {
        availableInnerMainDim = minInnerMainDim;
      } else if (
          !FILTFloatIsUndefined(maxInnerMainDim) &&
          collectedFlexItemsValues.sizeConsumedOnCurrentLine >
              maxInnerMainDim) {
        availableInnerMainDim = maxInnerMainDim;
      } else {
        if (!node->getConfig()->useLegacyStretchBehaviour &&
            (collectedFlexItemsValues.totalFlexGrowFactors == 0 ||
             node->resolveFlexGrow() == 0)) {
          // If we don't have any children to flex or we can't flex the node
          // itself, space we've used is all space we need. Root node also
          // should be shrunk to minimum
          availableInnerMainDim =
              collectedFlexItemsValues.sizeConsumedOnCurrentLine;
        }

        if (node->getConfig()->useLegacyStretchBehaviour) {
          node->setLayoutDidUseLegacyFlag(true);
        }
        sizeBasedOnContent = !node->getConfig()->useLegacyStretchBehaviour;
      }
    }

    if (!sizeBasedOnContent && !FILTFloatIsUndefined(availableInnerMainDim)) {
      collectedFlexItemsValues.remainingFreeSpace = availableInnerMainDim -
          collectedFlexItemsValues.sizeConsumedOnCurrentLine;
    } else if (collectedFlexItemsValues.sizeConsumedOnCurrentLine < 0) {
      // availableInnerMainDim is indefinite which means the node is being sized based on its
      // content.
      // sizeConsumedOnCurrentLine is negative which means the node will allocate 0 points for
      // its content. Consequently, remainingFreeSpace is 0 - sizeConsumedOnCurrentLine.
      collectedFlexItemsValues.remainingFreeSpace =
          -collectedFlexItemsValues.sizeConsumedOnCurrentLine;
    }

    if (!canSkipFlex) {
      FILTResolveFlexibleLength(
          node,
          collectedFlexItemsValues,
          mainAxis,
          crossAxis,
          mainAxisParentSize,
          availableInnerMainDim,
          availableInnerCrossDim,
          availableInnerWidth,
          availableInnerHeight,
          flexBasisOverflows,
          measureModeCrossDim,
          performLayout,
          config);
    }

    node->setLayoutHadOverflow(
        node->getLayout().hadOverflow |
        (collectedFlexItemsValues.remainingFreeSpace < 0));

    // STEP 6: MAIN-AXIS JUSTIFICATION & CROSS-AXIS SIZE DETERMINATION

    // At this point, all the children have their dimensions set in the main
    // axis.
    // Their dimensions are also set in the cross axis with the exception of
    // items
    // that are aligned "stretch". We need to compute these stretch values and
    // set the final positions.

    FILTJustifyMainAxis(
        node,
        collectedFlexItemsValues,
        startOfLineIndex,
        mainAxis,
        crossAxis,
        measureModeMainDim,
        measureModeCrossDim,
        mainAxisParentSize,
        parentWidth,
        availableInnerMainDim,
        availableInnerCrossDim,
        availableInnerWidth,
        performLayout);

    float containerCrossAxis = availableInnerCrossDim;
    if (measureModeCrossDim == FILTMeasureModeUndefined ||
        measureModeCrossDim == FILTMeasureModeAtMost) {
      // Compute the cross axis from the max cross dimension of the children.
      containerCrossAxis =
          FILTNodeBoundAxis(
              node,
              crossAxis,
              collectedFlexItemsValues.crossDim + paddingAndBorderAxisCross,
              crossAxisParentSize,
              parentWidth) -
          paddingAndBorderAxisCross;
    }

    // If there's no flex wrap, the cross dimension is defined by the container.
    if (!isNodeFlexWrap && measureModeCrossDim == FILTMeasureModeExactly) {
      collectedFlexItemsValues.crossDim = availableInnerCrossDim;
    }

    // Clamp to the min/max size specified on the container.
    collectedFlexItemsValues.crossDim =
        FILTNodeBoundAxis(
            node,
            crossAxis,
            collectedFlexItemsValues.crossDim + paddingAndBorderAxisCross,
            crossAxisParentSize,
            parentWidth) -
        paddingAndBorderAxisCross;

    // STEP 7: CROSS-AXIS ALIGNMENT
    // We can skip child alignment if we're just measuring the container.
    if (performLayout) {
      for (uint32_t i = startOfLineIndex; i < endOfLineIndex; i++) {
        const FILTNodeRef child = node->getChild(i);
        if (child->getStyle().display == FILTDisplayNone) {
          continue;
        }
        if (child->getStyle().positionType == FILTPositionTypeAbsolute) {
          // If the child is absolutely positioned and has a
          // top/left/bottom/right set, override
          // all the previously computed positions to set it correctly.
          const bool isChildLeadingPosDefined =
              child->isLeadingPositionDefined(crossAxis);
          if (isChildLeadingPosDefined) {
            child->setLayoutPosition(
                child->getLeadingPosition(crossAxis, availableInnerCrossDim) +
                    node->getLeadingBorder(crossAxis) +
                    child->getLeadingMargin(crossAxis, availableInnerWidth),
                pos[crossAxis]);
          }
          // If leading position is not defined or calculations result in Nan, default to border + margin
          if (!isChildLeadingPosDefined ||
              FILTFloatIsUndefined(child->getLayout().position[pos[crossAxis]])) {
            child->setLayoutPosition(
                node->getLeadingBorder(crossAxis) +
                    child->getLeadingMargin(crossAxis, availableInnerWidth),
                pos[crossAxis]);
          }
        } else {
          float leadingCrossDim = leadingPaddingAndBorderCross;

          // For a relative children, we're either using alignItems (parent) or
          // alignSelf (child) in order to determine the position in the cross
          // axis
          const FILTAlign alignItem = FILTNodeAlignItem(node, child);

          // If the child uses align stretch, we need to lay it out one more
          // time, this time
          // forcing the cross-axis size to be the computed cross size for the
          // current line.
          if (alignItem == FILTAlignStretch &&
              child->marginLeadingValue(crossAxis).unit != FILTUnitAuto &&
              child->marginTrailingValue(crossAxis).unit != FILTUnitAuto) {
            // If the child defines a definite size for its cross axis, there's
            // no need to stretch.
            if (!FILTNodeIsStyleDimDefined(child, crossAxis, availableInnerCrossDim)) {
              float childMainSize =
                  child->getLayout().measuredDimensions[dim[mainAxis]];
              float childCrossSize =
                  !FILTFloatIsUndefined(child->getStyle().aspectRatio)
                  ? ((child->getMarginForAxis(crossAxis, availableInnerWidth) +
                      (isMainAxisRow
                           ? childMainSize / child->getStyle().aspectRatio
                           : childMainSize * child->getStyle().aspectRatio)))
                  : collectedFlexItemsValues.crossDim;

              childMainSize +=
                  child->getMarginForAxis(mainAxis, availableInnerWidth);

              FILTMeasureMode childMainMeasureMode = FILTMeasureModeExactly;
              FILTMeasureMode childCrossMeasureMode = FILTMeasureModeExactly;
              FILTConstrainMaxSizeForMode(child,
                                        mainAxis,
                                        availableInnerMainDim,
                                        availableInnerWidth,
                                        &childMainMeasureMode,
                                        &childMainSize);
              FILTConstrainMaxSizeForMode(child,
                                        crossAxis,
                                        availableInnerCrossDim,
                                        availableInnerWidth,
                                        &childCrossMeasureMode,
                                        &childCrossSize);

              const float childWidth = isMainAxisRow ? childMainSize : childCrossSize;
              const float childHeight = !isMainAxisRow ? childMainSize : childCrossSize;

              const FILTMeasureMode childWidthMeasureMode =
                  FILTFloatIsUndefined(childWidth) ? FILTMeasureModeUndefined
                                                 : FILTMeasureModeExactly;
              const FILTMeasureMode childHeightMeasureMode =
                  FILTFloatIsUndefined(childHeight) ? FILTMeasureModeUndefined
                                                  : FILTMeasureModeExactly;

              FILTLayoutNodeInternal(
                  child,
                  childWidth,
                  childHeight,
                  direction,
                  childWidthMeasureMode,
                  childHeightMeasureMode,
                  availableInnerWidth,
                  availableInnerHeight,
                  true,
                  "stretch",
                  config);
            }
          } else {
            const float remainingCrossDim = containerCrossAxis -
                FILTNodeDimWithMargin(child, crossAxis, availableInnerWidth);

            if (child->marginLeadingValue(crossAxis).unit == FILTUnitAuto &&
                child->marginTrailingValue(crossAxis).unit == FILTUnitAuto) {
              leadingCrossDim += fmaxf(0.0f, remainingCrossDim / 2);
            } else if (
                child->marginTrailingValue(crossAxis).unit == FILTUnitAuto) {
              // No-Op
            } else if (
                child->marginLeadingValue(crossAxis).unit == FILTUnitAuto) {
              leadingCrossDim += fmaxf(0.0f, remainingCrossDim);
            } else if (alignItem == FILTAlignFlexStart) {
              // No-Op
            } else if (alignItem == FILTAlignCenter) {
              leadingCrossDim += remainingCrossDim / 2;
            } else {
              leadingCrossDim += remainingCrossDim;
            }
          }
          // And we apply the position
          child->setLayoutPosition(
              child->getLayout().position[pos[crossAxis]] + totalLineCrossDim +
                  leadingCrossDim,
              pos[crossAxis]);
        }
      }
    }

    totalLineCrossDim += collectedFlexItemsValues.crossDim;
    maxLineMainDim = fmaxf(maxLineMainDim, collectedFlexItemsValues.mainDim);
  }

  // STEP 8: MULTI-LINE CONTENT ALIGNMENT
  if (performLayout && (lineCount > 1 || FILTIsBaselineLayout(node)) &&
      !FILTFloatIsUndefined(availableInnerCrossDim)) {
    const float remainingAlignContentDim = availableInnerCrossDim - totalLineCrossDim;

    float crossDimLead = 0;
    float currentLead = leadingPaddingAndBorderCross;

    switch (node->getStyle().alignContent) {
      case FILTAlignFlexEnd:
        currentLead += remainingAlignContentDim;
        break;
      case FILTAlignCenter:
        currentLead += remainingAlignContentDim / 2;
        break;
      case FILTAlignStretch:
        if (availableInnerCrossDim > totalLineCrossDim) {
          crossDimLead = remainingAlignContentDim / lineCount;
        }
        break;
      case FILTAlignSpaceAround:
        if (availableInnerCrossDim > totalLineCrossDim) {
          currentLead += remainingAlignContentDim / (2 * lineCount);
          if (lineCount > 1) {
            crossDimLead = remainingAlignContentDim / lineCount;
          }
        } else {
          currentLead += remainingAlignContentDim / 2;
        }
        break;
      case FILTAlignSpaceBetween:
        if (availableInnerCrossDim > totalLineCrossDim && lineCount > 1) {
          crossDimLead = remainingAlignContentDim / (lineCount - 1);
        }
        break;
      case FILTAlignAuto:
      case FILTAlignFlexStart:
      case FILTAlignBaseline:
        break;
    }

    uint32_t endIndex = 0;
    for (uint32_t i = 0; i < lineCount; i++) {
      const uint32_t startIndex = endIndex;
      uint32_t ii;

      // compute the line's height and find the endIndex
      float lineHeight = 0;
      float maxAscentForCurrentLine = 0;
      float maxDescentForCurrentLine = 0;
      for (ii = startIndex; ii < childCount; ii++) {
        const FILTNodeRef child = node->getChild(ii);
        if (child->getStyle().display == FILTDisplayNone) {
          continue;
        }
        if (child->getStyle().positionType == FILTPositionTypeRelative) {
          if (child->getLineIndex() != i) {
            break;
          }
          if (FILTNodeIsLayoutDimDefined(child, crossAxis)) {
            lineHeight = fmaxf(
                lineHeight,
                child->getLayout().measuredDimensions[dim[crossAxis]] +
                    child->getMarginForAxis(crossAxis, availableInnerWidth));
          }
          if (FILTNodeAlignItem(node, child) == FILTAlignBaseline) {
            const float ascent = FILTBaseline(child) +
                child->getLeadingMargin(
                    FILTFlexDirectionColumn, availableInnerWidth);
            const float descent =
                child->getLayout().measuredDimensions[FILTDimensionHeight] +
                child->getMarginForAxis(
                    FILTFlexDirectionColumn, availableInnerWidth) -
                ascent;
            maxAscentForCurrentLine = fmaxf(maxAscentForCurrentLine, ascent);
            maxDescentForCurrentLine = fmaxf(maxDescentForCurrentLine, descent);
            lineHeight = fmaxf(lineHeight, maxAscentForCurrentLine + maxDescentForCurrentLine);
          }
        }
      }
      endIndex = ii;
      lineHeight += crossDimLead;

      if (performLayout) {
        for (ii = startIndex; ii < endIndex; ii++) {
          const FILTNodeRef child = node->getChild(ii);
          if (child->getStyle().display == FILTDisplayNone) {
            continue;
          }
          if (child->getStyle().positionType == FILTPositionTypeRelative) {
            switch (FILTNodeAlignItem(node, child)) {
              case FILTAlignFlexStart: {
                child->setLayoutPosition(
                    currentLead +
                        child->getLeadingMargin(crossAxis, availableInnerWidth),
                    pos[crossAxis]);
                break;
              }
              case FILTAlignFlexEnd: {
                child->setLayoutPosition(
                    currentLead + lineHeight -
                        child->getTrailingMargin(
                            crossAxis, availableInnerWidth) -
                        child->getLayout().measuredDimensions[dim[crossAxis]],
                    pos[crossAxis]);
                break;
              }
              case FILTAlignCenter: {
                float childHeight =
                    child->getLayout().measuredDimensions[dim[crossAxis]];

                child->setLayoutPosition(
                    currentLead + (lineHeight - childHeight) / 2,
                    pos[crossAxis]);
                break;
              }
              case FILTAlignStretch: {
                child->setLayoutPosition(
                    currentLead +
                        child->getLeadingMargin(crossAxis, availableInnerWidth),
                    pos[crossAxis]);

                // Remeasure child with the line height as it as been only measured with the
                // parents height yet.
                if (!FILTNodeIsStyleDimDefined(child, crossAxis, availableInnerCrossDim)) {
                  const float childWidth = isMainAxisRow
                      ? (child->getLayout()
                             .measuredDimensions[FILTDimensionWidth] +
                         child->getMarginForAxis(mainAxis, availableInnerWidth))
                      : lineHeight;

                  const float childHeight = !isMainAxisRow
                      ? (child->getLayout()
                             .measuredDimensions[FILTDimensionHeight] +
                         child->getMarginForAxis(
                             crossAxis, availableInnerWidth))
                      : lineHeight;

                  if (!(FILTFloatsEqual(
                            childWidth,
                            child->getLayout()
                                .measuredDimensions[FILTDimensionWidth]) &&
                        FILTFloatsEqual(
                            childHeight,
                            child->getLayout()
                                .measuredDimensions[FILTDimensionHeight]))) {
                    FILTLayoutNodeInternal(child,
                                         childWidth,
                                         childHeight,
                                         direction,
                                         FILTMeasureModeExactly,
                                         FILTMeasureModeExactly,
                                         availableInnerWidth,
                                         availableInnerHeight,
                                         true,
                                         "multiline-stretch",
                                         config);
                  }
                }
                break;
              }
              case FILTAlignBaseline: {
                child->setLayoutPosition(
                    currentLead + maxAscentForCurrentLine - FILTBaseline(child) +
                        child->getLeadingPosition(
                            FILTFlexDirectionColumn, availableInnerCrossDim),
                    FILTEdgeTop);

                break;
              }
              case FILTAlignAuto:
              case FILTAlignSpaceBetween:
              case FILTAlignSpaceAround:
                break;
            }
          }
        }
      }

      currentLead += lineHeight;
    }
  }

  // STEP 9: COMPUTING FINAL DIMENSIONS

  node->setLayoutMeasuredDimension(
      FILTNodeBoundAxis(
          node,
          FILTFlexDirectionRow,
          availableWidth - marginAxisRow,
          parentWidth,
          parentWidth),
      FILTDimensionWidth);

  node->setLayoutMeasuredDimension(
      FILTNodeBoundAxis(
          node,
          FILTFlexDirectionColumn,
          availableHeight - marginAxisColumn,
          parentHeight,
          parentWidth),
      FILTDimensionHeight);

  // If the user didn't specify a width or height for the node, set the
  // dimensions based on the children.
  if (measureModeMainDim == FILTMeasureModeUndefined ||
      (node->getStyle().overflow != FILTOverflowScroll &&
       measureModeMainDim == FILTMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.
    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node, mainAxis, maxLineMainDim, mainAxisParentSize, parentWidth),
        dim[mainAxis]);

  } else if (
      measureModeMainDim == FILTMeasureModeAtMost &&
      node->getStyle().overflow == FILTOverflowScroll) {
    node->setLayoutMeasuredDimension(
        fmaxf(
            fminf(
                availableInnerMainDim + paddingAndBorderAxisMain,
                FILTNodeBoundAxisWithinMinAndMax(
                    node, mainAxis, maxLineMainDim, mainAxisParentSize)),
            paddingAndBorderAxisMain),
        dim[mainAxis]);
  }

  if (measureModeCrossDim == FILTMeasureModeUndefined ||
      (node->getStyle().overflow != FILTOverflowScroll &&
       measureModeCrossDim == FILTMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.

    node->setLayoutMeasuredDimension(
        FILTNodeBoundAxis(
            node,
            crossAxis,
            totalLineCrossDim + paddingAndBorderAxisCross,
            crossAxisParentSize,
            parentWidth),
        dim[crossAxis]);

  } else if (
      measureModeCrossDim == FILTMeasureModeAtMost &&
      node->getStyle().overflow == FILTOverflowScroll) {
    node->setLayoutMeasuredDimension(
        fmaxf(
            fminf(
                availableInnerCrossDim + paddingAndBorderAxisCross,
                FILTNodeBoundAxisWithinMinAndMax(
                    node,
                    crossAxis,
                    totalLineCrossDim + paddingAndBorderAxisCross,
                    crossAxisParentSize)),
            paddingAndBorderAxisCross),
        dim[crossAxis]);
  }

  // As we only wrapped in normal direction yet, we need to reverse the positions on wrap-reverse.
  if (performLayout && node->getStyle().flexWrap == FILTWrapWrapReverse) {
    for (uint32_t i = 0; i < childCount; i++) {
      const FILTNodeRef child = FILTNodeGetChild(node, i);
      if (child->getStyle().positionType == FILTPositionTypeRelative) {
        child->setLayoutPosition(
            node->getLayout().measuredDimensions[dim[crossAxis]] -
                child->getLayout().position[pos[crossAxis]] -
                child->getLayout().measuredDimensions[dim[crossAxis]],
            pos[crossAxis]);
      }
    }
  }

  if (performLayout) {
    // STEP 10: SIZING AND POSITIONING ABSOLUTE CHILDREN
    for (auto child : node->getChildren()) {
      if (child->getStyle().positionType != FILTPositionTypeAbsolute) {
        continue;
      }
      FILTNodeAbsoluteLayoutChild(
          node,
          child,
          availableInnerWidth,
          isMainAxisRow ? measureModeMainDim : measureModeCrossDim,
          availableInnerHeight,
          direction,
          config);
    }

    // STEP 11: SETTING TRAILING POSITIONS FOR CHILDREN
    const bool needsMainTrailingPos =
        mainAxis == FILTFlexDirectionRowReverse || mainAxis == FILTFlexDirectionColumnReverse;
    const bool needsCrossTrailingPos =
        crossAxis == FILTFlexDirectionRowReverse || crossAxis == FILTFlexDirectionColumnReverse;

    // Set trailing position if necessary.
    if (needsMainTrailingPos || needsCrossTrailingPos) {
      for (uint32_t i = 0; i < childCount; i++) {
        const FILTNodeRef child = node->getChild(i);
        if (child->getStyle().display == FILTDisplayNone) {
          continue;
        }
        if (needsMainTrailingPos) {
          FILTNodeSetChildTrailingPosition(node, child, mainAxis);
        }

        if (needsCrossTrailingPos) {
          FILTNodeSetChildTrailingPosition(node, child, crossAxis);
        }
      }
    }
  }
}

uint32_t gDepth = 0;
bool gPrintTree = false;
bool gPrintChanges = false;
bool gPrintSkips = false;

static const char *spacer = "                                                            ";

static const char *FILTSpacer(const unsigned long level) {
  const size_t spacerLen = strlen(spacer);
  if (level > spacerLen) {
    return &spacer[0];
  } else {
    return &spacer[spacerLen - level];
  }
}

static const char *FILTMeasureModeName(const FILTMeasureMode mode, const bool performLayout) {
  const char *kMeasureModeNames[FILTMeasureModeCount] = {"UNDEFINED", "EXACTLY", "AT_MOST"};
  const char *kLayoutModeNames[FILTMeasureModeCount] = {"LAY_UNDEFINED",
                                                      "LAY_EXACTLY",
                                                      "LAY_AT_"
                                                      "MOST"};

  if (mode >= FILTMeasureModeCount) {
    return "";
  }

  return performLayout ? kLayoutModeNames[mode] : kMeasureModeNames[mode];
}

static inline bool FILTMeasureModeSizeIsExactAndMatchesOldMeasuredSize(FILTMeasureMode sizeMode,
                                                                     float size,
                                                                     float lastComputedSize) {
  return sizeMode == FILTMeasureModeExactly && FILTFloatsEqual(size, lastComputedSize);
}

static inline bool FILTMeasureModeOldSizeIsUnspecifiedAndStillFits(FILTMeasureMode sizeMode,
                                                                 float size,
                                                                 FILTMeasureMode lastSizeMode,
                                                                 float lastComputedSize) {
  return sizeMode == FILTMeasureModeAtMost && lastSizeMode == FILTMeasureModeUndefined &&
         (size >= lastComputedSize || FILTFloatsEqual(size, lastComputedSize));
}

static inline bool FILTMeasureModeNewMeasureSizeIsStricterAndStillValid(FILTMeasureMode sizeMode,
                                                                      float size,
                                                                      FILTMeasureMode lastSizeMode,
                                                                      float lastSize,
                                                                      float lastComputedSize) {
  return lastSizeMode == FILTMeasureModeAtMost && sizeMode == FILTMeasureModeAtMost &&
         lastSize > size && (lastComputedSize <= size || FILTFloatsEqual(size, lastComputedSize));
}

float FILTRoundValueToPixelGrid(const float value,
                              const float pointScaleFactor,
                              const bool forceCeil,
                              const bool forceFloor) {
  const float roundingError = fmax(0.0001, 0.01 * pointScaleFactor);
  float scaledValue = value * pointScaleFactor;
  float fractial = fmodf(scaledValue, 1.0);
  if (FILTFloatsEqualWithPrecision(fractial, 0.0, roundingError)) {
    // First we check if the value is already rounded
    scaledValue = scaledValue - fractial;
  } else if (FILTFloatsEqualWithPrecision(fractial, 1.0, roundingError)) {
    scaledValue = scaledValue - fractial + 1.0;
  } else if (forceCeil) {
    // Next we check if we need to use forced rounding
    scaledValue = scaledValue - fractial + 1.0f;
  } else if (forceFloor) {
    scaledValue = scaledValue - fractial;
  } else {
    // Finally we just round the value
    scaledValue = scaledValue - fractial +
        (fractial > 0.5f || FILTFloatsEqualWithPrecision(fractial, 0.5f, roundingError) ? 1.0f : 0.0f);
  }
  return scaledValue / pointScaleFactor;
}

bool FILTNodeCanUseCachedMeasurement(const FILTMeasureMode widthMode,
                                   const float width,
                                   const FILTMeasureMode heightMode,
                                   const float height,
                                   const FILTMeasureMode lastWidthMode,
                                   const float lastWidth,
                                   const FILTMeasureMode lastHeightMode,
                                   const float lastHeight,
                                   const float lastComputedWidth,
                                   const float lastComputedHeight,
                                   const float marginRow,
                                   const float marginColumn,
                                   const FILTConfigRef config) {
  if (lastComputedHeight < 0 || lastComputedWidth < 0) {
    return false;
  }
  bool useRoundedComparison =
      config != nullptr && config->pointScaleFactor != 0;
  const float effectiveWidth =
      useRoundedComparison ? FILTRoundValueToPixelGrid(width, config->pointScaleFactor, false, false)
                           : width;
  const float effectiveHeight =
      useRoundedComparison ? FILTRoundValueToPixelGrid(height, config->pointScaleFactor, false, false)
                           : height;
  const float effectiveLastWidth =
      useRoundedComparison
          ? FILTRoundValueToPixelGrid(lastWidth, config->pointScaleFactor, false, false)
          : lastWidth;
  const float effectiveLastHeight =
      useRoundedComparison
          ? FILTRoundValueToPixelGrid(lastHeight, config->pointScaleFactor, false, false)
          : lastHeight;

  const bool hasSameWidthSpec =
      lastWidthMode == widthMode && FILTFloatsEqual(effectiveLastWidth, effectiveWidth);
  const bool hasSameHeightSpec =
      lastHeightMode == heightMode && FILTFloatsEqual(effectiveLastHeight, effectiveHeight);

  const bool widthIsCompatible =
      hasSameWidthSpec || FILTMeasureModeSizeIsExactAndMatchesOldMeasuredSize(widthMode,
                                                                            width - marginRow,
                                                                            lastComputedWidth) ||
      FILTMeasureModeOldSizeIsUnspecifiedAndStillFits(widthMode,
                                                    width - marginRow,
                                                    lastWidthMode,
                                                    lastComputedWidth) ||
      FILTMeasureModeNewMeasureSizeIsStricterAndStillValid(
          widthMode, width - marginRow, lastWidthMode, lastWidth, lastComputedWidth);

  const bool heightIsCompatible =
      hasSameHeightSpec || FILTMeasureModeSizeIsExactAndMatchesOldMeasuredSize(heightMode,
                                                                             height - marginColumn,
                                                                             lastComputedHeight) ||
      FILTMeasureModeOldSizeIsUnspecifiedAndStillFits(heightMode,
                                                    height - marginColumn,
                                                    lastHeightMode,
                                                    lastComputedHeight) ||
      FILTMeasureModeNewMeasureSizeIsStricterAndStillValid(
          heightMode, height - marginColumn, lastHeightMode, lastHeight, lastComputedHeight);

  return widthIsCompatible && heightIsCompatible;
}

//
// This is a wrapper around the FILTNodelayoutImpl function. It determines
// whether the layout request is redundant and can be skipped.
//
// Parameters:
//  Input parameters are the same as FILTNodelayoutImpl (see above)
//  Return parameter is true if layout was performed, false if skipped
//
bool FILTLayoutNodeInternal(const FILTNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const FILTDirection parentDirection,
                          const FILTMeasureMode widthMeasureMode,
                          const FILTMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason,
                          const FILTConfigRef config) {
  FILTLayout* layout = &node->getLayout();

  gDepth++;

  const bool needToVisitNode =
      (node->isDirty() && layout->generationCount != gCurrentGenerationCount) ||
      layout->lastParentDirection != parentDirection;

  if (needToVisitNode) {
    // Invalidate the cached results.
    layout->nextCachedMeasurementsIndex = 0;
    layout->cachedLayout.widthMeasureMode = (FILTMeasureMode) -1;
    layout->cachedLayout.heightMeasureMode = (FILTMeasureMode) -1;
    layout->cachedLayout.computedWidth = -1;
    layout->cachedLayout.computedHeight = -1;
  }

  FILTCachedMeasurement* cachedResults = nullptr;

  // Determine whether the results are already cached. We maintain a separate
  // cache for layouts and measurements. A layout operation modifies the
  // positions
  // and dimensions for nodes in the subtree. The algorithm assumes that each
  // node
  // gets layed out a maximum of one time per tree layout, but multiple
  // measurements
  // may be required to resolve all of the flex dimensions.
  // We handle nodes with measure functions specially here because they are the
  // most
  // expensive to measure, so it's worth avoiding redundant measurements if at
  // all possible.
  if (node->getMeasure() != nullptr) {
    const float marginAxisRow =
        node->getMarginForAxis(FILTFlexDirectionRow, parentWidth);
    const float marginAxisColumn =
        node->getMarginForAxis(FILTFlexDirectionColumn, parentWidth);

    // First, try to use the layout cache.
    if (FILTNodeCanUseCachedMeasurement(widthMeasureMode,
                                      availableWidth,
                                      heightMeasureMode,
                                      availableHeight,
                                      layout->cachedLayout.widthMeasureMode,
                                      layout->cachedLayout.availableWidth,
                                      layout->cachedLayout.heightMeasureMode,
                                      layout->cachedLayout.availableHeight,
                                      layout->cachedLayout.computedWidth,
                                      layout->cachedLayout.computedHeight,
                                      marginAxisRow,
                                      marginAxisColumn,
                                      config)) {
      cachedResults = &layout->cachedLayout;
    } else {
      // Try to use the measurement cache.
      for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
        if (FILTNodeCanUseCachedMeasurement(widthMeasureMode,
                                          availableWidth,
                                          heightMeasureMode,
                                          availableHeight,
                                          layout->cachedMeasurements[i].widthMeasureMode,
                                          layout->cachedMeasurements[i].availableWidth,
                                          layout->cachedMeasurements[i].heightMeasureMode,
                                          layout->cachedMeasurements[i].availableHeight,
                                          layout->cachedMeasurements[i].computedWidth,
                                          layout->cachedMeasurements[i].computedHeight,
                                          marginAxisRow,
                                          marginAxisColumn,
                                          config)) {
          cachedResults = &layout->cachedMeasurements[i];
          break;
        }
      }
    }
  } else if (performLayout) {
    if (FILTFloatsEqual(layout->cachedLayout.availableWidth, availableWidth) &&
        FILTFloatsEqual(layout->cachedLayout.availableHeight, availableHeight) &&
        layout->cachedLayout.widthMeasureMode == widthMeasureMode &&
        layout->cachedLayout.heightMeasureMode == heightMeasureMode) {
      cachedResults = &layout->cachedLayout;
    }
  } else {
    for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
      if (FILTFloatsEqual(layout->cachedMeasurements[i].availableWidth, availableWidth) &&
          FILTFloatsEqual(layout->cachedMeasurements[i].availableHeight, availableHeight) &&
          layout->cachedMeasurements[i].widthMeasureMode == widthMeasureMode &&
          layout->cachedMeasurements[i].heightMeasureMode == heightMeasureMode) {
        cachedResults = &layout->cachedMeasurements[i];
        break;
      }
    }
  }

  if (!needToVisitNode && cachedResults != nullptr) {
    layout->measuredDimensions[FILTDimensionWidth] = cachedResults->computedWidth;
    layout->measuredDimensions[FILTDimensionHeight] = cachedResults->computedHeight;

    if (gPrintChanges && gPrintSkips) {
      FILTLog(node, FILTLogLevelVerbose, "%s%d.{[skipped] ", FILTSpacer(gDepth), gDepth);
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
      FILTLog(
          node,
          FILTLogLevelVerbose,
          "wm: %s, hm: %s, aw: %f ah: %f => d: (%f, %f) %s\n",
          FILTMeasureModeName(widthMeasureMode, performLayout),
          FILTMeasureModeName(heightMeasureMode, performLayout),
          availableWidth,
          availableHeight,
          cachedResults->computedWidth,
          cachedResults->computedHeight,
          reason);
    }
  } else {
    if (gPrintChanges) {
      FILTLog(
          node,
          FILTLogLevelVerbose,
          "%s%d.{%s",
          FILTSpacer(gDepth),
          gDepth,
          needToVisitNode ? "*" : "");
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
      FILTLog(
          node,
          FILTLogLevelVerbose,
          "wm: %s, hm: %s, aw: %f ah: %f %s\n",
          FILTMeasureModeName(widthMeasureMode, performLayout),
          FILTMeasureModeName(heightMeasureMode, performLayout),
          availableWidth,
          availableHeight,
          reason);
    }

    FILTNodelayoutImpl(node,
                     availableWidth,
                     availableHeight,
                     parentDirection,
                     widthMeasureMode,
                     heightMeasureMode,
                     parentWidth,
                     parentHeight,
                     performLayout,
                     config);

    if (gPrintChanges) {
      FILTLog(
          node,
          FILTLogLevelVerbose,
          "%s%d.}%s",
          FILTSpacer(gDepth),
          gDepth,
          needToVisitNode ? "*" : "");
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
      FILTLog(
          node,
          FILTLogLevelVerbose,
          "wm: %s, hm: %s, d: (%f, %f) %s\n",
          FILTMeasureModeName(widthMeasureMode, performLayout),
          FILTMeasureModeName(heightMeasureMode, performLayout),
          layout->measuredDimensions[FILTDimensionWidth],
          layout->measuredDimensions[FILTDimensionHeight],
          reason);
    }

    layout->lastParentDirection = parentDirection;

    if (cachedResults == nullptr) {
      if (layout->nextCachedMeasurementsIndex == FILT_MAX_CACHED_RESULT_COUNT) {
        if (gPrintChanges) {
          FILTLog(node, FILTLogLevelVerbose, "Out of cache entries!\n");
        }
        layout->nextCachedMeasurementsIndex = 0;
      }

      FILTCachedMeasurement *newCacheEntry;
      if (performLayout) {
        // Use the single layout cache entry.
        newCacheEntry = &layout->cachedLayout;
      } else {
        // Allocate a new measurement cache entry.
        newCacheEntry = &layout->cachedMeasurements[layout->nextCachedMeasurementsIndex];
        layout->nextCachedMeasurementsIndex++;
      }

      newCacheEntry->availableWidth = availableWidth;
      newCacheEntry->availableHeight = availableHeight;
      newCacheEntry->widthMeasureMode = widthMeasureMode;
      newCacheEntry->heightMeasureMode = heightMeasureMode;
      newCacheEntry->computedWidth = layout->measuredDimensions[FILTDimensionWidth];
      newCacheEntry->computedHeight = layout->measuredDimensions[FILTDimensionHeight];
    }
  }

  if (performLayout) {
    node->setLayoutDimension(
        node->getLayout().measuredDimensions[FILTDimensionWidth],
        FILTDimensionWidth);
    node->setLayoutDimension(
        node->getLayout().measuredDimensions[FILTDimensionHeight],
        FILTDimensionHeight);

    node->setHasNewLayout(true);
    node->setDirty(false);
  }

  gDepth--;
  layout->generationCount = gCurrentGenerationCount;
  return (needToVisitNode || cachedResults == nullptr);
}

void FILTConfigSetPointScaleFactor(const FILTConfigRef config, const float pixelsInPoint) {
  FILTAssertWithConfig(config, pixelsInPoint >= 0.0f, "Scale factor should not be less than zero");

  // We store points for Pixel as we will use it for rounding
  if (pixelsInPoint == 0.0f) {
    // Zero is used to skip rounding
    config->pointScaleFactor = 0.0f;
  } else {
    config->pointScaleFactor = pixelsInPoint;
  }
}

static void FILTRoundToPixelGrid(const FILTNodeRef node,
                               const float pointScaleFactor,
                               const float absoluteLeft,
                               const float absoluteTop) {
  if (pointScaleFactor == 0.0f) {
    return;
  }

  const float nodeLeft = node->getLayout().position[FILTEdgeLeft];
  const float nodeTop = node->getLayout().position[FILTEdgeTop];

  const float nodeWidth = node->getLayout().dimensions[FILTDimensionWidth];
  const float nodeHeight = node->getLayout().dimensions[FILTDimensionHeight];

  const float absoluteNodeLeft = absoluteLeft + nodeLeft;
  const float absoluteNodeTop = absoluteTop + nodeTop;

  const float absoluteNodeRight = absoluteNodeLeft + nodeWidth;
  const float absoluteNodeBottom = absoluteNodeTop + nodeHeight;

  // If a node has a custom measure function we never want to round down its size as this could
  // lead to unwanted text truncation.
  const bool textRounding = node->getNodeType() == FILTNodeTypeText;

  node->setLayoutPosition(
      FILTRoundValueToPixelGrid(nodeLeft, pointScaleFactor, false, textRounding),
      FILTEdgeLeft);

  node->setLayoutPosition(
      FILTRoundValueToPixelGrid(nodeTop, pointScaleFactor, false, textRounding),
      FILTEdgeTop);

  // We multiply dimension by scale factor and if the result is close to the whole number, we don't
  // have any fraction
  // To verify if the result is close to whole number we want to check both floor and ceil numbers
  const bool hasFractionalWidth = !FILTFloatsEqual(fmodf(nodeWidth * pointScaleFactor, 1.0), 0) &&
                                  !FILTFloatsEqual(fmodf(nodeWidth * pointScaleFactor, 1.0), 1.0);
  const bool hasFractionalHeight = !FILTFloatsEqual(fmodf(nodeHeight * pointScaleFactor, 1.0), 0) &&
                                   !FILTFloatsEqual(fmodf(nodeHeight * pointScaleFactor, 1.0), 1.0);

  node->setLayoutDimension(
      FILTRoundValueToPixelGrid(
          absoluteNodeRight,
          pointScaleFactor,
          (textRounding && hasFractionalWidth),
          (textRounding && !hasFractionalWidth)) -
          FILTRoundValueToPixelGrid(
              absoluteNodeLeft, pointScaleFactor, false, textRounding),
      FILTDimensionWidth);

  node->setLayoutDimension(
      FILTRoundValueToPixelGrid(
          absoluteNodeBottom,
          pointScaleFactor,
          (textRounding && hasFractionalHeight),
          (textRounding && !hasFractionalHeight)) -
          FILTRoundValueToPixelGrid(
              absoluteNodeTop, pointScaleFactor, false, textRounding),
      FILTDimensionHeight);

  const uint32_t childCount = FILTNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    FILTRoundToPixelGrid(FILTNodeGetChild(node, i), pointScaleFactor, absoluteNodeLeft, absoluteNodeTop);
  }
}

void FILTNodeCalculateLayout(const FILTNodeRef node,
                           const float parentWidth,
                           const float parentHeight,
                           const FILTDirection parentDirection) {
  // Increment the generation count. This will force the recursive routine to
  // visit
  // all dirty nodes at least once. Subsequent visits will be skipped if the
  // input
  // parameters don't change.
  gCurrentGenerationCount++;
  node->resolveDimension();
  float width = FILTUndefined;
  FILTMeasureMode widthMeasureMode = FILTMeasureModeUndefined;
  if (FILTNodeIsStyleDimDefined(node, FILTFlexDirectionRow, parentWidth)) {
    width =
        FILTResolveValue(
            node->getResolvedDimension(dim[FILTFlexDirectionRow]), parentWidth) +
        node->getMarginForAxis(FILTFlexDirectionRow, parentWidth);
    widthMeasureMode = FILTMeasureModeExactly;
  } else if (
      FILTResolveValue(
          node->getStyle().maxDimensions[FILTDimensionWidth], parentWidth) >=
      0.0f) {
    width = FILTResolveValue(
        node->getStyle().maxDimensions[FILTDimensionWidth], parentWidth);
    widthMeasureMode = FILTMeasureModeAtMost;
  } else {
    width = parentWidth;
    widthMeasureMode = FILTFloatIsUndefined(width) ? FILTMeasureModeUndefined : FILTMeasureModeExactly;
  }

  float height = FILTUndefined;
  FILTMeasureMode heightMeasureMode = FILTMeasureModeUndefined;
  if (FILTNodeIsStyleDimDefined(node, FILTFlexDirectionColumn, parentHeight)) {
    height = FILTResolveValue(
                 node->getResolvedDimension(dim[FILTFlexDirectionColumn]),
                 parentHeight) +
        node->getMarginForAxis(FILTFlexDirectionColumn, parentWidth);
    heightMeasureMode = FILTMeasureModeExactly;
  } else if (
      FILTResolveValue(
          node->getStyle().maxDimensions[FILTDimensionHeight], parentHeight) >=
      0.0f) {
    height = FILTResolveValue(
        node->getStyle().maxDimensions[FILTDimensionHeight], parentHeight);
    heightMeasureMode = FILTMeasureModeAtMost;
  } else {
    height = parentHeight;
    heightMeasureMode = FILTFloatIsUndefined(height) ? FILTMeasureModeUndefined : FILTMeasureModeExactly;
  }

  if (FILTLayoutNodeInternal(
          node,
          width,
          height,
          parentDirection,
          widthMeasureMode,
          heightMeasureMode,
          parentWidth,
          parentHeight,
          true,
          "initial",
          node->getConfig())) {
    node->setPosition(
        node->getLayout().direction, parentWidth, parentHeight, parentWidth);
    FILTRoundToPixelGrid(node, node->getConfig()->pointScaleFactor, 0.0f, 0.0f);

    if (gPrintTree) {
      FILTNodePrint(
          node,
          (FILTPrintOptions)(
              FILTPrintOptionsLayout | FILTPrintOptionsChildren |
              FILTPrintOptionsStyle));
    }
  }

  // We want to get rid off `useLegacyStretchBehaviour` from FILTConfig. But we
  // aren't sure whether client's of filates have gotten rid off this flag or not.
  // So logging this in FILTLayout would help to find out the call sites depending
  // on this flag. This check would be removed once we are sure no one is
  // dependent on this flag anymore. The flag
  // `shouldDiffLayoutWithoutLegacyStretchBehaviour` in FILTConfig will help to
  // run experiments.
  if (node->getConfig()->shouldDiffLayoutWithoutLegacyStretchBehaviour &&
      node->didUseLegacyFlag()) {
    const FILTNodeRef originalNode = FILTNodeDeepClone(node);
    originalNode->resolveDimension();
    // Recursively mark nodes as dirty
    originalNode->markDirtyAndPropogateDownwards();
    gCurrentGenerationCount++;
    // Rerun the layout, and calculate the diff
    originalNode->setAndPropogateUseLegacyFlag(false);
    if (FILTLayoutNodeInternal(
            originalNode,
            width,
            height,
            parentDirection,
            widthMeasureMode,
            heightMeasureMode,
            parentWidth,
            parentHeight,
            true,
            "initial",
            originalNode->getConfig())) {
      originalNode->setPosition(
          originalNode->getLayout().direction,
          parentWidth,
          parentHeight,
          parentWidth);
      FILTRoundToPixelGrid(
          originalNode,
          originalNode->getConfig()->pointScaleFactor,
          0.0f,
          0.0f);

      // Set whether the two layouts are different or not.
      node->setLayoutDoesLegacyFlagAffectsLayout(
          !originalNode->isLayoutTreeEqualToNode(*node));

      if (gPrintTree) {
        FILTNodePrint(
            originalNode,
            (FILTPrintOptions)(
                FILTPrintOptionsLayout | FILTPrintOptionsChildren |
                FILTPrintOptionsStyle));
      }
    }
    FILTConfigFreeRecursive(originalNode);
    FILTNodeFreeRecursive(originalNode);
  }
}

void FILTConfigSetLogger(const FILTConfigRef config, FILTLogger logger) {
  if (logger != nullptr) {
    config->logger = logger;
  } else {
#ifdef ANDROID
    config->logger = &FILTAndroidLog;
#else
    config->logger = &FILTDefaultLog;
#endif
  }
}

static void FILTVLog(const FILTConfigRef config,
                   const FILTNodeRef node,
                   FILTLogLevel level,
                   const char *format,
                   va_list args) {
  const FILTConfigRef logConfig = config != nullptr ? config : &gFILTConfigDefaults;
  logConfig->logger(logConfig, node, level, format, args);

  if (level == FILTLogLevelFatal) {
    abort();
  }
}

void FILTLogWithConfig(const FILTConfigRef config, FILTLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
  FILTVLog(config, nullptr, level, format, args);
  va_end(args);
}

void FILTLog(const FILTNodeRef node, FILTLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
  FILTVLog(
      node == nullptr ? nullptr : node->getConfig(), node, level, format, args);
  va_end(args);
}

void FILTAssert(const bool condition, const char *message) {
  if (!condition) {
    FILTLog(nullptr, FILTLogLevelFatal, "%s\n", message);
  }
}

void FILTAssertWithNode(const FILTNodeRef node, const bool condition, const char *message) {
  if (!condition) {
    FILTLog(node, FILTLogLevelFatal, "%s\n", message);
  }
}

void FILTAssertWithConfig(const FILTConfigRef config, const bool condition, const char *message) {
  if (!condition) {
    FILTLogWithConfig(config, FILTLogLevelFatal, "%s\n", message);
  }
}

void FILTConfigSetExperimentalFeatureEnabled(const FILTConfigRef config,
                                           const FILTExperimentalFeature feature,
                                           const bool enabled) {
  config->experimentalFeatures[feature] = enabled;
}

inline bool FILTConfigIsExperimentalFeatureEnabled(const FILTConfigRef config,
                                                 const FILTExperimentalFeature feature) {
  return config->experimentalFeatures[feature];
}

void FILTConfigSetUseWebDefaults(const FILTConfigRef config, const bool enabled) {
  config->useWebDefaults = enabled;
}

void FILTConfigSetUseLegacyStretchBehaviour(const FILTConfigRef config,
                                          const bool useLegacyStretchBehaviour) {
  config->useLegacyStretchBehaviour = useLegacyStretchBehaviour;
}

bool FILTConfigGetUseWebDefaults(const FILTConfigRef config) {
  return config->useWebDefaults;
}

void FILTConfigSetContext(const FILTConfigRef config, void *context) {
  config->context = context;
}

void *FILTConfigGetContext(const FILTConfigRef config) {
  return config->context;
}

void FILTConfigSetNodeClonedFunc(const FILTConfigRef config, const FILTNodeClonedFunc callback) {
  config->cloneNodeCallback = callback;
}
