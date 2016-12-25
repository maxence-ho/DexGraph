#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <TreeConstructor/TCOutputHelper.h>
#include <TreeConstructor/TCNode.h>

#define tc_print(string) { \
  printf("%s", string.c_str()); \
}  \

namespace
{
  auto constexpr tab_str = "  ";

  std::string get_formated_hex(int const& int_value)
  {
    std::stringstream s;
    s << "0x" 
      << std::setfill('0') 
      << std::setw(4)
      << std::hex 
      << int_value;
    return s.str();
  }

  std::string get_dot_header(TreeConstructor::Node const& node)
  {
    std::stringstream header_ss;
    header_ss << "digraph {\n";
    header_ss << tab_str << "label=\""
      << get_formated_hex(node.baseAddr) << "\"\n";
    return header_ss.str();
  }

  std::string get_dot_footer()
  {
    std::stringstream footer_ss;
    footer_ss << "}\n";
    return footer_ss.str();
  }
}

namespace TreeConstructor
{
Node::Node(uint32_t const& _baseAddr,
           uint16_t const& _size,
		       OpCode const& _opcode,
           uint32_t const& _internal_offset,
           uint32_t const& _opt_arg_offset)
{
	this->baseAddr = _baseAddr;
  this->size = _size;
	this->opcode = _opcode;
  this->intern_offset = _internal_offset;
  this->opt_arg_offset = _opt_arg_offset;
  this->opcode_type = OpCodeClassifier::get_opcode_type(_opcode);
}

void Node::pretty_print() const
{
	// Print current node
	auto node_str = std::basic_stringstream<char>();
	node_str << "------------" << "\n";
	node_str << "base address: " << this->baseAddr << "\n";
	node_str << "opcode: 0x" << std::hex
    << std::stoi(std::to_string(this->opcode)) << "\n";
	node_str << "------------\n" << "\n";
	Helper::write(Helper::classlist_filename, node_str.str());
	// Recursively print children
  for (auto const& node : this->next_nodes)
    node->pretty_print();
}

void Node::dot_fmt_dump() const
{
	// Print graph description
  auto current_node = this;
	std::stringstream dot_ss;
  dot_ss << get_dot_header(*current_node);

  dot_ss << dot_fmt_node(*current_node);

  dot_ss << get_dot_footer();
	tc_print(dot_ss.str());
}

int Node::count_node() const
{
  auto ret = 1;
  for (auto const& child_node : this->next_nodes)
    ret += child_node->count_node();
  return ret;
}

std::string dot_fmt_node(NodeSPtr const node)
{
  std::stringstream dot_ss;
  // Current Node
  dot_ss << tab_str << "\""
    << get_formated_hex(node->baseAddr) << "\"";
  dot_ss << "[label=\""
    << OpCodeTypeToStrMap.find(node->opcode_type)->second << "\"];\n";
  // Child fmt
  for (auto const& child_node : node->next_nodes)
  {
    // Link Child node to parent node
    dot_ss << tab_str << "\"" << get_formated_hex(node->baseAddr) << "\"";
    dot_ss << " -> ";
    dot_ss << "\"" << get_formated_hex(child_node->baseAddr) << "\";\n";
    // Recursive call
    dot_ss << dot_fmt_node(child_node);
  }

  return dot_ss.str();
}

std::string dot_fmt_node(Node const& node)
{
  std::stringstream dot_ss;
  // Current Node
  dot_ss << tab_str << "\""
    << get_formated_hex(node.baseAddr) << "\"";
  dot_ss << "[label=\""
    << OpCodeTypeToStrMap.find(node.opcode_type)->second << "\"];\n";
  // Child fmt
  for (auto const& child_node : node.next_nodes)
  {
    // Link Child node to parent node
    dot_ss << tab_str << "\"" << get_formated_hex(node.baseAddr) << "\"";
    dot_ss << " -> ";
    dot_ss << "\"" << get_formated_hex(child_node->baseAddr) << "\";\n";
    // Recursive call
    dot_ss << dot_fmt_node(child_node);
  }

  return dot_ss.str();
}

namespace
{
  bool is_cluster_end_opcodetype(OpCodeType const& opcodetype)
  {
    return (opcodetype == OpCodeType::IF
      || opcodetype == OpCodeType::JMP
      || opcodetype == OpCodeType::RET);
  }

  std::map<uint32_t, std::vector<NodeSPtr>> get_node_clusters(
    std::vector<NodeSPtr> const& nodeptr_vector)
  {
    using namespace TreeConstructor;
    std::queue<NodeSPtr> node_queue;
    for (auto const& elem : nodeptr_vector)
      node_queue.push(elem);

    std::map<uint32_t, std::vector<NodeSPtr>> ret;
    do
    {
      std::vector<NodeSPtr> cluster;
      do
      {
        // Link new node to cluster
        if (!cluster.empty())
          cluster.back()->next_nodes.push_back(node_queue.front());
        // Add new node to cluster
        cluster.push_back(node_queue.front());
        node_queue.pop();
        if (node_queue.empty()) break;
      } while (!is_cluster_end_opcodetype(cluster.back()->opcode_type));

      // Cleanup inner loop
      ret.emplace(cluster.front()->intern_offset, cluster);
    } while (!node_queue.empty());
    return ret;
  }

  void process_if_clusters(
    std::map<uint32_t, std::vector<NodeSPtr>> const& cluster_map)
  {
    for (auto const& elem : cluster_map)
    {
      if (elem.second.back()->opcode_type == OpCodeType::IF)
      {
        // Append true branch
        auto true_branch_it = 
          cluster_map.find(elem.second.back()->intern_offset
            + elem.second.back()->size);
        if (true_branch_it != std::end(cluster_map))
          elem.second.back()->next_nodes.push_back(
              true_branch_it->second.front());
        // Append false branch
        auto false_branch_it =
          cluster_map.find(elem.second.back()->opt_arg_offset);
        if (false_branch_it != std::end(cluster_map))
          elem.second.back()->next_nodes.push_back(
              false_branch_it->second.front());
      }
    }
  }
}

NodeSPtr construct_node_from_vec(std::vector<NodeSPtr> const &nodeptr_vector)
{
  auto cluster_map = get_node_clusters(nodeptr_vector);
  process_if_clusters(cluster_map);
  return cluster_map[0x0000].front();
}

}

