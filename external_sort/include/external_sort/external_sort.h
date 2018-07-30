#pragma once

#include <string>

namespace ExternalSort {
    struct IOutputStream {
        virtual ~IOutputStream() = default;
        virtual void WriteString(const std::string &s) = 0;
        virtual void flushAndClose() = 0;
    };

    struct IInputStream {
        virtual ~IInputStream() = default;
        virtual void ReadString(std::string &s) = 0;
        virtual bool eof() = 0;
    };

    struct IFileManager {
        virtual ~IFileManager() = default;
        virtual size_t OutputFileSize() = 0;
        virtual std::unique_ptr<IOutputStream> CreateTmpOutput() = 0;
        virtual std::vector<std::unique_ptr<IInputStream>> TmpInputs() = 0;
    };

    void Sort(IInputStream &is, IOutputStream &output, IFileManager &fileManager, size_t ChunkSize);
}
