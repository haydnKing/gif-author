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
    clear_dictionary();
};

LZW::~LZW(){
    delete [] chunk;
}

void LZW::write(const uint8_t* data, uint32_t length){
    std::cout << "writing " << length << " pixels" << std::endl;
    int32_t cur_code = -1;

    for(uint32_t i=0; i < length; i++){

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
            write_code(cur_code, code_size);

            //add the new code to the dictionary
            dict[cur_code].next_value[data[i]] = ++max_code;

            //if we've just moved up a code size
            if(max_code >= (1 << code_size)){
                code_size++;
            }
            //if the dictionary is full
            if(max_code == 4095){
                //simple method - clear it and begin again
                clear_dictionary();
            }

            //begin next run with the current value
            cur_code = data[i];
        }
    
    }

    //final code
    write_code(cur_code, code_size);

};

void LZW::flush(){
    std::cout << "flush()... chunk_index = " << (int)chunk_index << std::endl;
    clear_dictionary();
    write_code(stop_code, code_size);
    flush_byte();
    flush_chunk();
};

void LZW::clear_dictionary(){
    std::cout << "clear_dictionary()"<< std::endl;
    for(int i = 0; i < 4096; i++){
        std::memset(dict[i].next_value, 0, 256);
    }
    write_code(clear_code, code_size);
    code_size = min_code_size + 1;
    max_code = stop_code;
};

void LZW::write_code(uint32_t code, int length){
    std::cout << "write_code("<< code <<")" << std::endl;
    for(int i = 0; i < length; i++){
        write_bit(code & (1 << i));
    }
};

void LZW::write_bit(bool bit){
    //add the bit to the byte
    byte |= bit << byte_index++;
    
    //if we've run out of space in the byte
    if(byte_index > 7)
        flush_byte();
};

void LZW::flush_byte(){
    if(byte_index){
        chunk[chunk_index++] = byte;
        byte = 0;
        byte_index = 0;
        if(chunk_index >= 255)
            flush_chunk();
    }
};

void LZW::flush_chunk(){
    std::cout << "flush_chunk(" << (int)chunk_index << ")" << std::endl;
    if(chunk_index){
        //write the chunk out to the stream
        out.put(chunk_index & 0xff);
        out.write(reinterpret_cast<char*>(chunk), chunk_index);
        //reset the chunk
        chunk_index = 0;
        std::memset(chunk, 0, 255);
    }
};
