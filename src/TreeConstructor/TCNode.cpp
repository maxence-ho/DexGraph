#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <assert.h>

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

  dot_ss << dot_traversal(*current_node);

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

namespace
{
  template<typename T>
  bool find(std::vector<T> vec, T value)
  {
    return std::find(vec.begin(), vec.end(), value) != std::end(vec);
  }

  // Called in a loop to traverse the tree from current_node
  // descending via the leftest node each time
  // and adding it to visiting_tack iif node has not been visited yet
  void left_traversal_stack(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                            std::vector<NodeSPtr> & visited_nodeptr_vec,
                            NodeSPtr & current_node)
  {
    do
    {
      if (!find<NodeSPtr>(visiting_nodeptr_stack, current_node))
      {
        visiting_nodeptr_stack.push_back(current_node);
        auto const new_current_node = current_node->next_nodes[0];
        if (!find<NodeSPtr>(visited_nodeptr_vec, new_current_node))
          current_node = new_current_node;
      }
      else
      {
        break;
      }
    } while (!current_node->next_nodes.empty());
    // Finally add leaf node
    if (!find<NodeSPtr>(visiting_nodeptr_stack, current_node))
      visiting_nodeptr_stack.push_back(current_node);
  }

  // Called in a loop to destack all the nodes in visiting_stack
  // with only 1 next_nodes
  void destack_and_dump_node(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                             std::vector<NodeSPtr> & visited_nodeptr_vec, 
                             std::stringstream & dot_ss)
  {
    auto const popped_node = visiting_nodeptr_stack.back();
    visiting_nodeptr_stack.pop_back();
    if (!find<NodeSPtr>(visited_nodeptr_vec, popped_node))
    {
      dot_ss << dot_fmt_node(popped_node); // Visitor operation
      visited_nodeptr_vec.push_back(popped_node);
    }
  }

  // Called when all the next_nodes of visiting_stack.back()
  // are already visited => Destack and move cursor up the stack
  void process_cuttedfeet_node(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                               std::vector<NodeSPtr> & visited_nodeptr_vec,
                               std::stringstream & dot_ss,
                               NodeSPtr & current_node)
  {
    destack_and_dump_node(visiting_nodeptr_stack,
                          visited_nodeptr_vec,
                          dot_ss);
    // Move current_node cursor
    if (!visiting_nodeptr_stack.empty())
      current_node = visiting_nodeptr_stack.back();
  }

  // Called when destacking stop
  // ie. visiting_stack.back() has multiple next_nodes
  void process_multiplefeet_node(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                                 std::vector<NodeSPtr> & visited_nodeptr_vec,
                                 std::stringstream & dot_ss,
                                 NodeSPtr & current_node)
  {
   
    {
      auto const right_node = visiting_nodeptr_stack.back()->next_nodes[1];
      if (!find<NodeSPtr>(visited_nodeptr_vec, right_node))
        current_node = right_node;
      else
        process_cuttedfeet_node(visiting_nodeptr_stack,
                                visited_nodeptr_vec,
                                dot_ss,
                                current_node);
    }
  }
}

std::string dot_traversal(Node const& node)
{
  std::stringstream dot_ss;
  std::vector<NodeSPtr> visiting_nodeptr_stack;
  std::vector<NodeSPtr> visited_nodeptr_vec;
  // Step 1: Initialized current node as root
  NodeSPtr current_node = std::make_shared<Node>(node);
  // Step 2: Push current node to S 
  // and set current = current->left until no child
  do
  {
    // Lonely Nodes
    if (current_node->next_nodes.empty())
      break;
   
    left_traversal_stack(visiting_nodeptr_stack, 
                         visited_nodeptr_vec,
                         current_node);
    
    // Step 3: If no childs and stack is not empty
    // a) Pop the top item from the stack
    // b) Do visitor operation, and set current_node = popped_item->right
    // c) Go to Step 2
    while (!visiting_nodeptr_stack.empty()
      && visiting_nodeptr_stack.back()->next_nodes.size() < 2)
    {
      destack_and_dump_node(visiting_nodeptr_stack,
                            visited_nodeptr_vec,
                            dot_ss);
    }

    if (!visiting_nodeptr_stack.empty())
    {
      process_multiplefeet_node(visiting_nodeptr_stack,
                                visited_nodeptr_vec,
                                dot_ss,
                                current_node);
    }
     
  } while (!visiting_nodeptr_stack.empty());
  return dot_ss.str();
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
        auto const true_branch_it = 
          cluster_map.find(elem.second.back()->intern_offset
            + elem.second.back()->size);
        if (true_branch_it != std::end(cluster_map))
          elem.second.back()->next_nodes.push_back(
              true_branch_it->second.front());
        // Append false branch
        auto const false_branch_it =
          cluster_map.find(elem.second.back()->opt_arg_offset);
        if (false_branch_it != std::end(cluster_map))
        {
          elem.second.back()->next_nodes.push_back(
            false_branch_it->second.front());
        }
      }
    }
  }

  void process_jmp_clusters(
    std::map < uint32_t, std::vector<NodeSPtr>> const& cluster_map)
  {
    typedef uint32_t Offset;
    std::vector<std::pair<NodeSPtr, Offset>> jmp_vector;
    for (auto const& cluster : cluster_map)
    {
      auto const last_nodeptr = cluster.second.back();
      // FIXME
      if (last_nodeptr->opcode == OpCode::OP_SPARSE_SWITCH
        || last_nodeptr->opcode == OpCode::OP_PACKED_SWITCH)
      {
        // Switch link implementation is not trivial
        break;
      }

      if (last_nodeptr->opcode_type == OpCodeType::JMP)
      {
        jmp_vector.push_back(
          std::make_pair(last_nodeptr, last_nodeptr->opt_arg_offset));
      }
    }
    // Link all jmp to target
    for (auto const& elem : jmp_vector)
    {
      for (auto const& cluster : cluster_map)
      {
        auto cluster_vector = cluster.second;
        auto const equal_it = 
          std::find_if(cluster_vector.begin(), cluster_vector.end(),
            [&](NodeSPtr const& node) {
              return elem.second == node->intern_offset;
            });
        if (equal_it != std::end(cluster_vector))
        {
          elem.first->next_nodes.push_back(
            cluster_vector[equal_it - cluster_vector.begin()]);
          break;
        }
      }
    }
  }
}

NodeSPtr construct_node_from_vec(std::vector<NodeSPtr> const &nodeptr_vector)
{
  auto cluster_map = get_node_clusters(nodeptr_vector);
  process_if_clusters(cluster_map);
  process_jmp_clusters(cluster_map);
  return cluster_map[0x0000].front();
}

}

