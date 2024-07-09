cmake_minimum_required (VERSION 3.8)

add_definitions(-DRMGUI_SHARED_LIBRARY)

add_library (RmGUI SHARED
	RmGUI.h
	Utility/span.h
	Widget/IRmGUIContext.h
	Widget/IRmGUIEvent.h
	Widget/IRmGUIPainter.h
	Widget/IRmGUIRender.h
	Widget/IRmGUISignal.h
	Widget/IRmGUIWidget.h
	Widget/RmGUIButton.cpp
	Widget/RmGUIButton.h
	Widget/RmGUICombo.cpp
	Widget/RmGUICombo.h
	Widget/RmGUIContext.cpp
	Widget/RmGUIContext.h
	Widget/RmGUIControl.cpp
	Widget/RmGUIControl.h
	Widget/RmGUIEvent.cpp
	Widget/RmGUIFlow.cpp
	Widget/RmGUIFlow.h
	Widget/RmGUIGrid.cpp
	Widget/RmGUIGrid.h
	Widget/RmGUIHBox.cpp
	Widget/RmGUIHBox.h
	Widget/RmGUILabel.cpp
	Widget/RmGUILabel.h
	Widget/RmGUILayout.cpp
	Widget/RmGUILayout.h
	Widget/RmGUIPanel.cpp
	Widget/RmGUIPanel.h
	Widget/RmGUIScroll.cpp
	Widget/RmGUIScroll.h
	Widget/RmGUIScrollBar.cpp
	Widget/RmGUIScrollBar.h
	Widget/RmGUISignal.h
	Widget/RmGUIText.cpp
	Widget/RmGUIText.h
	Widget/RmGUIVBox.cpp
	Widget/RmGUIVBox.h
	Widget/RmGUIWidget.cpp
	Widget/RmGUIWidget.h
	Widget/RmGUIEdit.h
	Widget/RmGUIEdit.cpp
	Widget/RmPieceTable.h
	Widget/RmPieceTable.cpp)

target_include_directories(RmGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RmGUI 
	taitank
)