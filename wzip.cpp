#include <iostream>

#include <fcntl.h>
#include <cstdlib>

#include <unistd.h>
#include <vector>

using namespace std;

void file_compress(int argc, char* argv[]) {
    char current_char = '\0';
    int reoccuring_char = 0;
    bool initial_file = true;

    vector<char> buffer_out;

    for (int i = 1; i < argc; i++) {
        int fileDescriptor = open(argv[i], O_RDONLY);
        if (fileDescriptor < 0) {
            cout << "wzip: cannot open file " << argv[i] << endl;
            continue;
        }

        char buffer_in[4096];
        ssize_t bytes_read;

        while ((bytes_read = read(fileDescriptor, buffer_in, sizeof(buffer_in))) > 0) {
            for (ssize_t j = 0; j < bytes_read; ++j) {
                char ch = buffer_in[j];

                if (initial_file) {
                    current_char = ch;
                    reoccuring_char = 1;
                    initial_file = false;
                } else if (ch == current_char) {
                    reoccuring_char++;
                } else {
                    for (size_t k = 0; k < sizeof(int); ++k) {
                        buffer_out.push_back((reoccuring_char >> (k * 8)) & 0xFF);
                    }

                    buffer_out.push_back(current_char);

                    current_char = ch;
                    reoccuring_char = 1;
                }

                if (buffer_out.size() >= buffer_out.capacity()) {
                    if (write(STDOUT_FILENO, buffer_out.data(), buffer_out.size()) != static_cast<ssize_t>(buffer_out.size())) {
                        cout << "wzip: error writing to output" << endl;
                        close(fileDescriptor);
                        return;
                    }
                    buffer_out.clear();
                }
            }
        }

        if (bytes_read < 0) {
            cout << "wzip: error reading file " << argv[i] << endl;
            close(fileDescriptor);
            return;
        }

        close(fileDescriptor);
    }

    // Final Buffer Output
    if (reoccuring_char > 0) {
        for (size_t k = 0; k < sizeof(int); ++k) {
            buffer_out.push_back((reoccuring_char >> (k * 8)) & 0xFF);
        }
        buffer_out.push_back(current_char);
    }

    // Empty rest of buffer
    if (!buffer_out.empty()) {
        if (write(STDOUT_FILENO, buffer_out.data(), buffer_out.size()) != static_cast<ssize_t>(buffer_out.size())) {
            cout << "wzip: error writing to output" << endl;
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "wzip: file1 [file2 ...]" << endl;
        return 1;
    }

    file_compress(argc, argv);

    return 0;
}