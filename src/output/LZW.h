#ifndef GIF_AUTHOR_LZW_H
#define GIF_AUTHOR_LZW_H

#include <ostream>
#include <cstring>
#include <stdint.h>

#include <iostream>

class LZWNode {
    public:
        LZWNode() {};
        ~LZWNode() {};

        uint16_t next_value[256];
};

/**
 * A Lempel-Ziv-Welch encoder
 */
class LZW {
public:
    LZW(std::ostream& _stream, int _minimum_code_size);
    virtual ~LZW();

    /*
     * Encode and write the data to the stream
     */
    int write(const uint8_t* data, uint32_t length);

    int write_debug(const uint8_t* data, uint32_t length);

    /*
     * Flush buffer and write the end bit
     */
    int flush();

protected:
    //LZW dictionary
    LZWNode* dict; 

    int clear_dictionary();

    //partial byte and index
    uint8_t byte_index, byte, *chunk;
    uint32_t chunk_index;

    int min_code_size, code_size;
    uint32_t clear_code, stop_code, max_code;

    int write_code(uint32_t code, int length);
    int write_bit(bool bit);
    int flush_byte();
    int flush_chunk();

    std::ostream& out;

};


#endif //GIF_AUTHOR_LZW_H
