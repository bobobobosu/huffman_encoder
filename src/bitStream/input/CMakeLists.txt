add_library(bit_input_stream BitInputStream.cpp BitInputStream.hpp)
target_include_directories(bit_input_stream PUBLIC .)
# target_link_libraries(huffman_encoder PUBLIC bit_input_stream bit_output_stream)
