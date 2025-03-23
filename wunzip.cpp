#include <iostream>

#include <fcntl.h>
#include <cstdlib>

#include <unistd.h>

using namespace std;

void file_decompress(int argc, char* argv[]) {
    char buffer_in[4096];
    char buffer_out[4096];
    size_t buffer_current = 0;

    for (int i = 1; i < argc; i++) {
        int fileDescriptor = open(argv[i], O_RDONLY);
        if (fileDescriptor < 0) {
            cout << "wunzip: cannot open file " << argv[i] << endl;
            continue;
        }

        ssize_t bytes_read;
        size_t read_current = 0;

        while ((bytes_read = read(fileDescriptor, buffer_in + read_current, sizeof(buffer_in) - read_current)) > 0) {
            bytes_read += read_current;
            size_t read_pos = 0;

            while (read_pos + sizeof(int) + sizeof(char) <= static_cast<size_t>(bytes_read)) {
                int reoccuring_char = 0;
                char current_char = '\0';

                for (size_t j = 0; j < sizeof(int); ++j) {
                    reoccuring_char |= static_cast<unsigned char>(buffer_in[read_pos + j]) << (j * 8);
                }
                read_pos += sizeof(int);

                current_char = buffer_in[read_pos];
                read_pos += sizeof(char);

                while (reoccuring_char > 0) {
                    size_t to_write = min(static_cast<size_t>(reoccuring_char), sizeof(buffer_out) - buffer_current);
                    for (size_t k = 0; k < to_write; ++k) {
                        buffer_out[buffer_current + k] = current_char;
                    }
                    buffer_current += to_write;
                    reoccuring_char -= to_write;

                    if (buffer_current == sizeof(buffer_out)) {
                        if (write(STDOUT_FILENO, buffer_out, sizeof(buffer_out)) != static_cast<ssize_t>(sizeof(buffer_out))) {
                            cout << "wunzip: error writing to output." << endl;
                            close(fileDescriptor);
                            return;
                        }
                        buffer_current = 0;
                    }
                }
            }

            // Empty rest of buffer
            read_current = bytes_read - read_pos;
            for (size_t j = 0; j < read_current; ++j) {
                buffer_in[j] = buffer_in[read_pos + j];
            }
        }

        if (bytes_read < 0) {
            cout << "wunzip: error reading file " << argv[i] << endl;
            close(fileDescriptor);
            return;
        }

        close(fileDescriptor);
    }

    // Final Buffer Output
    if (buffer_current > 0) {
        if (write(STDOUT_FILENO, buffer_out, buffer_current) != static_cast<ssize_t>(buffer_current)) {
            cout << "wunzip: error writing to output." << endl;
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "wunzip: file1 [file2 ...]" << endl;
        return 1;
    }

    file_decompress(argc, argv);

    return 0;
}
