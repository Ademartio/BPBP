
#include <iostream>
#include <tuple>
#include <vector>
#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

// helper function to avoid making a copy when returning a py::array_t
// author: https://github.com/YannickJadoul
// source: https://github.com/pybind/pybind11/issues/1042#issuecomment-642215028
template <typename Sequence>
inline py::array_t<typename Sequence::value_type> as_pyarray(Sequence &&seq) {
   auto size = seq.size();
   auto data = seq.data();
   std::unique_ptr<Sequence> seq_ptr = std::make_unique<Sequence>(std::move(seq));
   auto capsule = py::capsule(seq_ptr.get(), 
                              [](void *p) 
                              {
                                 std::unique_ptr<Sequence>(reinterpret_cast<Sequence *>(p));
                              }
                              );
   seq_ptr.release();

   return py::array(size, data, capsule);
}

std::tuple<py::array_t<size_t>, py::array_t<unsigned char>> 
   koh_v2(py::array_t<unsigned char, py::array::c_style> const & hog)
{
   const size_t hog_rows = hog.shape(0);
   const size_t hog_cols = hog.shape(1);
   std::cout << "cols: " << hog_cols << ", rows: " << hog_rows << std::endl;

   std::vector<size_t> tmp(5, 0);
   std::vector<unsigned char> tmp2(100, 1);

   return std::make_tuple(as_pyarray(std::move(tmp)), as_pyarray(std::move(tmp2)));
}