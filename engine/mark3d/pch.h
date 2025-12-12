#ifndef __PCH_H__
#define __PCH_H__

#include "predefine.h"
#include "spin_lock.h"
#include "interlock_atomic.h"
#include "unicode.h"
#include "linked_list.h"
#include "hash_table.h"
#include "Heap.h"
#include "NameHash.h"
#include "IUnknownImpl.h"
#include "AssetDef.h"
#include "IAsset.h"
#include "GlobalVars.h"
#include "Log.h"
#include "RenderDef.h"
#include "IRenderSystem.h"

#ifndef __MARK_D3D11_RENDER_SYSTEM__
#include <d3d11.h>
#include <d3d11_3.h>
#include <d3dcompiler.h>
#include <dxgiformat.h>

#endif // __MARK_D3D11_RENDER_SYSTEM__

#endif // __PCH_H__