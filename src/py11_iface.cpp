// Pybind11 header libraries
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

extern std::tuple<py::array_t<size_t>, py::array_t<unsigned char>> 
   koh_v2(py::array_t<unsigned char, py::array::c_style> const & hog);

// Bindings for the bpbp module
PYBIND11_MODULE(bpbp, bpbp) {
   bpbp.def("koh_v2", &koh_v2, "A very nice function");
}