#include "LZW.h"

LZW::LZW(std::ostream& _stream, int _minimum_code_size):
    out(_stream),
    min_code_size(_minimum_code_size)
{
    byte_index = 0;
    byte = 0;
    chunk_index = 0;

    code_size = min_code_size + 1;
    clear_code = 1 << min_code_size;
    stop_code = clear_code + 1;
    max_code = stop_code;
    
    chunk = new uint8_t[255];
    std::memset(chunk, 0, 255);

    dict = new LZWNode[4096];
};

LZW::~LZW(){
    delete [] chunk;
    delete [] dict;
}

int LZW::write_debug(const uint8_t* data, uint32_t length)
{
    int len = 0;
    for(uint32_t i=0; i < length; i++)
    {
        len += write_code(data[i], code_size);
        len += write_code(clear_code, code_size);
    }
    return len;
};

int LZW::write(const uint8_t* data, uint32_t length){
    int len = clear_dictionary();
    int32_t cur_code = -1;

    for(uint32_t i=0; i < length; i++)
    {

        //if first code in a run
        if(cur_code < 0){
            cur_code = data[i];
        }
        //if cur_code + data[i] is in the dictionary
        else if(dict[cur_code].next_value[data[i]]){
            cur_code = dict[cur_code].next_value[data[i]];
        }
        //else we've encountered a new sequence
        else {
            len += write_code(cur_code, code_size);

            //add the new code to the dictionary
            dict[cur_code].next_value[data[i]] = ++max_code;

            //if we've just moved up a code size
            if(max_code >= (1 << code_size)){
                code_size++;
            }
            //if the dictionary is full
            if(max_code == 4095){
                //simple method - clear it and begin again
                len += clear_dictionary();
            }

            //begin next run with the current value
            cur_code = data[i];
        }
    
    }

    //final code
    len += write_code(cur_code, code_size);
    
    return len;
};

int LZW::flush(){
    int len = clear_dictionary();
    len += write_code(stop_code, code_size);
    len += flush_byte();
    len += flush_chunk();
    return len;
};

int LZW::clear_dictionary(){
    for(int i = 0; i < 4096; i++){
        std::memset(dict[i].next_value, 0, 256*sizeof(uint16_t));
    }
    int len = write_code(clear_code, code_size);
    code_size = min_code_size + 1;
    max_code = stop_code;
    return len;
};

int LZW::write_code(uint32_t code, int length){
    int len = 0;
    for(int i = 0; i < length; i++){
        len += write_bit(code & (1 << i));
    }
    return len;
};

int LZW::write_bit(bool bit){
    int len = 0;
    //add the bit to the byte
    byte |= bit << byte_index++;
    
    //if we've run out of space in the byte
    if(byte_index > 7)
        len += flush_byte();
    return len;
};

int LZW::flush_byte(){
    int len = 0;
    if(byte_index){
        chunk[chunk_index++] = byte;
        byte = 0;
        byte_index = 0;
        if(chunk_index >= 255)
            len += flush_chunk();
    }
    return len;
};

int LZW::flush_chunk(){
    int len = 0;
    if(chunk_index){
        //write the chunk out to the stream
        out.put(chunk_index & 0xff);
        out.write(reinterpret_cast<char*>(chunk), chunk_index);
        len += chunk_index + 1;
        //reset the chunk
        chunk_index = 0;
        std::memset(chunk, 0, 255);
    }
    return len;
};
