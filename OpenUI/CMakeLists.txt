cmake_minimum_required (VERSION 3.8)

add_definitions(-DOPENUI_SHARED_LIBRARY)

add_library (OpenUI SHARED
"UI.h" "UIContext.cpp" "UIContext.h" "UIElement.h" "UIFactory.h" "UIBuilder.h" "UIRender.h" "UIEvent.h"   "UIPainter.h"  "UISignal.h" "UIElement.cpp" "Element/UIButton.h" "Element/UILabel.h")

target_include_directories(OpenUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenUI  )