
add_library(OpenUI STATIC
     "Private/UIBuilder.cpp"
     "Private/UIButton.cpp"
     "Private/UICheck.cpp"
     "Private/UICombo.cpp"
     "Private/UICanvas.cpp"
     "Private/UIEdit.cpp"
     "Private/UIWidget.cpp"
     "Private/UIFlow.cpp"
     "Private/UIGrid.cpp"
     "Private/UIHBox.cpp"
     "Private/UIInput.cpp"
     "Private/UILabel.cpp"
     "Private/UILine.cpp"
     "Private/UIMenu.cpp"
     "Private/UIParser.cpp"
     "Private/UIPieceTable.h"
     "Private/UIProgress.cpp"
     "Private/UIRadio.cpp"
     "Private/UIScroll.cpp"
     "Private/UIScrollBar.cpp"
     "Private/UISlider.cpp"
     "Private/UISplit.cpp"
     "Private/UIVBox.cpp"
     "UI.h"
     "UIBuilder.h"
     "UIButton.h"
     "UICheck.h"
     "UICombo.h"
     "UICanvas.h"
     "UIDevice.h"
     "UIEdit.h"
     "UIWidget.h"
     "UIEvent.h"
     "UIFlow.h"
     "UIGrid.h"
     "UIHBox.h"
     "UIInput.h"
     "UILabel.h"
     "UILine.h"
     "UIMenu.h"
     "UIPainter.h"
     "UIProgress.h"
     "UIRadio.h"
     "UIRender.h"
     "UIScroll.h"
     "UIScrollBar.h"
     "UISignal.h"
     "UISlider.h"
     "UISplit.h"
     "UIVBox.h"
        Private/UIFactory.cpp
        UIStyle.h
        Private/UIStyle.cpp
)

find_package(yoga CONFIG REQUIRED)
target_link_libraries(OpenUI PRIVATE yoga::yogacore)

find_package(Stb REQUIRED)
target_include_directories(OpenUI PRIVATE ${Stb_INCLUDE_DIR})