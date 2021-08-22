

#ifndef __ARGUMENT_PARSER_H__
#define __ARGUMENT_PARSER_H__

#include <string>
#include <map>

namespace CWSLib
{
	typedef enum tagEnArgType
	{
		EN_ARG_TYPE_NO_ARG = 0, // 表明该参数后不需带参数(如：--name)
		EN_ARG_TYPE_REQUIRE_ARG = 1, // 表明该参数后必须带参数(如：--name Bob)
		EN_ARG_TYPE_OPTIONAL_ARG = 2 // 表明该参数后可待参数也可不带参数 (即--name和--name Bob均可)
	}EN_ARG_TYPE;

	struct COption
	{
		std::string strLongArgName; // 长参数名称 --name $name$,则strLongArgName为"name"
		char cShortArgName; // 短参数名称 -n $name$,则cShortArgName为'n',仅允许字符或者NULL
		EN_ARG_TYPE enArgType;
		std::string strComments; // 注释
		std::string strHelpMsg; // 帮助信息
	};

	class ArgumentParser
	{
	public:
		void addDefaultArg(const COption& option);
		void addDefaultArgs(COption* optionList, int size);
		void parse(int argc, char** argv);	// May throw std::exception if argument not in format
		std::map<std::string, std::string>& getArgValue();

	private:
		bool checkShortName(const std::string& shortName, std::string& outName);
		bool checkLongName(const std::string& fullName, std::string& outName);
		void printErrorInfo();

	private:
		std::map<std::string, COption> m_defaultArgs;
		std::map<std::string, std::string> m_argumentMap;
		std::map<char, std::string> m_shortIndex;
	};
}

#endif // !__ARGUMENT_PARSER_H__

