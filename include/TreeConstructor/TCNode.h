#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include <TreeConstructor/OpcodeType.h>
#include <TreeConstructor/TCHelper.h>

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
  MethodInfo called_method_info;
  std::vector<uint32_t> opt_arg_offset;
  OpCodeType opcode_type;
  std::vector<std::shared_ptr<Node>> next_nodes;

  Node() {};
  Node(uint32_t const& _baseAddr,
       uint16_t const& _size,
	     OpCode const& _opcode,
       MethodInfo const& _called_method_info,
       uint32_t const& _internal_offset,
       std::vector<uint32_t> const& _opt_arg_offset);

  int count_node() const;
};

typedef std::function<std::string(NodeSPtr const&)> FmtLambda;
typedef std::function<std::pair<NodeSPtr, std::vector<std::pair<NodeSPtr, NodeSPtr>>>(NodeSPtr const&)> BinaryFmtLambda;
std::string dot_traversal(Node const& node,
                          FmtLambda dump_format_method);
std::pair<std::vector<NodeSPtr>, std::vector<std::pair<NodeSPtr, NodeSPtr>>>
binary_traversal(Node const& node, BinaryFmtLambda dump_format_method);


NodeSPtr construct_node_from_vec(std::vector<NodeSPtr> const &nodeptr_vector);

std::vector<NodeSPtr> get_method_call_nodes(
    std::vector<NodeSPtr> const &node_vec);

void process_calls(std::map<MethodInfo, NodeSPtr> &map,
                   std::vector<NodeSPtr> &call_node_vec);
}
