#ifndef __TEST_HEADER_H__
#define __TEST_HEADER_H__

#include <string>
#include <iostream>
#include <vector>
#include "commlib/basic/Json.h"

struct SubStruct
{
	SubStruct() : numA(0), numB(0), bVal(true) {}

	int numA;
	int numB;
	std::string strA;
	bool bVal;

	void toJson(CWSLib::JsonNode& node)
	{
		node.addElement("num_a", numA);
		node.addElement("num_b", numB);
		node.addElement("str_a", strA);
		node.addElement("b_val", bVal);
	}
};

struct TestJson
{
	TestJson() : numA(0), numB(0), numC(0) {}

	int numA;
	long long numB;
	double numC;
	SubStruct sub;
	std::vector<SubStruct> subList;
	std::vector<std::string> strList;

	void toJson(CWSLib::JsonNode& node)
	{
		node.addElement("num_a", numA);
		node.addElement("num_b", numB);
		node.addElement("num_c", numC);
		node.addElement("sub", sub);
		node.addElement("sub_list", subList);
		node.addElement("str_list", strList);
	}
};

#endif // !__TEST_HEADER_H__


