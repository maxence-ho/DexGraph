#include <sstream>

#include <TreeConstructor/FmtEdg.h>
#include <TreeConstructor/TCHelper.h>

void tc_binary_print(std::ofstream & file, std::string const& str)
{
  if (file.good())
    file.write(str.c_str(), str.size());
}

template <typename IntType>
void tc_int_binary_print(std::ofstream & file, IntType const& enum_int)
{
  if (file.good())
  {
    auto const to_write = reinterpret_cast<const char*>(&enum_int);
    file.write(to_write,
               sizeof(IntType));
  }
}

namespace Fmt
{
namespace Edg
{
  std::string const edg_header = "GRAPHBIN";
  void dump_all(
    std::vector<TreeConstructor::NodeSPtr> const& nodesptr_vec,
    std::vector<std::pair<TreeConstructor::NodeSPtr,
                          TreeConstructor::NodeSPtr>> const& edges_vec)
  {
    using TreeConstructor::NodeSPtr;
    std::ofstream file("graph.edg", std::ios::app | std::ios::binary);
    tc_binary_print(file, "GRAPHBIN");
    file.close();
    dump_edg_body(nodesptr_vec, edges_vec);
  }

  std::pair<TreeConstructor::NodeSPtr,
            std::vector<std::pair<TreeConstructor::NodeSPtr, TreeConstructor::NodeSPtr>>>
  dump_single_node(TreeConstructor::NodeSPtr const node)
  {
    using TreeConstructor::NodeSPtr;
    std::vector<std::pair<NodeSPtr, NodeSPtr>> edges_vec;

    for (auto const& child_nodesptr : node->next_nodes)
    {
      auto const new_pair =
          std::make_pair(node, child_nodesptr);
      edges_vec.push_back(new_pair);
    }

    return std::make_pair(node, edges_vec);
  }
  
  void
  dump_node_vec(std::vector<TreeConstructor::NodeSPtr> const& nodesptr_vec)
  {
    auto const node_count = (uint32_t)nodesptr_vec.size();
    std::ofstream file("graph.edg", std::ios::app | std::ios::binary);
    tc_int_binary_print<uint32_t>(file, node_count);
    
    for (auto const& nodesptr : nodesptr_vec)
    {
      if (nodesptr == nullptr)
        break;
      tc_binary_print(file, "n");
      tc_int_binary_print<uint64_t>(file, (uint64_t)nodesptr->baseAddr);
      tc_int_binary_print<uint32_t>(file, static_cast<uint32_t>(nodesptr->opcode_type));
    }
    file.close();
  }

  void dump_edge_vec(
      std::vector <
      std::pair<TreeConstructor::NodeSPtr, TreeConstructor::NodeSPtr>> const&
          edges_vec) 
  {
    std::ofstream file("graph.edg", std::ios::app | std::ios::binary);
    for (auto const& pair : edges_vec)
    {
      if (pair.first == nullptr || pair.second == nullptr)
        break;
      
      tc_binary_print(file, "e");
      tc_int_binary_print<uint64_t>(file, (uint64_t)pair.first->baseAddr);
      tc_int_binary_print<uint64_t>(file, (uint64_t)pair.second->baseAddr);
    }
    file.close();
  }

  void dump_edg_body(
      std::vector<TreeConstructor::NodeSPtr> const& nodesptr_vec,
      std::vector<std::pair<TreeConstructor::NodeSPtr,
                            TreeConstructor::NodeSPtr>> const& edges_vec)
  {
    dump_node_vec(nodesptr_vec);
    dump_edge_vec(edges_vec);
  }
}
}
