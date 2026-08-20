#include "Dx11library/File.hpp"

#include <stdio.h>

namespace library {

bool ReadBinaryFile(const char *path, std::vector<unsigned char> *data)
{
    if (!path || !data) return false;
    FILE *file = 0;
    if (fopen_s(&file, path, "rb") != 0) return false;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return false; }
    long size = ftell(file);
    if (size < 0 || fseek(file, 0, SEEK_SET) != 0) { fclose(file); return false; }
    data->resize(static_cast<unsigned int>(size));
    const bool ok = size == 0 || fread(&(*data)[0], 1, size, file) == static_cast<size_t>(size);
    fclose(file);
    if (!ok) data->clear();
    return ok;
}

bool WriteBinaryFile(const char *path, const void *data, unsigned int size)
{
    if (!path || (size && !data)) return false;
    FILE *file = 0;
    if (fopen_s(&file, path, "wb") != 0) return false;
    const bool ok = size == 0 || fwrite(data, 1, size, file) == size;
    fclose(file);
    return ok;
}

} // namespace library
