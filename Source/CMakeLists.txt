cmake_minimum_required (VERSION 3.8)

add_definitions(-DRMGUI_SHARED_LIBRARY)

add_library (RmGUI SHARED
	RmGUI.h
	Utility/shared_recursive_mutex.h
	Utility/flex.h
	Utility/flex.c
	Utility/span.h
	Utility/HashUtility.h
	Widget/IRmGUIWidget.h
	Widget/IRmGUIPainter.h
	Widget/IRmGUIRender.h
	Widget/IRmGUIContext.h
	Widget/IRmGUIEvent.h
	Widget/Private/RmGUIWidget.h
	Widget/Private/RmGUIWidget.cpp
	Widget/Private/RmGUIContext.h 
	Widget/Private/RmGUIContext.cpp 
	Widget/Private/RmGUIEvent.cpp 
	Widget/Private/RmGUIPanel.h 
	Widget/Private/RmGUILabel.h 
	Widget/Private/RmGUIPanel.cpp  
	Widget/Private/RmGUILabel.cpp 
	Widget/IRmGUISurface.h
 "Widget/Private/RmGUIControl.h" "Widget/Private/RmGUIControl.cpp" "Widget/Private/RmGUILayout.h" "Widget/Private/RmGUILayout.cpp")

target_include_directories(RmGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RmGUI 
	cairo
)