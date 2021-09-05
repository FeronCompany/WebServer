# Cream Web Server
Cream Web Server  
A web server frame by C++11  
## <1> What each directory for?
directories|for  
:--:|--  
commlib|some basic tools  
frame|utilities for server frame
src|classes of business  
test|functions and classes of test
## <2> How to start develope?
Start by the following steps:  
+ `cd build && cmake ..` and get the stubs in build/stubs/
+ `cd src/proto` and replace src/proto/*.proto by your own
+ `mkdir <your service name>` and develope inside
+ When developing some common utils of business, just put them into proj_comm
## <3> How to build?
+ install third-party libs:
  + protobuf
  + mysqlclient
  + hiredis
  + redis++
+ start build within the following steps:  
`mkdir build` if not exists  
`cd build`  
`cmake ..`  
`make`
## <4> Level of internal dependency
level|directories  
:--:|--  
basic|commlib  
frame|frame  
business|src  