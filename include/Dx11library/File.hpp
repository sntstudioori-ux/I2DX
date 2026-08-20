#pragma once

#include <vector>

namespace library {

// Loads an external file. Archive/resource lookup is intentionally left to callers.
bool ReadBinaryFile(const char *path, std::vector<unsigned char> *data);
bool WriteBinaryFile(const char *path, const void *data, unsigned int size);

} // namespace library
