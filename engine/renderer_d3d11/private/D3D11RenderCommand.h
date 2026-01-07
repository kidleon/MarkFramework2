#pragma once

struct BASE_RENDER_COMMAND
{
	UINT32 Flags;
	LINK_NODE LinkNode;
};

struct D3D11_MESH_DRAW_COMMAND : public BASE_RENDER_COMMAND
{
};


