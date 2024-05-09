// Pybind11 header libraries
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "kruskal_on_hypergraph.h"
#include "uf_kruskal_on_hypergraph.h"

namespace py = pybind11;

// Bindings for the bpbp module
PYBIND11_MODULE(bpbp, bpbp) {
   bpbp.def("print_matrix", &printMatrix, "A function to print a matrix");
   bpbp.def("kruskal_on_hypergraph", &kruskal_on_hypergraph, "A very nice function");
   bpbp.def("uf_kruskal_on_hypergraph", &uf_kruskal_on_hypergraph, "Another very nice function");
}