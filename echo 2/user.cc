#include "user.h"
#include <iostream>
using namespace std;

User::User(){
  
};

void User::putMessage(string subject, string message){
  cout << "storing subject " << subject << " message " << message << endl;
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
  if(index > messages.size() || index <= 0){
    return "error message doesn't exist\n";
  }
  string message = messages.at(index - 1).message;
  cout << "message " + messages.at(index - 1).subject + " " + to_string(message.length()) + 
  "\n" + message << endl;
  return "message " + messages.at(index - 1).subject + " " + to_string(message.length()) + 
  "\n" + message;
};