#include <external_sort/external_sort.h>

#include <algorithm>
#include <vector>
#include <queue>

#include <sstream>

#include <iostream>

using namespace std;

namespace ExternalSort {

    struct StringComparator {
        bool operator()(const pair<size_t, string> &lhs, const pair<size_t, string> &rhs) const {
            return lhs.second > rhs.second;
        }
    };

    using PriorityQueue = priority_queue<pair<size_t, string>, vector<pair<size_t, string>>, StringComparator>;

    void ExtractChunkHeadIfNeeded(size_t chunkIndex, vector<unique_ptr<IInputStream>> &iss,
                                  PriorityQueue &queue) {
        auto& is = iss[chunkIndex];
        if (is->eof()) {
            return;
        }

        string s;
        is->ReadString(s);
        if (!s.empty()) {
            queue.push({chunkIndex, s});
        }
    }

    void Sort(IInputStream &input, IOutputStream &output, IFileManager &fileManager, size_t ChunkSize) {
        auto inputFileSize = fileManager.OutputFileSize();
        size_t chunkQty = inputFileSize / ChunkSize + (inputFileSize % ChunkSize == 0 ? 0 : 1);

        size_t string_count = 0;

        for (size_t i = 0; i < chunkQty; ++i) {
            vector<string> strings;
            size_t bytesRead = 0;

            while (bytesRead < ChunkSize && !input.eof()) {
                string s;
                input.ReadString(s);
                bytesRead += s.size() + 1; // delimiter size
                strings.push_back(s);
                string_count += 1;
            }

            if (strings.empty()) {
                chunkQty = i;
                break;
            }

            sort(strings.begin(), strings.end());

            auto chunkOs = fileManager.CreateTmpOutput();
            for (auto &it: strings) {
                chunkOs->WriteString(it);
                chunkOs->WriteString("\n");
            }
            chunkOs->flushAndClose();
        }

        PriorityQueue chunkHeads;
        auto chunkInputs = fileManager.TmpInputs();

        for (size_t i = 0; i < chunkQty; ++i) {
            ExtractChunkHeadIfNeeded(i, chunkInputs, chunkHeads);
        }

        while (!chunkHeads.empty()) {
            auto &pair = chunkHeads.top();
            auto index = pair.first;
            auto &s = pair.second;

            output.WriteString(s);
            output.WriteString("\n");

            chunkHeads.pop();
            ExtractChunkHeadIfNeeded(index, chunkInputs, chunkHeads);
        }
        output.flushAndClose();
    }
}
