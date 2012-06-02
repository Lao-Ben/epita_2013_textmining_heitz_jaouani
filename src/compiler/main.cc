#include <iostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

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
    std::cerr << "File not found: "
	      << argv[1]
	      << std::endl;
    return 255;
  }


  PatriciaTree tree;
  std::string line;
  while (!words.eof() && !words.fail())
  {
    getline(words, line);
    size_t tab_pos;
    if (std::string::npos == (tab_pos = line.find('\t')))
      continue;
    std::string word = line.substr(0, tab_pos);
    std::string freq = line.substr(tab_pos);

    tree.insert(word.c_str(), atoi(freq.c_str()));
    //tree.display(std::cout);
    //std::cout << std::endl << std::endl << std::endl << std::endl;
  }
  //tree.display(std::cout);
  words.close();

  // Open txt dictionary
  std::ofstream dico(argv[2], std::ios::binary);
  tree.saveToDico (dico);
  dico.close();

  return 0;
}
