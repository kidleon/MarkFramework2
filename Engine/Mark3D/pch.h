#ifndef __PCH_H__
#define __PCH_H__

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <format>
#include <memory_resource>

#if defined(__MEMORY_TRACKING_ENABLED__)
#include <source_location>
#endif // __MEMORY_TRACKING_ENABLED__

#include "predefine.h"

#include "core_heap.h"
#include "core_generic.h"
#include "log.h"
#include "mathlib.h"

#include "render_def.h"

#endif // __PCH_H__
