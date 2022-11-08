#pragma once

#include <filesystem>
#include <string>

namespace fileut
{
    bool createFolderRecursive(const std::string& folder, bool ignoreIfExists);

}