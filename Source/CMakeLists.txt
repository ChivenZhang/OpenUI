cmake_minimum_required (VERSION 3.8)

add_definitions(-DRMGUI_SHARED_LIBRARY)

add_library (RmGUI SHARED
	RmGUI.h
	Render/IRmGUIRender.h
	Render/Private/RmGUIRender.cpp
	Render/Private/RmGUIRender.h
	Utility/shared_recursive_mutex.h
	Utility/span.h
	Utility/HashUtility.h
	Widget/IRmGUIWidget.h
	Widget/Private/RmGUIWidget.h
	Widget/Private/RmGUIWidget.cpp
	Widget/IRmGUIPainter.h
	Widget/IRmGUIContext.h
	Widget/IRmGUIEvent.h
	Widget/Private/RmGUIContext.h 
	Widget/Private/RmGUIContext.cpp 
	Widget/Private/RmGUIPainter.h 
	Widget/Private/RmGUIPainter.cpp
	Widget/Private/RmGUIEvent.cpp 
 "Widget/Private/RmGUIPanel.h" "Widget/Private/RmGUILabel.h" "Widget/Private/RmGUIPanel.cpp"  "Widget/Private/RmGUILabel.cpp" "Widget/IRmGUISurface.h")

target_include_directories(RmGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RmGUI 
	cairo
)