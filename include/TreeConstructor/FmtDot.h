#pragma once
#include <string>

#include <TreeConstructor/TCNode.h>

namespace Fmt
{
namespace Dot
{
  std::string get_header(TreeConstructor::Node const& node);
  std::string get_footer();
  void dump_tree(TreeConstructor::Node const& current_node);
  std::string dump_single_node(TreeConstructor::NodeSPtr const node);
}
}
