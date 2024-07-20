cmake_minimum_required (VERSION 3.8)

add_definitions(-DOPENUI_SHARED_LIBRARY)

add_library (OpenUI SHARED
"UI.h" "UIContext.cpp" "UIContext.h" "UIElement.cpp" "UIElement.h" "UIRender.h" "UIPainter.h" "UIEvent.h" "UISignal.h" "UIFactory.h" "UIBuilder.cpp" "UIBuilder.h" "UIParser.h" "UIParser.cpp" 
"Element/UILabel.cpp" "Element/UILabel.h" "Element/UIButton.cpp" "Element/UIButton.h" "Element/UIHBox.h" "Element/UIHBox.cpp" "Element/UIVBox.h" "Element/UIVBox.cpp" "Element/UIFlow.h" "Element/UIFlow.cpp" "Element/UIScroll.h" "Element/UIScroll.cpp" 
"Element/UIScrollBar.h" "Element/UIScrollBar.cpp" "Element/UICombo.h" "Element/UICombo.cpp" "Element/UIInput.h" "Element/UIInput.cpp" "Element/UIEdit.h" "Element/UIEdit.cpp" "Element/UIGrid.h" "Element/UIGrid.cpp" "Element/UISlider.h" "Element/UISlider.cpp" "Element/UIProgress.h" "Element/UIProgress.cpp" "Element/UICheck.h" "Element/UICheck.cpp" "Element/UIRadio.h" "Element/UIRadio.cpp" "Element/UIMenu.h" "Element/UIMenu.cpp" "Element/UISplit.h" "Element/UISplit.cpp" "Element/UILine.h" "Element/UILine.cpp")

target_link_libraries(OpenUI yogacore)