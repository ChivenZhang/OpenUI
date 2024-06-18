cmake_minimum_required (VERSION 3.8)

add_definitions(-DRMGUI_SHARED_LIBRARY)

add_library (RmGUI SHARED
	RmGUI.h
	Utility/shared_recursive_mutex.h
	Utility/span.h
	Widget/IRmGUIWidget.h
	Widget/Private/RmGUIWidget.h
	Widget/Private/RmGUIWidget.cpp
	Widget/IRmGUIPainter.h
	Widget/IRmGUIEvent.h 
	Render/IRmGUIRender.h
	Widget/IRmGUIContext.h
	Render/Private/RmGUIRender.cpp
	Render/Private/RmGUIRender.h
 "Widget/Private/RmGUIContext.h" "Widget/Private/RmGUIContext.cpp" "Widget/Private/RmGUIPainter.h" "Widget/Private/RmGUIPainter.cpp")

target_include_directories(RmGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(RmGUI)
