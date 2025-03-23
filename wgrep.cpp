#include <iostream>
#include <string>

#include <fcntl.h>
#include <cstdlib>
#include <cstring>

#include <unistd.h>


using namespace std;

void search(const string &searchTerm, int fileDescriptor, bool &Match) {
    char buffer[4096];
    string line;
    ssize_t bytesRead;

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; ++i) {
            if (buffer[i] == '\n') {
                if (line.find(searchTerm) != string::npos) {
                    cout << line << endl;
                    Match = true;
                }
                line.clear();
            } else {
                line += buffer[i];
            }
        }
    }

    if (bytesRead < 0) {
        cout << "wgrep: error reading file" << endl;
    }

    if (!line.empty() && line.find(searchTerm) != string::npos) {
        cout << line << endl;
        Match = true;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "wgrep: searchterm [file ...]" << endl;
        return 1;
    }

    string searchTerm = argv[1];
    bool Match = false;

    if (argc == 2) {
        search(searchTerm, STDIN_FILENO, Match);
    } else {
        for (int i = 2; i < argc; ++i) {
            int fileDescriptor = open(argv[i], O_RDONLY);

            if (fileDescriptor < 0) {
                cout << "wgrep: cannot open file" << endl;
                return 1;
            }

            search(searchTerm, fileDescriptor, Match);

            if (close(fileDescriptor) < 0) {
                cout << "wgrep: error closing file" << endl;
                return 1;
            }
        }
    }
    return 0;
}

