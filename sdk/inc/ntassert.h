#pragma once

#include <crtdbg.h>

#ifndef NT_ASSERT
#define NT_ASSERT(cond) _ASSERTE(cond)
#endif
