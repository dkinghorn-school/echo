#include <string>
#include <map>
#include <vector>
#include "user.h"
using namespace std;

class ServerController
{
public:
  
  ServerController();
  ~ServerController();
  string putMessage(string user, string subject, string message);
  string getList(string user);
  string getMessage(string user, int index);
  void reset();
private:
  map <string,User> users;
  User* getUser(string name);
  
};