

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <string>
#include <stdarg.h>
#include <vector>

namespace CWSLib
{
namespace String
{
	// no more than 4096 bytes
	std::string format(const char* fmt, ...);

	void split(std::vector<std::string>& strList, const std::string& src, char spliter);

	std::string baseName(const std::string& fileName);

	std::string genRandomCode(int length);

    template <typename T>
    inline void append(T t, std::string &dst) { dst += std::to_string(t);};
    inline void append(const std::string& t, std::string &dst){ dst += t;};
    inline void append(const char* t, std::string &dst) { dst += t;};
    template <typename ...T>
    inline std::string append(const T& ...t)
    {
        std::string dst;
        int argv[] = {(append(t, dst), 0)...};
        return dst;
    }
}
}

#endif // !__STRING_UTILS_H__

