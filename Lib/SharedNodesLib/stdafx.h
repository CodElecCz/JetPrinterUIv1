// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <process.h>
#include <time.h>

#include <map>
#include <list>
#include <vector>
#include <memory>
#include <string>

#include "Events.h"
#include "INodeException.h"
#include "Lock.h"
#include "INode.h"
#include "Timers.h"
#include "Types.h"