#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/handle.hpp>

using namespace std;
namespace np = boost::python::numpy;
namespace p = boost::python;

np::ndarray vector_to_ndarray(vector<double> in){
    p::tuple shape = p::make_tuple(in.size());
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::zeros(shape,dtype);
    copy(in.begin(),in.end(),reinterpret_cast<double*>(result.get_data()));
    return result;
}
