#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <sstream>

#include "../patricia_tree/patricia_tree.hh"
#include "../patricia_tree/search_result.hh"

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
    std::cerr << "File not found: "
	      << argv[1]
	      << std::endl;
    return 255;
  }


  PatriciaTree tree;
  unsigned int nbEntries = tree.loadFromDico(dico);
  std::cerr << "[Log] Read: " << nbEntries << " entries." << std::endl;
  dico.close();

  //tree.display(std::cout);

  // Read from the standard input
  std::string cmd;
  while(!std::cin.eof())
  {
    getline(std::cin, cmd);
    if (cmd.find("approx") == 0) // APPROX function
    {
      std::string param_str = cmd.substr(7);
      std::stringstream param(param_str, std::ios::in);

      unsigned int maxDistance;
      param >> maxDistance;
      if (param.fail())
	continue;

      std::string word;
      param >> word;
      if (word.size() == 0)
	continue;

      //std::cout << "maxdistance: " << maxDistance << std::endl;
      //std::cout << "word: \"" << word << "\"" << std::endl;


      std::list<SearchResult> resultCollector;
      tree.search(word.c_str(), maxDistance, resultCollector);
      resultCollector.sort(resultCompare);
      exportJSon(resultCollector, std::cout);
      std::cout << std::endl;
    }
  }

  return 0;
}
