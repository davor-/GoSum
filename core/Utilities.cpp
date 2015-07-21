

#include "Utilities.h"
#include <windows.h>


BOOST_CLASS_EXPORT_IMPLEMENT(MatrixXd)
BOOST_CLASS_EXPORT_IMPLEMENT(VectorXd)

BOOST_CLASS_EXPORT_IMPLEMENT(Matrix2d)
BOOST_CLASS_EXPORT_IMPLEMENT(Vector2d)

BOOST_CLASS_EXPORT_IMPLEMENT(ArrayXi)
BOOST_CLASS_EXPORT_IMPLEMENT(ArrayXd)
BOOST_CLASS_EXPORT_IMPLEMENT(ArrayXXd)






std::wstring string2WideString(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
