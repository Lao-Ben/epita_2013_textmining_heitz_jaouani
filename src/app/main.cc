#include <iostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "../patricia_tree/patricia_tree.hh"

int main(int argc, char** argv)
{
  // Check parameters
  if (argc < 2)
  {
    std::cout << "Usage: " << argv[0]
	      << " /path/to/compiled/dict.bin"
	      << std::endl;
    return 1;
  }



  // Open txt dictionary
  std::ifstream dico(argv[1], std::ios::binary);
  if (!dico.is_open())
  {
    std::cout << "File not found: "
	      << argv[1]
	      << std::endl;
    return 255;
  }


  PatriciaTree tree;
  tree.loadFromDico(dico);
  dico.close();

  //tree.display(std::cout);




  return 0;
}
