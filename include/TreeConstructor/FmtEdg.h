#pragma once
#include <string>

#include <TreeConstructor/TCNode.h>

namespace Fmt
{
namespace Edg
{
  void dump_all(
    std::vector<TreeConstructor::NodeSPtr> const& nodesptr_vec,
    std::vector<std::pair<TreeConstructor::NodeSPtr,
                          TreeConstructor::NodeSPtr>> const& edges_vec);
  
  std::pair<TreeConstructor::NodeSPtr,
            std::vector<std::pair<TreeConstructor::NodeSPtr, TreeConstructor::NodeSPtr>>>
  dump_single_node(TreeConstructor::NodeSPtr const node);
  
  void dump_edg_body(
      std::vector<TreeConstructor::NodeSPtr> const& nodesptr_vec,
      std::vector<std::pair<TreeConstructor::NodeSPtr,
                            TreeConstructor::NodeSPtr>> const& edges_vec);
}
}
