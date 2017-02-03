#include <sstream>
#include <string>
#include <utility>
#include <assert.h>

#include <TreeConstructor/TCHelper.h>
#include <TreeConstructor/TCNode.h>

namespace TreeConstructor
{
Node::Node(uint32_t const& _baseAddr,
           uint16_t const& _size,
		       OpCode const& _opcode,
					 MethodInfo const& _called_method_info,
           uint32_t const& _internal_offset,
           std::vector<uint32_t> const& _opt_arg_offset)
{
	this->baseAddr = _baseAddr;
  this->size = _size;
	this->opcode = _opcode;
	this->called_method_info = _called_method_info;
  this->intern_offset = _internal_offset;
  this->opt_arg_offset = _opt_arg_offset;
  this->opcode_type = OpCodeClassifier::get_opcode_type(_opcode);
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
  // and adding it to visiting_stack iif node has not been visited yet
  void left_traversal_stack(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                            std::vector<NodeSPtr> & visited_nodeptr_vec,
                            NodeSPtr & current_node)
  {
    if (current_node->next_nodes.empty())
    {
      if (!find<NodeSPtr>(visiting_nodeptr_stack, current_node))
        visiting_nodeptr_stack.push_back(current_node);
      return;
    }
    
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
                             std::stringstream & dot_ss,
														 FmtLambda dump_format_method)
  {
    auto const popped_node = visiting_nodeptr_stack.back();
    visiting_nodeptr_stack.pop_back();
    if (!find<NodeSPtr>(visited_nodeptr_vec, popped_node))
    {
      dot_ss << dump_format_method(popped_node); // Visitor operation
      visited_nodeptr_vec.push_back(popped_node);
    }
  }

	std::pair<NodeSPtr, std::vector<std::pair<NodeSPtr, NodeSPtr>>>
  binary_destack_and_dump_node(std::vector<NodeSPtr> &visiting_nodeptr_stack,
                               std::vector<NodeSPtr> &visited_nodeptr_vec,
                               BinaryFmtLambda dump_format_method)
	{
		std::pair<NodeSPtr, std::vector<std::pair<NodeSPtr, NodeSPtr>>> pair;
    auto const popped_node = visiting_nodeptr_stack.back();
    visiting_nodeptr_stack.pop_back();
    if (!find<NodeSPtr>(visited_nodeptr_vec, popped_node))
    {
      pair = dump_format_method(popped_node); // Visitor operation
      visited_nodeptr_vec.push_back(popped_node);
    }
		return pair; 
	}

  NodeSPtr
  get_next_unvisited_child(std::vector<NodeSPtr> &visiting_nodeptr_stack,
                           std::vector<NodeSPtr> &visited_nodeptr_vec) 
	{
    if (visiting_nodeptr_stack.back()->next_nodes.empty())
      return nullptr;
    auto const children_next_nodes = visiting_nodeptr_stack.back()->next_nodes;
    auto next_child_it = std::find_if(
        children_next_nodes.begin(), children_next_nodes.end(),
        [&](auto const nodesptr) {
          auto const search_visiting_it = std::find_if(
              visiting_nodeptr_stack.begin(), visiting_nodeptr_stack.end(),
              [&](auto const visiting_nodesptr) {
                return nodesptr->baseAddr == visiting_nodesptr->baseAddr;
              });
          return !find<NodeSPtr>(visited_nodeptr_vec, nodesptr) &&
                 search_visiting_it == std::end(visiting_nodeptr_stack);
        });
    if (next_child_it == std::end(children_next_nodes))
      return nullptr;
    else
      return *next_child_it;
  }

  // Called when all the next_nodes of visiting_stack.back()
  // are already visited => Destack and move cursor up the stack
  void process_cuttedfeet_node(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                               std::vector<NodeSPtr> & visited_nodeptr_vec,
                               std::stringstream & dot_ss,
                               NodeSPtr & current_node,
															 FmtLambda dump_format_method)
  {
    destack_and_dump_node(visiting_nodeptr_stack,
                          visited_nodeptr_vec,
                          dot_ss,
													dump_format_method);
    // Move current_node cursor
    if (!visiting_nodeptr_stack.empty())
      current_node = visiting_nodeptr_stack.back();
  }

  std::pair<NodeSPtr, std::vector<std::pair<NodeSPtr, NodeSPtr>>>
  binary_process_cuttedfeet_node(std::vector<NodeSPtr> &visiting_nodeptr_stack,
                                 std::vector<NodeSPtr> &visited_nodeptr_vec,
                                 NodeSPtr &current_node,
                                 BinaryFmtLambda dump_format_method) 
	{
    auto const pair = binary_destack_and_dump_node(
        visiting_nodeptr_stack, visited_nodeptr_vec, dump_format_method);
    // Move current_node cursor
    if (!visiting_nodeptr_stack.empty())
      current_node = visiting_nodeptr_stack.back();
		return pair;
	}

  // Called when destacking stop
  // ie. visiting_stack.back() has multiple next_nodes
  void process_multiplefeet_node(std::vector<NodeSPtr> & visiting_nodeptr_stack,
                                 std::vector<NodeSPtr> & visited_nodeptr_vec,
                                 std::stringstream & dot_ss,
                                 NodeSPtr & current_node,
																 FmtLambda dump_format_method)
  {
    auto const next_child =
        get_next_unvisited_child(visiting_nodeptr_stack, visited_nodeptr_vec);
    if (next_child != nullptr)
      current_node = next_child;
    else
      process_cuttedfeet_node(visiting_nodeptr_stack,
                              visited_nodeptr_vec,
                              dot_ss,
                              current_node,
															dump_format_method);
  }

  std::pair<NodeSPtr, std::vector<std::pair<NodeSPtr, NodeSPtr>>>
  binary_process_multiplefeet_node(
      std::vector<NodeSPtr> &visiting_nodeptr_stack,
      std::vector<NodeSPtr> &visited_nodeptr_vec, NodeSPtr &current_node,
      BinaryFmtLambda dump_format_method) 
	{
    auto const next_child =
        get_next_unvisited_child(visiting_nodeptr_stack, visited_nodeptr_vec);
    if (next_child != nullptr)
		{
      current_node = next_child;
      std::pair<NodeSPtr, std::vector<std::pair<NodeSPtr, NodeSPtr>>>
          empty_pair;
      return empty_pair;
		}
		else
      return binary_process_cuttedfeet_node(visiting_nodeptr_stack,
                                            visited_nodeptr_vec,
																					 	current_node,
                                            dump_format_method);
  }

}

std::string dot_traversal(Node const& node, 
													FmtLambda dump_format_method)
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
      auto const search_visited_it =
          std::find_if(visited_nodeptr_vec.begin(), visited_nodeptr_vec.end(),
                       [&](auto const visited_nodesptr) {
                         return visiting_nodeptr_stack.back()->baseAddr ==
                                visited_nodesptr->baseAddr;
                       });
      if (search_visited_it == std::end(visited_nodeptr_vec))
        destack_and_dump_node(visiting_nodeptr_stack,
					 										visited_nodeptr_vec,
                              dot_ss,
															dump_format_method);
      else
        visiting_nodeptr_stack.pop_back();
    }

    if (!visiting_nodeptr_stack.empty())
    {
      process_multiplefeet_node(visiting_nodeptr_stack,
                                visited_nodeptr_vec,
                                dot_ss,
                                current_node,
																dump_format_method);
    }
     
  } while (!visiting_nodeptr_stack.empty());
  return dot_ss.str();
}

std::pair<std::vector<NodeSPtr>, std::vector<std::pair<NodeSPtr, NodeSPtr>>>
binary_traversal(Node const& node, BinaryFmtLambda dump_format_method) 
{
	// ret locals
	std::vector<NodeSPtr> nodesptr_vec;
	std::vector<std::pair<NodeSPtr, NodeSPtr>> edges_vec;

  std::stringstream dot_ss;
  std::vector<NodeSPtr> visiting_nodeptr_stack;
  std::vector<NodeSPtr> visited_nodeptr_vec;
  // Step 1: Initialized current node as root
  NodeSPtr current_node = std::make_shared<Node>(node);
  // Step 2: Push current node to S 
  // and set current = current->left until no child
  do
  {
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
      auto const search_visited_it =
          std::find_if(visited_nodeptr_vec.begin(), visited_nodeptr_vec.end(),
                       [&](auto const visited_nodesptr) {
                         return visiting_nodeptr_stack.back()->baseAddr ==
                                visited_nodesptr->baseAddr;
                       });
      if (search_visited_it == std::end(visited_nodeptr_vec)) 
			{
        NodeSPtr destacked_node;
        std::vector<std::pair<NodeSPtr, NodeSPtr>> destacked_egdes_vec;
        std::tie(destacked_node, destacked_egdes_vec) =
            binary_destack_and_dump_node(visiting_nodeptr_stack,
                                         visited_nodeptr_vec,
                                         dump_format_method);
        // Update ret vectors
        nodesptr_vec.push_back(destacked_node);
        edges_vec.insert(edges_vec.end(), destacked_egdes_vec.begin(),
                         destacked_egdes_vec.end());
      }
		 	else
        visiting_nodeptr_stack.pop_back();
    }

    if (!visiting_nodeptr_stack.empty())
    {
			NodeSPtr destacked_node;
			std::vector<std::pair<NodeSPtr, NodeSPtr>> destacked_egdes_vec;
			std::tie(destacked_node, destacked_egdes_vec) = 
				binary_process_multiplefeet_node(visiting_nodeptr_stack,
																				 visited_nodeptr_vec,
																				 current_node,
																				 dump_format_method);

			// Update ret vectors
      if (destacked_node != nullptr)
      {
        nodesptr_vec.push_back(destacked_node);
        edges_vec.insert(edges_vec.end(),
                         destacked_egdes_vec.begin(),
                         destacked_egdes_vec.end());
      }
    }
     
  } while (!visiting_nodeptr_stack.empty());
  return std::make_pair(nodesptr_vec, edges_vec);
}

namespace
{
bool is_cluster_end_opcodetype(OpCodeType const& opcodetype)
{
  return (opcodetype == OpCodeType::IF
    || opcodetype == OpCodeType::JMP
    || opcodetype == OpCodeType::SWITCH
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
  for (auto const& cluster : cluster_map)
  {
    if (cluster.second.back()->opcode_type == OpCodeType::IF)
    {
      // Append true branch
      auto const true_branch_it = 
        cluster_map.find(cluster.second.back()->intern_offset
          + cluster.second.back()->size);
      if (true_branch_it != std::end(cluster_map))
        cluster.second.back()->next_nodes.push_back(
            true_branch_it->second.front());
      // Append false branch
      auto const false_branch_it =
        cluster_map.find(cluster.second.back()->opt_arg_offset.front());
      if (false_branch_it != std::end(cluster_map))
      {
        cluster.second.back()->next_nodes.push_back(
          false_branch_it->second.front());
      }
    }
  }
}

void process_jmp_clusters(
  std::map<uint32_t, std::vector<NodeSPtr>> const& cluster_map)
{
  typedef uint32_t Offset;
  std::vector<std::pair<NodeSPtr, Offset>> jmp_vector;
  for (auto const& cluster : cluster_map)
  {
    auto const last_nodeptr = cluster.second.back();
    if (last_nodeptr->opcode_type == OpCodeType::JMP)
    {
      jmp_vector.push_back(
        std::make_pair(last_nodeptr, last_nodeptr->opt_arg_offset.front()));
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

void process_switch_clusters(
  std::map<uint32_t, std::vector<NodeSPtr>> const& cluster_map)
{
  for (auto const& cluster : cluster_map)
  {
    auto switch_node = cluster.second.back();
    if (switch_node->opcode_type == OpCodeType::SWITCH)
    {
      auto const offset_vec = switch_node->opt_arg_offset; 
      for (auto const& offset : offset_vec)
      {
        auto const switch_branch_it = cluster_map.find(offset);
        if (switch_branch_it != std::end(cluster_map))
          switch_node->next_nodes.push_back(switch_branch_it->second.front());
      }
      // Link fallthrough case (not in the offset listed unfortunately)
      if (!offset_vec.empty())
      {
        auto const fallthrough_cluster = std::find_if(
            cluster_map.begin(), cluster_map.end(), [&](auto const pair) {
              return !find<uint32_t>(offset_vec, pair.first) &&
                     pair.first != switch_node->intern_offset;
            });
        if (fallthrough_cluster != std::end(cluster_map))
          switch_node->next_nodes.push_back(
              fallthrough_cluster->second.front());
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
  process_switch_clusters(cluster_map);
  return cluster_map[0x0000].front();
}

std::vector<NodeSPtr> get_method_call_nodes(
		std::vector<NodeSPtr> const &node_vec)
{
	std::vector<NodeSPtr> ret;
	for (auto const& nodesptr : node_vec)
	{
		if (OpCodeClassifier::get_opcode_type(nodesptr->opcode) == OpCodeType::CALL)
			ret.push_back(nodesptr);
	}
	return ret;
}

void process_calls(std::map<MethodInfo, NodeSPtr> &map,
                   std::vector<NodeSPtr> &call_node_vec)
{
  for (auto &call_node : call_node_vec)
  {
    auto const it = map.find(call_node->called_method_info);
    if (it != map.end())
      call_node->next_nodes.push_back(it->second);
  }
}
}
