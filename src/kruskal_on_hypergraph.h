#ifndef KOH_H_
#define KOH_H_

#include <vector>
#include <tuple>

void printMatrix(const std::vector<std::vector<bool>>& matrix);

std::tuple<std::vector<std::vector<bool>>, std::vector<size_t>>
   kruskal_on_hypergraph(std::vector<std::vector<bool>> const & hog);

#endif //KOH_H_