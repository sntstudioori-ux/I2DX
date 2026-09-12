#include "Dx11library/File.hpp"

#include <limits>
#include <stdio.h>

namespace library {

namespace {

bool ReadBinaryFileFromHandle(FILE *file, std::vector<unsigned char> *data)
{
    if (!file || !data) {
        if (file) fclose(file);
        return false;
    }

    if (_fseeki64(file, 0, SEEK_END) != 0) {
        fclose(file);
        return false;
    }

    const __int64 size = _ftelli64(file);
    if (size < 0 || static_cast<unsigned long long>(size) >
                         static_cast<unsigned long long>(std::numeric_limits<unsigned int>::max()) ||
        _fseeki64(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return false;
    }

    data->resize(static_cast<size_t>(size));
    const bool ok = size == 0 || fread(&(*data)[0], 1, static_cast<size_t>(size), file) ==
                                      static_cast<size_t>(size);
    fclose(file);
    if (!ok) data->clear();
    return ok;
}

} // namespace

bool ReadBinaryFile(const char *path, std::vector<unsigned char> *data)
{
    if (!path || !data) return false;
    FILE *file = 0;
    if (fopen_s(&file, path, "rb") != 0) return false;
    return ReadBinaryFileFromHandle(file, data);
}

bool ReadBinaryFile(const wchar_t *path, std::vector<unsigned char> *data)
{
    if (!path || !data) return false;
    FILE *file = 0;
    if (_wfopen_s(&file, path, L"rb") != 0) return false;
    return ReadBinaryFileFromHandle(file, data);
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
