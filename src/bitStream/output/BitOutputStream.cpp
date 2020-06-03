#include "BitOutputStream.hpp"

/**
 * TODO: Write the part of the buffer that was written by the user to the output
 * stream, and then clear the buffer to allow further use. You may use fill() to
 * zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation that
 * may cause a timeout.
 */
void BitOutputStream::flush() {
    // complete part
    for (int i = 0; i < (nbits / 8); i++) {
        out.put(buf[i]);
        buf[i] = 0;
    }

    // partial part
    if (nbits % 8 != 0) {
        out.put(buf[nbits / 8]);
        buf[nbits / 8] = 0;
    }
    nbits = 0;
}

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
void BitOutputStream::writeBit(unsigned int i) {
    if (nbits == bufSize * 8) {
        for (int i = 0; i < (nbits / 8); i++) {
            out.put(buf[i]);
            buf[i] = 0;
        }
        nbits = 0;
    }
    int byteIdx = nbits / 8;
    int bitIdx = nbits % 8;
    buf[byteIdx] |= i << (7 - bitIdx);
    nbits++;
}
BitOutputStream::~BitOutputStream() { free(buf); }
