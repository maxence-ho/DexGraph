#pragma once
#include <string>
#include <sstream>
#include <fstream>

#include <libdex/DexFile.h>
#include <TreeConstructor/PackedSwitchPayload.h>
#include <TreeConstructor/SparseSwitchPayload.h>

#define tc_str_format(buff_str, ...) { \
	char buff[256]; \
	snprintf(buff, 256, __VA_ARGS__); \
	buff_str = buff; \
}  \

#define tc_print(string) { \
  printf("%s", string.c_str()); \
}  \

namespace TreeConstructor
{
namespace Helper
{
auto constexpr classlist_filename = "class_list.txt";
auto constexpr graph_filename = "graph.dot";

void write(std::basic_string<char> const& filename,
           std::basic_string<char> const& content);

std::string get_formated_hex(int const& int_value);
}

// Switch Payload getters
PackedSwitchPayload get_packed_switch_payload(int swicth_offset,
                                              intptr_t payload_addr);

SparseSwitchPayload get_sparse_switch_offsets(int swicth_offset,
                                              intptr_t payload_addr);

// MethodInfo constructor
struct MethodInfo
{
  uint32_t method_idx = 0;

  uint16_t class_idx = 0;
  uint16_t proto_idx = 0;
  uint32_t name_idx = 0;

  std::string class_descriptor;
  std::string name;
  std::string signature;

  friend bool operator ==(MethodInfo const& lhs,
                          MethodInfo const& rhs)
  {
    return lhs.method_idx == rhs.method_idx;
  }

  friend bool operator < (MethodInfo const& lhs,
                          MethodInfo const& rhs)
  {
    return lhs.method_idx < rhs.method_idx;
  }
};


MethodInfo get_method_info(DexFile const& dex_file,
                           uint32_t const& method_idx);
}
