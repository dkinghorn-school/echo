#include <string>
#include <vector>


using namespace std;
class User
{
public:
  User();
  void putMessage(string subject, string message);
  vector<string> getList();
  string getMessage( int index);
private:
  class UserMessage
  {
  public:
    UserMessage(string subject, string message): subject(subject),message(message){}
    string subject;
    string message;
  } ;
  
  vector<UserMessage> messages;

};