/**
 * TODO: file header
 *
 * Author:
 */
#include <stdio.h>

#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: add pseudo compression with ascii encoding and naive header
 * (checkpoint) */
void pseudoCompression(const string& inFileName, const string& outFileName) {
    char c;
    vector<unsigned int> frequencyVec(256, 0);
    ifstream inputfile(inFileName);
    if (inputfile.is_open()) {
        while (inputfile.get(c)) {
            frequencyVec[(unsigned char)c]++;
        }
    }

    HCTree hcTree;
    hcTree.build(frequencyVec);

    remove(outFileName.c_str());
    ofstream outputfile(outFileName);
    inputfile.clear();
    inputfile.seekg(0, ios::beg);
    if (inputfile.is_open() && outputfile.is_open()) {
        for (int i = 0; i < frequencyVec.size(); ++i) {
            char buffer[33];
            snprintf(buffer, sizeof(buffer), "%d", frequencyVec[i]);
            outputfile.write(buffer, strlen(buffer));
            outputfile.put('\n');
        }

        while (inputfile.get(c)) {
            hcTree.encode(c, outputfile);
        }
        inputfile.close();
        outputfile.close();
    }
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    remove(outFileName.c_str());
    ifstream inputfile(inFileName);
    ofstream outputfile(outFileName);
    BitOutputStream out(outputfile, 4000);
    char c;
    vector<unsigned int> frequencyVec(256, 0);

    if (inputfile.is_open()) {
        while (inputfile.get(c)) {
            frequencyVec[(unsigned char)c]++;
        }
    }

    HCTree hcTree;
    hcTree.build(frequencyVec);

    stringstream ss;
    hcTree.dump(ss);
    string erg = ss.str();

    inputfile.clear();
    inputfile.seekg(0, ios::beg);
    if (inputfile.is_open() && outputfile.is_open()) {
        stringstream sss;
        hcTree.dump(outputfile);
        char last_byte_bits = 0;
        stringstream data_strstream;
        while (inputfile.get(c)) {
            hcTree.encode(c, data_strstream);
        }
        last_byte_bits = data_strstream.tellp() % 8;
        outputfile.put(last_byte_bits);
        while (data_strstream.get(c)) {
            out.writeBit((c == '1') ? 1 : 0);
        }
        out.flush();
        inputfile.close();
        outputfile.close();
    }
}

/* Main program that runs the compression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Compresses files using Huffman Encoding");
    options.positional_help("./path_to_input_file ./path_to_output_file");

    bool isAsciiOutput = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Write output in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAsciiOutput))(
        "input", "", cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        return 0;
    }

    // if original file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        return 0;
    }

    if (isAsciiOutput) {
        pseudoCompression(inFileName, outFileName);
    } else {
        trueCompression(inFileName, outFileName);
    }

    return 0;
}