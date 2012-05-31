#include <iostream>
#include <fstream>

#include "../patricia_tree/patricia_tree.hh"

int main(int argc, char** argv)
{
  // Check parameters
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0]
	      << " /path/to/word/freq.txt /path/to/output/dict.bin"
	      << std::endl;
    return 1;
  }



  // Open txt dictionary
  std::ifstream words(argv[1]);
  if (!words.is_open())
  {
    std::cout << "File not found: "
	      << argv[1]
	      << std::endl;
    return 255;
  }


  PatriciaTree tree;

  // Fixme : Insert all the words !


  // Open txt dictionary
  std::ofstream dico(argv[2]);
  tree.saveToDico (dico);
  dico.close();

  return 0;
}
