#include <iostream>
#include <string>
#include "encoder.h"
#include "decoder.h"

void printUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "  app encode <input_jpg_path> <output_dir>" << std::endl;
    std::cout << "  app decode <input_qr_dir> <output_jpg_path>" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printUsage();
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "encode")
    {
        std::string input_path = argv[2];
        std::string output_dir = argv[3];
        encodeJpgToQr(input_path, output_dir);
    }
    else if (mode == "decode")
    {
        std::string input_dir = argv[2];
        std::string output_path = argv[3];
        decodeQrToJpg(input_dir, output_path);
    }
    else
    {
        printUsage();
        return 1;
    }

    return 0;
}