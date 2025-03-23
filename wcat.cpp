#include <iostream>

#include <fcntl.h>
#include <cstdlib>

#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    // Exit if no files given
    if (argc < 2) {
        return 0;
    }

    char buffer[4096];

    for (int i = 1; i < argc; i++) {
        int fileDescriptor = open(argv[i], O_RDONLY);
        
        // Exit if file opened incorrectly
        if (fileDescriptor < 0) {
            cout << "wcat: cannot open file" << endl;
            exit(1);
        }

        int bytesRead = 0;
        while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
            if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead) {
                cout << "wcat: error writing to standard output" << endl;
                close(fileDescriptor);
                exit(1);
            }
        }

        // Exit if read error
        if (bytesRead < 0) {
            cout << "wcat: error reading file " << endl;
            close(fileDescriptor);
            exit(1);
        }

        close(fileDescriptor);
    }

    return 0;
}
