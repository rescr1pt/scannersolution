#include "Scanner.h"

// relative path because it was generated
#include "../projectversion/ProjectVersion.h"

#include <fstream>

void printArgs()
{
    printf(
        "Usage \n"
        "./portscanner <config_file> \n");
}

int main(int argc, char** argv)
{
    printf("PortScanner\n"
        "Version: %u.%u\n"
        "Author: %s\n"
        "\n", PROJECT_VERSION_MAJ, PROJECT_VERSION_MIN, PROJECT_AUTHOR);

    if (argc != 2) {
        printf("Not set configuration file\n");
        printArgs();
        return 1;
    }

    Scanner scanner(argv[1]);
    scanner.run();

}
