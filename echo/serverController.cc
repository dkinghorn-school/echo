#include "serverController.h"
// #include <map>
// #include <sstream>

using namespace std;

ServerController::ServerController()
{
  this->m = new mutex();

}
ServerController::~ServerController()
{
  delete m;

}
void ServerController::reset()
{
  lock_guard<std::mutex> lock(*m);
  users.clear();
}
string ServerController::putMessage(string userName, string subject, string message)
{
  lock_guard<std::mutex> lock(*m);
  User *myUser = getUser(userName);
  myUser->putMessage(subject, message);
  return "OK\n";
}
string ServerController::getList(string userName)
{
  lock_guard<std::mutex> lock(*m);
  User *myUser = getUser(userName);
  vector<string> messages = myUser->getList();
  string response = "list " + to_string(messages.size()) + "\n";
  for (vector<string>::iterator it = messages.begin(); it != messages.end(); ++it)
  {
    response += *it + "\n";
  }
  return response;
}
string ServerController::getMessage(string userName, int index)
{
  lock_guard<std::mutex> lock(*m);
  return getUser(userName)->getMessage(index);
}
User *ServerController::getUser(string name)
{
  if (users.find(name) == users.end())
  {
    users[name] = User();
  }
  return &users[name];
}