

#include <iostream>

#include "CwsServer.h"
#include "ProJobImpl.h"
#include "ProServerInit.h"

int main(int argc, char** argv)
{
	if (0 != ServerInit()) return -1;
	CwsFrame::Server& server = *CServerSingleton::instance();
	server.Run();
	return 0;
}

