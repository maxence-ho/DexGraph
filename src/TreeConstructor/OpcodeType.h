#pragma once

#include <map>

#include <libdex/OpCode.h>

enum class OpCodeType
{
	SEQ,
	IF,
	CALL,
	NEW,
	JMP,
	THROW,
	SYSCALL,
	RET,
};

std::map<OpCodeType, std::string> const OpCodeTypeToStrMap = {
	{OpCodeType::SEQ, "SEQ"},
	{OpCodeType::IF, "IF"},
	{OpCodeType::CALL, "CALL"},
	{OpCodeType::NEW, "NEW"},
	{OpCodeType::JMP, "JMP"},
	{OpCodeType::THROW, "THROW"},
	{OpCodeType::SYSCALL, "SYSCALL"},
	{OpCodeType::RET, "RET"},
};

namespace OpCodeClassifier
{
	bool is_if(OpCode const& candidate);
	bool is_call(OpCode const& candidate);
	bool is_jmp(OpCode const& candidate);
	bool is_exception(OpCode const& candidate);
	bool is_ret(OpCode const& candidate);
  bool is_new(OpCode const & candidate);
	OpCodeType get_opcode_type(OpCode const& opcode);
}