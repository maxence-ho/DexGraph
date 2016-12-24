#include <iostream>
#include <sstream>
#include <string>

#include <TreeConstructor/TCOutputHelper.h>
#include <TreeConstructor/TCNode.h>

namespace TreeConstructor
{
Node::Node(uint32_t _baseAddr,
		   OpCode _opcode,
		   std::string _instructions)
{
	this->baseAddr = _baseAddr;
	this->opcode = _opcode;
	this->instructions = _instructions;
}

void Node::pretty_print() const
{
	// Print current node
	auto node_str = std::basic_stringstream<char>();
	node_str << "------------" << "\n";
	node_str << "base address: " << this->baseAddr << "\n";
	node_str << "opcode: 0x" << std::hex << std::stoi(std::to_string(this->opcode)) << "\n";
	node_str << "instructions: " << this->instructions << "\n";
	node_str << "------------\n" << "\n";
	Helper::write(Helper::classlist_filename, node_str.str());
	// Recursively print children
	if (this->next_nodes == nullptr)
	{
		return;
	}
	else
	{
		this->next_nodes->pretty_print();
	}
}

void Node::dot_fmt_dump() const
{
	// Print graph description
	auto constexpr tab_str = "  ";
	std::stringstream dot_str;
	dot_str << "digraph {\n";
	dot_str << tab_str << "label=\"" << std::hex << this->baseAddr << "\"\n";
	auto current_node = this;
	while (current_node != nullptr)
	{
		dot_str << tab_str << "\"0x" << std::hex << current_node->baseAddr << "\"";
		auto const current_node_optype = OpCodeClassifier::get_opcode_type(current_node->opcode);
		dot_str << "[label=\"" << std::hex << OpCodeTypeToStrMap.find(current_node_optype)->second << "\"];\n";
		auto const next_node = current_node->next_nodes.get();
		if (next_node != nullptr)
		{
			dot_str << tab_str << "\"0x" << std::hex << current_node->baseAddr << "\"";
			dot_str << " -> ";
			dot_str << "\"0x" << std::hex << next_node->baseAddr << "\";\n";
		}
		current_node = next_node;
	}
	dot_str << "}\n";
	printf("%s", dot_str.str().c_str());
	//Helper::write(Helper::graph_filename, dot_str.str());
}

void Node::copy(Node const& node)
{
	this->baseAddr = node.baseAddr;
	this->opcode = node.opcode;
	this->instructions = node.instructions;
}

void append_node_to(Node& parent_node,
        			Node const& child_node)
{
	auto new_child_ptr = std::unique_ptr<Node>(new Node(child_node.baseAddr, 
			                      						child_node.opcode,
													    child_node.instructions));
	if (parent_node.baseAddr == -1)
	{
		parent_node.copy(child_node);
		return;
	}
		
	if (parent_node.next_nodes == nullptr)
		parent_node.next_nodes = std::move(new_child_ptr);
	else
		append_node_to(*parent_node.next_nodes.get(),
					   child_node);
}

}
