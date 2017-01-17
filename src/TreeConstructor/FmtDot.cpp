#include <sstream>
#include <TreeConstructor/FmtDot.h>
#include <TreeConstructor/TCHelper.h>


auto constexpr tab_str = "  ";

namespace Fmt
{
namespace Dot
{
std::string get_header(TreeConstructor::Node const& node)
{
  std::stringstream header_ss;
  header_ss << "digraph {\n";
  header_ss << tab_str << "label=\""
            << TreeConstructor::Helper::get_formated_hex(node.baseAddr)
            << "\"\n";
  return header_ss.str();
}

std::string get_footer()
{
  std::stringstream footer_ss;
  footer_ss << "}\n";
  return footer_ss.str();
}

void dump_tree(TreeConstructor::Node const& current_node)
{
  // Print graph description
  std::stringstream dot_ss;
  dot_ss << get_header(current_node);

  dot_ss << dot_traversal(current_node, dump_single_node);

  dot_ss << get_footer();
  tc_print(dot_ss.str());
}

std::string dump_single_node(TreeConstructor::NodeSPtr const node)
{
  using namespace TreeConstructor::Helper;
  std::stringstream dot_ss;
  // Current Node
  dot_ss << tab_str << "\""
    << get_formated_hex(node->baseAddr) << "\"";
  dot_ss << "[label=\""
    << OpCodeTypeToStr(node->opcode_type) << "\"];\n";
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
}
}
