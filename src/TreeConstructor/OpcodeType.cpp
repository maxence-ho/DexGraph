#include <array>
#include <algorithm>
#include <string>

#include <TreeConstructor/OpcodeType.h>

auto constexpr if_opcodes = std::array<OpCode, 12>
{
	OP_IF_EQ,
	OP_IF_NE,
	OP_IF_LT,
	OP_IF_GE,
	OP_IF_GT,
	OP_IF_LE,
	OP_IF_EQZ,
	OP_IF_NEZ,
	OP_IF_LTZ,
	OP_IF_GEZ,
	OP_IF_GTZ,
	OP_IF_LEZ,
};

auto constexpr call_opcodes = std::array<OpCode, 10>
{
	OP_INVOKE_VIRTUAL,
	OP_INVOKE_SUPER,
	OP_INVOKE_DIRECT,
	OP_INVOKE_STATIC,
	OP_INVOKE_INTERFACE,
	OP_INVOKE_VIRTUAL_RANGE,
	OP_INVOKE_SUPER_RANGE,
	OP_INVOKE_DIRECT_RANGE,
	OP_INVOKE_STATIC_RANGE,
	OP_INVOKE_INTERFACE_RANGE,
};

auto constexpr jmp_opcodes = std::array<OpCode, 5> 
{
	OP_GOTO,
	OP_GOTO_16,
	OP_GOTO_32,
	OP_PACKED_SWITCH,
	OP_SPARSE_SWITCH,
};

auto constexpr exception_opcodes = std::array<OpCode, 1> 
{
	OP_THROW,
};

auto constexpr ret_opcodes = std::array<OpCode, 4>
{
	OP_RETURN_VOID,
	OP_RETURN,
	OP_RETURN_WIDE,
	OP_RETURN_OBJECT,
};

auto constexpr new_opcodes = std::array<OpCode, 4>
{
  OP_NEW_INSTANCE,
  OP_NEW_ARRAY,
  OP_FILLED_NEW_ARRAY,
  OP_FILLED_NEW_ARRAY_RANGE,
};

bool OpCodeClassifier::is_if(OpCode const & candidate)
{
  return (std::find(begin(if_opcodes), end(if_opcodes), candidate) !=
          std::end(if_opcodes));
}

bool OpCodeClassifier::is_call(OpCode const & candidate)
{
  return (std::find(begin(call_opcodes), end(call_opcodes), candidate) !=
          std::end(call_opcodes));
}

bool OpCodeClassifier::is_jmp(OpCode const & candidate)
{
  return (std::find(begin(jmp_opcodes), end(jmp_opcodes), candidate) !=
          std::end(jmp_opcodes));
}

bool OpCodeClassifier::is_exception(OpCode const & candidate)
{
  return (std::find(begin(exception_opcodes), end(exception_opcodes),
                    candidate) != std::end(exception_opcodes));
}

bool OpCodeClassifier::is_ret(OpCode const & candidate)
{
  return (std::find(begin(ret_opcodes), end(ret_opcodes), candidate) !=
          std::end(ret_opcodes));
}

bool OpCodeClassifier::is_new(OpCode const & candidate)
{
  return (std::find(begin(new_opcodes), end(new_opcodes), candidate) !=
          std::end(new_opcodes));
}

std::string OpCodeTypeToStr(OpCodeType const& opcodetype)
{
  switch (opcodetype)
  {
    case OpCodeType::SEQ: return "SEQ"; break;
    case OpCodeType::IF: return "IF"; break;
    case OpCodeType::CALL: return "CALL"; break;
    case OpCodeType::NEW: return "NEW"; break;
    case OpCodeType::JMP: return "JMP"; break;
    case OpCodeType::THROW: return "THROW"; break;
    case OpCodeType::SYSCALL: return "SYSCALL"; break;
    case OpCodeType::RET: return "RET"; break;
  }
}

OpCodeType OpCodeClassifier::get_opcode_type(OpCode const& opcode)
{
  if (is_if(opcode))
    return OpCodeType::IF;
  else if (is_call(opcode))
    return OpCodeType::CALL;
  else if (is_jmp(opcode))
    return OpCodeType::JMP;
  else if (is_exception(opcode))
    return OpCodeType::THROW;
  else if (is_ret(opcode))
    return OpCodeType::RET;
  else if (is_new(opcode))
    return OpCodeType::NEW;
	else
		return OpCodeType::SEQ;
}
