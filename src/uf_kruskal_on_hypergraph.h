#ifndef UF_KOH_H_
#define UF_KOH_H_

#include <vector>
#include <tuple>

std::tuple<std::vector<std::vector<bool>>, std::vector<size_t>>
   uf_kruskal_on_hypergraph(std::vector<std::vector<bool>> const & hog);

std::vector<size_t> sort_llrs(std::vector<double> const & llrs);

#endif // UF_KOH_H_