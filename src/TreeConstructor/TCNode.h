#pragma once

namespace TreeConstructor
{
struct Node
{
  int size;
  char opcode[4];
  std::string instructions;
  std::vector<uint32_t> next;
}
}
