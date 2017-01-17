#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include <TreeConstructor/OpcodeType.h>

namespace TreeConstructor
{
struct Node;
typedef std::shared_ptr<Node> NodeSPtr;
struct Node
{
  uint32_t baseAddr = 0;
  uint16_t size = 0;
  OpCode opcode = static_cast<OpCode>(0x00);
  uint32_t intern_offset = 0;
  std::vector<uint32_t> opt_arg_offset;
  OpCodeType opcode_type;
  std::vector<std::shared_ptr<Node>> next_nodes;

  Node() {};
  Node(uint32_t const& _baseAddr,
       uint16_t const& _size,
	     OpCode const& _opcode,
       uint32_t const& _internal_offset,
       std::vector<uint32_t> const& _opt_arg_offset);

  void pretty_print() const;
  void dot_fmt_dump() const;
  int count_node() const;
};

typedef std::function<std::string(NodeSPtr const&)> FmtLambda;
std::string dot_traversal(Node const& node,
                          FmtLambda dump_format_method);
std::string dot_fmt_node(NodeSPtr const node);

NodeSPtr construct_node_from_vec(std::vector<NodeSPtr> const &nodeptr_vector);
}
