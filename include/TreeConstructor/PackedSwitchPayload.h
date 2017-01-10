#pragma once

#include <vector>

struct PackedSwitchPayload
{
  unsigned short const ident = 0x0100;
  unsigned short size = 0;
  int first_key = 0;
  std::vector<int> targets;
};
