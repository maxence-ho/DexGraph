#include <fstream>
#include <assert.h>

#include <TreeConstructor/TCOutputHelper.h>

namespace TreeConstructor
{
namespace Helper
{
void write(std::basic_string<char> const& filename,
  	       std::basic_string<char> const& msg)
{
	std::ofstream ofs;
	ofs.open(filename, std::ios_base::app);
	//if (ofs)
	ofs << msg << "\n";
	ofs.close();
}
}
PackedSwitchPayload get_packed_switch_payload(int switch_offset,
                                              intptr_t payload_addr)
{
  auto ret = PackedSwitchPayload();
  
  // Assert identification of data pack
  auto const candidate_id = *((const uint16_t*)payload_addr);
  assert(candidate_id == ret.ident && "Incorrect payload ident.");

  // Get payload size
  auto const size = *((const uint16_t*)payload_addr + 1)/2;
  ret.size = size;

  // Get first_key
  ret.first_key = *((const int*)payload_addr + 1);
  

  // Get offset payload (given offset are relative to switch addr)
  for (std::size_t i = 0; i < size; i++)
  {
    int const relative_offset = *((unsigned int*)payload_addr + 2 + i);
    auto const methodrelative_offset = switch_offset + relative_offset;
    ret.targets.push_back(methodrelative_offset);
  }

  // Check payload size
  assert(ret.targets.size() == size && "Incorrectly sized payload.");
  
  return ret;
}

SparseSwitchPayload get_sparse_switch_offsets(int switch_offset,
                                              intptr_t payload_addr)
{
  auto ret = SparseSwitchPayload();
  
  // Assert identification of data pack
  auto const candidate_id = *((const uint16_t*)payload_addr);
  assert(candidate_id == ret.ident && "Incorrect payload ident.");

  // Get payload size
  auto const size = *((const uint16_t*)payload_addr + 1);
  ret.size = size;

  // Get keys
  for (std::size_t i = 0; i < size; i ++)
  {
    int const key = *((const int*)payload_addr + 1 + i);
    ret.keys.push_back(key);
  }

  // Check keys size
  assert(ret.keys.size() == size && "Incorrectly sized keys payload");

  // Get offset payload (given offset are relative to switch addr)
  for (std::size_t i = 0; i < size; i++)
  {
    int const relative_offset = *((unsigned int*)payload_addr + 1 + size + i);
    auto const methodrelative_offset = switch_offset + relative_offset;
    ret.targets.push_back(methodrelative_offset);
  }

  // Check payload size
  assert(ret.targets.size() == size && "Incorrectly sized targets payload.");
  
  return ret;
}
}
