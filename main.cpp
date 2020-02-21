#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "Graph.h"
#include "Util.h"

#include "rr_graph_uxsdcxx.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <numeric>
#include <random>
#include <algorithm>

using namespace std;
using namespace Gorder;

const int INPUTNUM = 1;

int main(int argc, char *argv[]) {
  ios::sync_with_stdio(false);
  int i;
  int W = 5;
  clock_t start, end;
  string basename;

  if (argc == 1) {
    cout << "please provide parameter" << endl;
    quit();
  }

  i = 1;
  while (i < argc) {
    if (strcmp("-w", argv[i]) == 0) {
      i++;
      W = atoi(argv[i]);
      if (W <= 0) {
        cout << "w should be larger than 0" << endl;
        quit();
      }
      i++;
    } else {
      basename = argv[i++];
    }
  }

  srand(time(0));

  Graph g;
  cout << "Processing " << basename << endl;
  g.setFilename(basename);

  start = clock();
  int fd = open((basename + ".rr_graph.real.bin").c_str(), O_RDONLY);
  if (fd < 0) {
    cout << "Fail to open " << basename << "rr_graph.real.bin" << endl;
    quit();
  }

  ::capnp::ReaderOptions opts = ::capnp::ReaderOptions();
  opts.traversalLimitInWords = 1024 * 1024 * 1024;
  ::capnp::StreamFdMessageReader messageIn(fd, opts);
  auto rrIn = messageIn.getRoot<ucap::RrGraph>();

  g.readRrGraph(rrIn);
  g.Transform();
  cout << basename << " readGraph is complete." << endl;
  end = clock();
  cout << "Time Cost: " << (double)(end - start) / CLOCKS_PER_SEC << endl;

  start = clock();
  vector<int> order; // <-- remapping
  if(0) {
    order.resize(rrIn.getRrNodes().getNodes().size());
    std::iota(order.begin(), order.end(), 0);
    if(1) {
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(order.begin(), order.end(), g);
    }
  } else if(1) {
    g.RCMOrder(order);
  } else {
    g.GorderGreedy(order, W);
  }
  end = clock();
  cout << "ReOrdered Time Cost: " << (double)(end - start) / CLOCKS_PER_SEC
       << endl;
  cout << "Begin Output the Reordered Graph" << endl;

  int fdout = open((basename + ".rr_graph.real.ordered.bin").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fdout < 0) {
    cout << "Fail to open " << basename << ".rr_graph.real.ordered.bin" << endl;
    quit();
  }

  ::capnp::MallocMessageBuilder messageOut;
  auto rrOut = messageOut.initRoot<ucap::RrGraph>();
  g.WriteReOrderedRrGraph(order, rrIn, rrOut);
  writeMessageToFd(fdout, messageOut);
  close(fdout);
  close(fd);
}
