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
	Widget/IRmGUISurface.h
	Widget/IRmGUIWidget.h
	Widget/Private/RmGUIButton.cpp
	Widget/Private/RmGUIButton.h
	Widget/Private/RmGUICombo.cpp
	Widget/Private/RmGUICombo.h
	Widget/Private/RmGUIContext.cpp
	Widget/Private/RmGUIContext.h
	Widget/Private/RmGUIControl.cpp
	Widget/Private/RmGUIControl.h
	Widget/Private/RmGUIEvent.cpp
	Widget/Private/RmGUIFlow.cpp
	Widget/Private/RmGUIFlow.h
	Widget/Private/RmGUIGrid.cpp
	Widget/Private/RmGUIGrid.h
	Widget/Private/RmGUIHBox.cpp
	Widget/Private/RmGUIHBox.h
	Widget/Private/RmGUILabel.cpp
	Widget/Private/RmGUILabel.h
	Widget/Private/RmGUILayer.cpp
	Widget/Private/RmGUILayer.h
	Widget/Private/RmGUILayout.cpp
	Widget/Private/RmGUILayout.h
	Widget/Private/RmGUIPanel.cpp
	Widget/Private/RmGUIPanel.h
	Widget/Private/RmGUIScroll.cpp
	Widget/Private/RmGUIScroll.h
	Widget/Private/RmGUIScrollBar.cpp
	Widget/Private/RmGUIScrollBar.h
	Widget/Private/RmGUISignal.h
	Widget/Private/RmGUIVBox.cpp
	Widget/Private/RmGUIVBox.h
	Widget/Private/RmGUIWidget.cpp
	Widget/Private/RmGUIWidget.h)

target_include_directories(RmGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RmGUI 
	cairo
	taitank
)