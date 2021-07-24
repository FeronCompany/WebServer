

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "test.h"
#include "TestHeader.h"
#include "TestTask.h"
#include "commlib/thread_pool/ThreadPool.h"
#include "commlib/app/MacroAssemble.h"
#include "commlib/app/TypeRegSingleton.h"

void CWSTest::helloWorld()
{
	std::cout << "Hello world!\n";
}

//REG_TYPE(CWSLib::BaseJob, TestTask, test_task, false);
TypeRegister<CWSLib::BaseJob, TestTask, false> registerTestTask;

void CWSTest::threadPool()
{
	srand((unsigned int)time(NULL));

	CWSLib::ThreadPool pool;
	pool.init(5, 20, 20);

	for (int i = 0; i < 100000; ++i)
	{
		std::cout << i << "\n";
		pool.addTask(new TestTask);
		std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
	}

	std::this_thread::sleep_for(std::chrono::seconds(200000));
}

void CWSTest::log()
{
	int num = 10086;
	const char* lala = "aa10086";
	char c = 'c';
	SET_LOG_FILE("test_log.log");
	BASIC_LOG(CWSLib::LogLevel::DEBUG, "hahaha[][][]");
	BASIC_LOG(CWSLib::LogLevel::DEBUG, "hahaha[%d][%s][%c]", num, lala, c);
	DEBUG_LOG("hahaha[%d][%s][%c]", num, lala, c);
	NORMAL_LOG("hahaha[%d][%s][%c]", num, lala, c);
	WARN_LOG("hahaha[%d][%s][%c]", num, lala, c);
	ERROR_LOG("hahaha[%d][%s][%c]", num, lala, c);
}

void CWSTest::json()
{
	CWSLib::Json json;
	TestJson testObject;
	testObject.numA = 100;
	testObject.numB = 150;
	testObject.numC = 200;
	testObject.sub.numA = 996;
	testObject.sub.numB = 251;
	testObject.strList.push_back("aaa");
	testObject.strList.push_back("bbb");
	testObject.strList.push_back("ccc");

	SubStruct subA, subB;
	subA.numA = 1;
	subA.numB = 2;
	subA.strA = "haha";
	subA.bVal = true;
	subB.numA = 3;
	subB.numB = 4;
	subB.strA = "404 not found";
	subB.bVal = false;
	testObject.subList.push_back(subA);
	testObject.subList.push_back(subB);

	json << testObject;

	/*std::cout << json.toString() << "\n\n\n";
	std::cout << json.toFmtString() << "\n\n\n";

	CWSLib::Json jsonArray;
	jsonArray << testObject.subList;
	std::cout << jsonArray.toFmtString() << "\n";*/

	/*//std::string input = "{\"name\":\"Alpha\",\"age\":26,\"friends\":[\"Bill\",\"Anny\"]}";
	std::string input = "{\"name\":\"Alpha\",\"age\":26,\"score\":{\"math\":99,\"pe\":88}}";
	//std::string input = "[\"Bill\",\"Anny\"]";
	//std::string input = "{\"name\":\"Alpha\",\"age\":26,\"friends\":\"Bill\"}";
	CWSLib::Json json;
	if (json.parse(input) == 0)
		std::cout << json.toFmtString() << "\n";
	else
		std::cout << "parse error\n";*/

	/*std::vector<int> intList{1, 2, 3, 5, 7, 9};
	CWSLib::Json jsonArray;
	jsonArray << intList;
	std::cout << jsonArray.toFmtString() << "\n\n";*/
	CWSLib::Json psJson;
	psJson.parse(json.toString());
	std::cout << psJson.toFmtString() << "\n";
}

void CWSTest::client()
{
	struct sockaddr_in servaddr;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(12000);
	connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr));
	while (true)
	{
		std::string str;
		char buf[1000] = { 0 };
		std::cout << "Say: \n";
		std::cin >> str;
		if (str == "quit" || str == "exit")
		{
			break;
		}
		write(sockfd, str.c_str(), str.length());
		int n = read(sockfd, buf, sizeof(buf));
		std::cout << "Server: " << buf << "\n";
	}
	
	close(sockfd);
}


