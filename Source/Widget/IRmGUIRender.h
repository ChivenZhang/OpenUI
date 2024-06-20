#pragma once
#include "RmGUI.h"

/// @brief Base interface of render backends.
class RMGUI_API IRmGUIRender
{
public:
	using point_t = struct
	{
		float X, Y, Z, U, V;
	};
	using face_t = struct
	{
		point_t P0, P1, P2;
	};
	using mesh_t = struct
	{
		RmArrayView<face_t> TriangleList;
	};

public:
	virtual ~IRmGUIRender() = default;

	virtual void render(RmArrayView<mesh_t> data) = 0;
};
using rmrender = RmRaw<IRmGUIRender>;