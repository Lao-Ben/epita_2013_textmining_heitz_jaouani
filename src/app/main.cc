#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <sstream>

#include "../patricia_tree/patricia_tree.hh"
#include "../patricia_tree/search_result.hh"
#include "../patricia_tree/thread_pool.hh"

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
  tree.loadFromDico(dico);
  //tree.display(std::cout);
  dico.close();

  //tree.display(std::cout);

  ThreadPool pool(1);

  // Read from the standard input
  std::string line;
  while(!std::cin.eof())
  {
    getline(std::cin, line);
    std::stringstream cmd(line, std::ios::in);
    std::string functionName;
    cmd >> functionName;
    if (functionName == "approx") // APPROX function
    {
      unsigned int maxDistance;
      cmd >> maxDistance;
      if (cmd.fail())
	continue;

      std::string word;
      cmd >> word;
      if (word.size() == 0)
	continue;

      //std::cout << "maxdistance: " << maxDistance << std::endl;
      //std::cout << "word: \"" << word << "\"" << std::endl;


      std::list<SearchResult> resultCollector;
      pool.configure(word.c_str(), maxDistance,
		     tree.getData(), resultCollector);
      tree.search(pool);
      resultCollector.sort(resultCompare);
      //tree.display(std::cout);
      exportJSon(resultCollector, std::cout);
      //std::cout << std::endl;
    }
  }
  pool.join();

  return 0;
}
