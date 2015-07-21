#ifndef UTILITIES_H
#define UTILITIES_H



//#pragma warning (disable : 4100 996)


#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <utility>
#include <queue>

#include <boost/math/special_functions/erf.hpp>
#include <boost/math/distributions/chi_squared.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
//#include <boost/serialization/assume_abstract.hpp>
#include <boost/ptr_container/serialize_ptr_vector.hpp>

#include <boost/config.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/thread.hpp>

//#include <boost/signal.hpp>
/////////////////////////////////////////////////////////////
  // signalslib.hpp
  #ifndef SIGNALSLIB_HPP_INCLUDED
  #define SIGNALSLIB_HPP_INCLUDED

  #if defined(signals) && defined(QOBJECTDEFS_H) && \
    !defined(QT_MOC_CPP)
  #undef signals
  #define signals signals
  #endif

  #include <boost/signal.hpp>
  namespace boost
  {
    namespace signalslib = signals;
  }

  #if defined(signals) && defined(QOBJECTDEFS_H) && \
    !defined(QT_MOC_CPP)
  #undef signals
  // Restore the macro definition of "signals", as it was
  // defined by Qt's <qobjectdefs.h>.
  #define signals protected
  #endif

  #endif

  /////////////////////////////////////////////////////////////




#include <boost/bind.hpp>

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;




/// Boost serialization for some Eigen Arrays.

#define boostSerializeArrayXdiMacro(A) \
template<class Archive> \
void save(Archive &ar,const A &a,const unsigned int version) \
{   int i,n=int(a.size()); \
    ar << boost::serialization::make_nvp("size",n); \
    for ( i=0; i<n; i++ )  ar << boost::serialization::make_nvp("data",a(i)); \
} \
template<class Archive> \
void load(Archive &ar,A &a,const unsigned int version) \
{   int i,n; \
    ar >> boost::serialization::make_nvp("size",n); \
    a.resize(n); \
    for ( i=0; i<n; i++ )  ar >> boost::serialization::make_nvp("data",a(i)); \
}


#define boostSerializeArrayXXdiMacro(B) \
template<class Archive> \
void save(Archive &ar,const B &a,const unsigned int version) \
{   int i,j,n=int(a.rows()),m=int(a.cols()); \
    ar << boost::serialization::make_nvp("rows",n); \
    ar << boost::serialization::make_nvp("cols",m); \
    for ( i=0; i<n; i++ ) \
       for ( j=0; j<m; j++ ) \
            ar << boost::serialization::make_nvp("data",a(i,j)); \
} \
template<class Archive> \
void load(Archive &ar,B &a,const unsigned int version) \
{   int i,j,n,m; \
    ar >> boost::serialization::make_nvp("rows",n); \
    ar >> boost::serialization::make_nvp("cols",m); \
    a.resize(n,m); \
    for ( i=0; i<n; i++ ) \
        for ( j=0; j<m; j++ ) \
            ar >> boost::serialization::make_nvp("data",a(i,j)); \
}


namespace boost {
namespace serialization {


boostSerializeArrayXdiMacro(ArrayXi)
boostSerializeArrayXdiMacro(ArrayXd)
boostSerializeArrayXXdiMacro(ArrayXXd)

boostSerializeArrayXdiMacro(Vector2d)
boostSerializeArrayXXdiMacro(Matrix2d)

boostSerializeArrayXdiMacro(VectorXd)
boostSerializeArrayXXdiMacro(MatrixXd)

}
}

BOOST_SERIALIZATION_SPLIT_FREE(MatrixXd)
BOOST_CLASS_EXPORT_KEY(MatrixXd)
BOOST_SERIALIZATION_SPLIT_FREE(VectorXd)
BOOST_CLASS_EXPORT_KEY(VectorXd)

BOOST_SERIALIZATION_SPLIT_FREE(Matrix2d)
BOOST_CLASS_EXPORT_KEY(Matrix2d)
BOOST_SERIALIZATION_SPLIT_FREE(Vector2d)
BOOST_CLASS_EXPORT_KEY(Vector2d)


BOOST_SERIALIZATION_SPLIT_FREE(ArrayXi)
BOOST_CLASS_EXPORT_KEY(ArrayXi)
BOOST_SERIALIZATION_SPLIT_FREE(ArrayXd)
BOOST_CLASS_EXPORT_KEY(ArrayXd)
BOOST_SERIALIZATION_SPLIT_FREE(ArrayXXd)
BOOST_CLASS_EXPORT_KEY(ArrayXXd)






//! \brief
//! Some utility functions.
std::wstring string2WideString(const std::string& s); //!< Converts string to wide string.





#endif // UTILITIES_H
