// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "FlexLayout",
  products: [
    .library(name: "FlexLayout", targets: ["FlexLayout"]),
    .library(name: "FlexLayoutFilates", targets: ["FlexLayoutFilates"]),
    .library(name: "FlexLayoutFilatesKit", targets: ["FlexLayoutFilatesKit"])
  ],
  dependencies: [
  ],
  targets: [
    .target(
      name: "FlexLayout",
      dependencies: ["FlexLayoutFilates", "FlexLayoutFilatesKit"],
      path: "Sources/Swift",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/filates/"),
        .headerSearchPath("./")
      ],
      swiftSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ]
    ),
    .target(
      name: "FlexLayoutFilates",
      dependencies: [],
      path: "Sources/filates",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/filates/"),
        .headerSearchPath("./")
      ]
    ),
    .target(
      name: "FlexLayoutFilatesKit",
      dependencies: ["FlexLayoutFilates"],
      path: "Sources/FilatesKit",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/FilatesKit/"),
        .headerSearchPath("./")
      ]
    )
  ],
  cLanguageStandard: .gnu99,
  cxxLanguageStandard: .gnucxx11
)
