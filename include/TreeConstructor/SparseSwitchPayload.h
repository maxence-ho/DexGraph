#pragma once

#include <vector>

struct SparseSwitchPayload
{
  unsigned short const ident = 0x0200;
  unsigned short size = 0;
  std::vector<int> keys;
  std::vector<int> targets;
};
