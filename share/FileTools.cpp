#include "FileTools.h"

namespace fileut
{
    namespace fs = std::filesystem;

    bool createFolderRecursive(const std::string& folder, bool ignoreIfExists)
    {
        if (ignoreIfExists && fs::exists(folder)) {
            return true;
        }
        return fs::create_directories(folder);
    }

}

