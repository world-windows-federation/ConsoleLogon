#pragma once

#include <windows.h>
#include <Shlwapi.h>

#include <Windows.Foundation.h>

#include <wrl/module.h>

#include <wil/token_helpers.h>

#include "InternalAsync.h"
#include "logoninterfaces.h"
#include "NativeString.h"
#include "RefCountedObject.h"
#include "ResultUtils.h"
#include "SimpleArray.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
