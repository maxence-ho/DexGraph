#pragma once
#include <string>
#include <sstream>
#include <fstream>

#include <TreeConstructor/PackedSwitchPayload.h>
#include <TreeConstructor/SparseSwitchPayload.h>

#define tc_str_format(buff_str, ...) { \
	char buff[256]; \
	snprintf(buff, 256, __VA_ARGS__); \
	buff_str = buff; \
}  \


namespace TreeConstructor
{
namespace Helper
{
auto constexpr classlist_filename = "class_list.txt";
auto constexpr graph_filename = "graph.dot";

void write(std::basic_string<char> const& filename,
           std::basic_string<char> const& content);
}
PackedSwitchPayload get_packed_switch_payload(int swicth_offset,
                                              intptr_t payload_addr);

SparseSwitchPayload get_sparse_switch_offsets(int swicth_offset,
                                              intptr_t payload_addr);
}
