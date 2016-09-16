#include "serverController.h"
#include <map>

using namespace std;

ServerController::ServerController(){}
ServerController::~ServerController(){
}
void ServerController::putMessage(string userName, string subject, string message){
  User* myUser = getUser(userName); 
  myUser->putMessage(subject,message);
}
string ServerController::getList(string userName){
  User* myUser = getUser(userName);
  vector<string> messages = myUser->getList();  
  string response = "list " + to_string(messages.size()) + "\n";
  for(auto m : messages){
    response += m + "\n";
  }
  return response;
}
string ServerController::getMessage(string userName, int index){
  return getUser(userName)->getMessage(index);
}
User* ServerController::getUser(string name){
  if(users.find(name) == users.end()){
    users[name] = User();
  }
  return & users[name];
}