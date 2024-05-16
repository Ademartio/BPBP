// Pybind11 header libraries
#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

extern py::array_t<size_t> koh_v2_uf(py::array_t<ssize_t, py::array::f_style> const & hog, 
                                       py::array_t<float> const & llrs, 
                                       size_t const & orig_hog_rows,
                                       py::array_t<size_t, py::array::c_style> idxs);

extern py::array_t<size_t> koh_v2_classical_uf(py::array_t<ssize_t, py::array::f_style> const & hog, 
                                                py::array_t<float> const & llrs, 
                                                size_t const & orig_hog_rows,
                                                py::array_t<size_t, py::array::c_style> idxs);

// Bindings for the bpbp module
PYBIND11_MODULE(bpbp, bpbp) {
   bpbp.def("kruskal_on_hypergraph_v2_uf", &koh_v2_uf, "A very nice function");
   bpbp.def("kruskal_on_hypergraph_v2_classical_uf", &koh_v2_classical_uf, "Another very nice function");
}