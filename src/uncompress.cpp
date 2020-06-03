/**
 * TODO: file header
 *
 * Author:
 */
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: Pseudo decompression with ascii encoding and naive header (checkpoint)
 */
void pseudoDecompression(const string& inFileName, const string& outFileName) {
    string line;
    vector<unsigned int> frequencyVec(256, 0);
    ifstream inputfile(inFileName);
    if (inputfile.is_open()) {
        for (int i = 0; i < 256; i++) {
            getline(inputfile, line);
            frequencyVec[i] = stoi(line);
        }
    }
    HCTree hcTree;
    hcTree.build(frequencyVec);

    remove(outFileName.c_str());
    ofstream outputfile(outFileName);
    if (inputfile.is_open() && outputfile.is_open()) {
        while (!inputfile.eof() && inputfile.peek() >= '0') {
            outputfile.put((char)hcTree.decode(inputfile));
        }
        inputfile.close();
        outputfile.close();
    }
}

/* TODO: True decompression with bitwise i/o and small header (final) */
void trueDecompression(const string& inFileName, const string& outFileName) {
    char c;
    ifstream inputfile(inFileName);
    ofstream outputfile(outFileName);
    HCTree hcTree;
    hcTree.load(inputfile);
    char last_byte_bits;
    inputfile.get(last_byte_bits);

    stringstream bytess;
    stringstream datass;
    while (inputfile.get(c)) {
        int bits_to_read = (inputfile.peek()) == EOF ? last_byte_bits : 8;
        bytess.put(c);
        BitInputStream in(bytess, 1);
        while (bits_to_read > 0) {
            int bit = in.readBit();
            if (in.eof()) break;
            datass.put(bit ? '1' : '0');
            bits_to_read--;
        }
    }
    string fds = datass.str();
    while (!datass.eof() && (datass.peek() == '0' || datass.peek() == '1')) {
        c = (char)hcTree.decode(datass);
        outputfile.put(c);
    }
    char cm;
}

/* Main program that runs the decompression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Uncompresses files using Huffman Encoding");
    options.positional_help(
        "./path_to_compressed_input_file ./path_to_output_file");

    bool isAscii = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Read input in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAscii))("input", "",
                                       cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit.");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // if compressed file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        exit(0);
    }

    if (isAscii) {
        pseudoDecompression(inFileName, outFileName);
    } else {
        trueDecompression(inFileName, outFileName);
    }

    return 0;
}
