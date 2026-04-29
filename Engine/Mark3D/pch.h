#ifndef __PCH_H__
#define __PCH_H__

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <format>
#include <memory_resource>

#include <d3d11_1.h>
#include <dxgi1_2.h>      // DXGI 1.2 — Flip 모델 스왑체인 (D3D11.1과 짝)
#include <d3dcompiler.h>  // 셰이더 런타임 컴파일 (D3DCompile)
#include <dxgidebug.h>
#include <dxgiformat.h>

#if defined(__MEMORY_TRACKING_ENABLED__)
#include <source_location>
#endif // __MEMORY_TRACKING_ENABLED__

#include "predefine.h"

#include "core_heap.h"
#include "core_generic.h"
#include "log.h"
#include "mathlib.h"


#endif // __PCH_H__
