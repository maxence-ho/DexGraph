#include <fstream>
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
}
