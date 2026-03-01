#pragma once
// Backward-compatibility header: Controller has been renamed to Extension.
#include "Harmony/Core/Extension.h"

namespace Harmony {
    // Type alias so that existing code referencing Controller continues to compile.
    using Controller = Extension;
}
