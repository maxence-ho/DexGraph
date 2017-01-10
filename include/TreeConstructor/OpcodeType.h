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
  SWITCH,
	THROW,
	SYSCALL,
	RET,
};

std::string OpCodeTypeToStr(OpCodeType const& opcodetype);

namespace OpCodeClassifier
{
	bool is_if(OpCode const& candidate);
	bool is_call(OpCode const& candidate);
	bool is_jmp(OpCode const& candidate);
  bool is_switch(OpCode const& candidate);
	bool is_exception(OpCode const& candidate);
	bool is_ret(OpCode const& candidate);
  bool is_new(OpCode const & candidate);
	OpCodeType get_opcode_type(OpCode const& opcode);
}
