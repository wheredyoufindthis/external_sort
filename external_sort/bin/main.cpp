#include <external_sort/external_sort.h>

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>

using namespace std;
using namespace ExternalSort;

struct TBufferedOFileStream: IOutputStream {
    TBufferedOFileStream(size_t bufferSize, const string& filename)
            : File(fopen(filename.c_str(), "w"))
            , BufferSize(bufferSize)
            , Buffer(new char[bufferSize]) {}

    ~TBufferedOFileStream() {
        fclose(File);
    }

    void WriteString(const string& s) final {
        if (Offset + s.size() > BufferSize) {
            flush();
        }
        size_t count = s.size();
        memcpy(Buffer.get() + Offset, s.c_str(), count);
        Offset += count;
    }

    void flush() final {
        fwrite(Buffer.get(), 1, Offset, File);
        Offset = 0;
        fflush(File);
    }

private:
    FILE* File;
    size_t BufferSize;
    unique_ptr<char[]> Buffer;

    size_t Offset = 0;
};

struct TBufferedIFileStream: IInputStream {
    TBufferedIFileStream(size_t bufferSize, const string& filename)
            : File(fopen(filename.c_str(), "r"))
            , BufferSize(bufferSize)
            , Buffer(new char[bufferSize]) {}

    ~TBufferedIFileStream() {
        fclose(File);
    }

    void ReadString(string& s) final {
        if (Offset >= EndOfLine) {
            Offset = 0;
            size_t bytesRead = fread(Buffer.get(), 1, BufferSize, File);
            assert(ferror(File) == 0);
            size_t lengthOfNextPortion = 0;
            while (Buffer[bytesRead - lengthOfNextPortion] != '\n') {
                lengthOfNextPortion++;
            }
            EndOfLine = bytesRead - lengthOfNextPortion;
            if (!feof(File)) {
                fseek(File, -1 * lengthOfNextPortion + 1, SEEK_CUR);
            }
        }
        size_t count = 0;
        while (Buffer[Offset + count] != '\n') {
            count++;
        }
        s = string(Buffer.get() + Offset, count);
        Offset += count + 1;
    }

    bool eof() final {
        return static_cast<bool>(feof(File)) && Offset >= EndOfLine;
    }

private:
    FILE* File{};
    size_t BufferSize;

    unique_ptr<char[]> Buffer;
    size_t Offset = 0;
    size_t EndOfLine = 0;
};

struct TFileManager: IFileManager {
    explicit TFileManager(size_t fileSize, size_t outputChunkFileBuffer, size_t inputMergeFileBuffer)
            : FileSize(fileSize)
            , OutputChunkFileBuffer(outputChunkFileBuffer)
            , InputMergeFileBuffer(inputMergeFileBuffer) {}

    size_t OutputFileSize() final {
        return FileSize;
    }

    unique_ptr<IOutputStream> CreateTmpOutput() final {
        string tmpFileName = tmpnam(nullptr);
        TmpFileNames.push_back(tmpFileName);
        return unique_ptr<IOutputStream>(new TBufferedOFileStream(OutputChunkFileBuffer, tmpFileName));
    }

    vector<unique_ptr<IInputStream>> TmpInputs() final {
        vector<unique_ptr<IInputStream>> pointers;
        for (auto& fileName: TmpFileNames) {
            pointers.emplace_back(new TBufferedIFileStream(InputMergeFileBuffer, fileName));
        }
        return pointers;
    }

private:
    size_t FileSize;
    size_t OutputChunkFileBuffer;
    size_t InputMergeFileBuffer;
    vector<string> TmpFileNames{};
};

int main(int argc, char** argv) {
    string inputFileName = argv[1];
    string outputFileName = argv[2];

    size_t chunkSize = 5000000;
    size_t inputFileBuffer = 10000000;
    size_t outputFileBuffer = 10000000;
    size_t outputChunkFileBuffer = 10000000;
    size_t inputMergeFileBuffer = 10000000;

    ifstream in(inputFileName, ifstream::ate);
    auto fileSize = static_cast<size_t>(in.tellg());
    in.close();

    TBufferedIFileStream inputStream(inputFileBuffer, inputFileName);
    TBufferedOFileStream outputStream(outputFileBuffer, outputFileName);

    TFileManager fileManager(fileSize, outputChunkFileBuffer, inputMergeFileBuffer);

    Sort(inputStream, outputStream, fileManager, chunkSize);

    outputStream.flush();
}
