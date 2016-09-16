#include <string>
#include <map>
#include <vector>
#include "user.h"
using namespace std;

class ServerController
{
public:
  void putMessage(string user, string subject, string message);
  ServerController();
  ~ServerController();
  string getList(string user);
  string getMessage(string user, int index);

private:
  map <string,User> users;
  User* getUser(string name);

};