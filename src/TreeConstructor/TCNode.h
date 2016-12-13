#pragma once

#include <string>
#include <vector>
#include <memory>

namespace TreeConstructor
{
struct Node
{
  uint32_t baseAddr = -1;
  char opcode = 0;
  std::string instructions;
  std::unique_ptr<Node> next_nodes = nullptr;

  Node() {};
  Node(uint32_t _baseAddr,
	   char _opcode,
	   std::string _instructions);

  void copy(Node const& node);
  void pretty_print() const;
  void dot_fmt_dump() const;
};

void append_node_to(Node& parent_node,
				    Node const& child_node);
}
