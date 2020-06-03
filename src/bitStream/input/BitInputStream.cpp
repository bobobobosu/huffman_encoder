#include "BitInputStream.hpp"

/**
 * TODO: Fill the buffer of size bufSize from the input stream.
 */
void BitInputStream::fill() {
    in.read(buf, bufSize);
    nbits = 0;
}

/**
 * TODO: Return true when the istream tried to read more bytes than the
 * remaining bytes in the file and the buffer was also fully read. Otherwise,
 * return false.
 *
 * Note that eof() is different from this function. eof() in most other istreams
 * is only true when we are trying to read past the end of the file. Whereas
 * this function should be only true when we are at the end of the file, but
 * have not read past it yet. For example:
 *
 * // some code to read all the data in the file
 * atEndOfFile(); // atEndOfFile() = true. But eof() is still false for now.
 * readBit();     // trying to read past the end of the file
 * eof();         // eof is true now
 */
bool BitInputStream::atEndOfFile() { return in.gcount() != bufSize; }

/* Returns true when trying to read past the end of the file, false otherwise */
bool BitInputStream::eof() { return eofBit; }

/**
 * TODO: Read the next bit from the bit buffer. Fills the buffer from the input
 * stream if all the bits have already been read. It should return 0 if the bit
 * read is 0, and return 1 if the bit read is 1.
 *
 * Before trying to read a bit from the buffer, if atEndOFfile() is true, set
 * the eofBit and directly return 0 (You can return any number here, since after
 * setting eofBit to true, eof() will return true and the program won’t be using
 * the returned value from readBit).
 */
unsigned int BitInputStream::readBit() {
    if (firstRead) {
        fill();
        firstRead = 0;
    } else {
        if (nbits == bufSize * 8) fill();
    }

    if ((atEndOfFile() && (nbits == in.gcount() * 8)) || in.gcount() == 0) {
        // eof
        eofBit = 1;
        return 0;
    }

    int byteIdx = nbits / 8;
    int bitIdx = nbits % 8;
    char thisByte = buf[byteIdx];
    nbits++;
    return ((unsigned char)(thisByte >> (7 - bitIdx))) % 2;
}
BitInputStream::~BitInputStream() {
    free(buf);
}
