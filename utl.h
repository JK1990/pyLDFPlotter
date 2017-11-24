#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/handle.hpp>

using namespace std;
namespace np = boost::python::numpy;
namespace p = boost::python;

np::ndarray vector_to_ndarray(vector<double> in);
