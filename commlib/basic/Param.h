

#ifndef __PARAM_H__
#define __PARAM_H__

#include <string>
#include <map>

namespace CWSLib
{
	class Param
	{
	public:
		// setters
		void setString(const std::string& key, const std::string& value);
		void setValue(const std::string& key, int32_t value);
		void setValue(const std::string& key, int64_t value);
		void setValue(const std::string& key, double value);

		// getters
		// 如果值不存在或者类型错误，返回该类型的默认值
		// If value does not exist or type is wrong, return default value of that type
		int32_t getInt(const std::string& key);
		int64_t getLong(const std::string& key);
		double getDouble(const std::string& key);
		std::string getString(const std::string& key);

		// 生成key value串，bigSep是键值对之间的分隔符 smallSep是键值对内部的分隔符
		std::string toQueryString(const std::string& bigSep, const std::string& smallSep);

		// 解析key value串，bigSep是键值对之间的分隔符 smallSep是键值对内部的分隔符
		void parseQueryString(const std::string& rawString, const std::string& bigSep,
			const std::string& smallSep);

		// 清除所有键值对并释放内存
		void clear();

	private:

	private:
		std::map<std::string, std::string> mKvMap;
	};
}

#endif // !__PARAM_H__

