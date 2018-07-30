#include <gtest/gtest.h>
#include <external_sort/external_sort.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace ExternalSort;

struct TIStream: IInputStream {
    explicit TIStream(istream* stream): IStream(stream) {}

    void ReadString(string& s) final {
        *IStream >> s;
    }

    bool eof() final {
        return IStream->eof();
    }

    void close() final { }

private:
    istream* IStream;
};

struct TOStream: IOutputStream {
    explicit TOStream(ostream* stream): OStream(stream) {}

    void WriteString(const string& s) final {
        *OStream << s;
    }

    void flushAndClose() final {
        OStream->flush();
    }

private:
    ostream* OStream;
};

struct TMockFileManager: IFileManager {
    explicit TMockFileManager(size_t fileSize): FileSize(fileSize) {}

    size_t OutputFileSize() final {
        return FileSize;
    }

    unique_ptr<IOutputStream> CreateTmpOutput() final {
        tmpStreams.emplace_back(new stringstream);
        return unique_ptr<IOutputStream>(new TOStream(tmpStreams.back().get()));
    }

    vector<unique_ptr<IInputStream>> TmpInputs() final {
        vector<unique_ptr<IInputStream>> pointers;
        for (auto& it: tmpStreams) {
            pointers.emplace_back(new TIStream(it.get()));
        }
        return pointers;
    }

    vector<unique_ptr<stringstream>> tmpStreams;

private:
    size_t FileSize;
};


TEST(TestCaseName, TestName) {
    using namespace std;
    string s = "bb\ncc\naa\ndd\ncc\nd\nb\na\nc\ne\nf\ng\n";
    istringstream input(s);
    ostringstream output;

    TMockFileManager fileManager(s.size());

    TIStream istream(&input);
    TOStream ostream(&output);

    ExternalSort::Sort(istream, ostream, fileManager, 4);
    ostream.flushAndClose();

    vector<string> strings;
    istringstream iss(s);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(strings));

    sort(strings.begin(), strings.end());

    ostringstream imploded;
    copy(strings.begin(), strings.end(),
              ostream_iterator<string>(imploded, "\n"));

    ASSERT_EQ(output.str(), imploded.str().substr(0, imploded.str().size()));
}
