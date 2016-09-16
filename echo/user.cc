#include "user.h"

using namespace std;

User::User(){
  
};

void User::putMessage(string subject, string message){
  messages.push_back(UserMessage(subject, message));
};
vector<string> User::getList(){
  vector<string> mylist;
  int size = messages.size();
  for(int i = 0; i < size; i++){
    mylist.push_back(to_string(i + 1) + " " + messages.at(i).subject);
  }
  return mylist;
};
string User::getMessage(int index){
  return messages.at(index - 1).message;
};