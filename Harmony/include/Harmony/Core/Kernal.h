#pragma once
// Backward-compatibility header: Kernal has been renamed to Kernel.
// See Harmony/Core/Kernel.h for the current definition.
#include "Harmony/Core/Kernel.h"

namespace Harmony {
    // Type alias so that existing code referencing Kernal continues to compile.
    using Kernal = Kernel;
}
