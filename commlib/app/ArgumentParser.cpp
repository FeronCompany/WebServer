#include "ArgumentParser.h"
#include "MacroAssemble.h"

#include <iostream>
#include <vector>
#include <cstring>
//#include <exception>

void CWSLib::ArgumentParser::addDefaultArg(const COption& option)
{
	m_defaultArgs.insert(std::make_pair(option.strLongArgName, option));
	m_shortIndex.insert(std::make_pair(option.cShortArgName, option.strLongArgName));
}

void CWSLib::ArgumentParser::addDefaultArgs(COption* optionList, int size)
{
	for (int i = 0; i < size; ++i)
	{
		addDefaultArg(optionList[i]);
	}
}

void CWSLib::ArgumentParser::parse(int argc, char** argv)
{
	std::vector<std::string> commandStack;
	for (int i = 1; i < argc; ++i)
	{
		std::string commandName;
		if (checkLongName(argv[i], commandName) || checkShortName(argv[i], commandName))
		{
			// 确认是key时，确认其是否要跟value，如果明确不跟，直接跳过，否则放入commandStack
			std::map<std::string, COption>::iterator itor = m_defaultArgs.find(commandName);
			if (itor != m_defaultArgs.end() && itor->second.enArgType == EN_ARG_TYPE_NO_ARG)
			{
				m_argumentMap[commandName] = "";
			}
			else if (commandStack.empty())
			{
				commandStack.push_back(commandName);
			}
			else
			{
				printErrorInfo();
				exit(-1);
				//throw std::exception("more than one command in stack");
			}
		}
		else
		{
			// 如果确认是value，匹配commandStack栈顶的command，若为空，则废弃此值；
			if (!commandStack.empty())
			{
				m_argumentMap[commandStack.back()] = argv[i];
				commandStack.pop_back();
			}
			else
			{
				DEBUG_LOG("[%s] is not match with any command.", argv[i]);
				printErrorInfo();
				exit(-1);
				//throw std::exception("value is not match with any command");
			}
		}
	}

	if (!commandStack.empty())
	{
		DEBUG_LOG("error command [%s]", commandStack.front().c_str());
		printErrorInfo();
		exit(-1);
		//throw std::exception("error command");
	}
}

std::map<std::string, std::string>& CWSLib::ArgumentParser::getArgValue()
{
	return m_argumentMap;
}

bool CWSLib::ArgumentParser::checkShortName(const std::string& shortName, std::string& outName)
{
	if (shortName.size() == 2 && shortName.at(0) == '-')
	{
		std::map<char, std::string>::iterator itor = m_shortIndex.find(shortName.at(1));
		if (itor == m_shortIndex.end())
		{
			return false;
		}
		else
		{
			outName = itor->second;
			return true;
		}
	}
	return false;
}

bool CWSLib::ArgumentParser::checkLongName(const std::string& fullName, std::string& outName)
{
	if (fullName.size() > 2 && fullName.at(0) == '-' && fullName.at(1) == '-')
	{
		outName = fullName.substr(2);
		return true;
	}
	return false;
}

void CWSLib::ArgumentParser::printErrorInfo()
{
	PURE_LOG("Please check your input parameters...");
	PURE_LOG("Add arguments like this: --assert1 value1 --assert2 value2");

	if (!m_defaultArgs.empty())
	{
		PURE_LOG("Here is some default arguments:");
		for (auto& item : m_defaultArgs)
		{
			COption& option = item.second;
			if (option.enArgType != EN_ARG_TYPE_NO_ARG)
			{
				PURE_LOG("\t--%s xxx [%s][%s]",
					option.strLongArgName.c_str(),
					option.strComments.c_str(),
					option.strHelpMsg.c_str());
			}
			else
			{
				PURE_LOG("\t--%s [%s][%s]",
					option.strLongArgName.c_str(),
					option.strComments.c_str(),
					option.strHelpMsg.c_str());
			}
		}
	}
}
