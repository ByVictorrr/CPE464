
#include <iostream>
#include <string>
#include <regex>
#include <map>
using namespace std;

std::vector<std::string> splitByWhiteSpace(std::string &input){
    std::vector<std::string> result; 
    std::istringstream iss(input); 
    for(std::string s; iss >> s; ) 
        result.push_back(s); 
    return result;
}

int main(){
    /*
    std::string v1 = "%M 1 handlefffffffffffffffffffffffffff				                                   dafdfdfad                                                            	akdfjdafdadsf                                         faddadsfa 		adfadfadfaddadffdaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa adfafd	kdafdfadfadfadfsfddfas kkkkkkkkkkkkkkkk         ";
    const char * reg = "^\\s*%[M|m]\\s+[1-9]((\\s+[^\\s]{1,100}){8})\\s+(.*)";
     std::regex rgx(reg);
    std::smatch match;
    if (std::regex_search(v1, match, rgx)){
        int val = match[3].length();
        std::string v2 = match[3].str();
    }
    */
   /*
   std::map<int,std::string> v;
   v.insert({1,"hi"});
   for(const auto &a: v){
       std::cout << "hi" << a.first << " with size: " << v.size() << std::endl;
   }
   for(std::map<int,std::string>::iterator it = v.begin(), next=it; it!=v.end();){
       int first = it.operator*().first;
       it++;
       v.erase(first);
       std::cout << "hi" << first << " with size: " << v.size() << std::endl;
   }
   */
 
  std::string str = "               %M     12132       adfaadfffffffff     ";
  std::string str1 = "";
  std::vector<std::string> v= splitByWhiteSpace(str);
  std::vector<std::string> v1= splitByWhiteSpace(str1);
  for (size_t i = 0; i < v.size(); i++)
  {
      std::cout << v[i] << std::endl;
  }
   for (size_t i = 0; i < v1.size(); i++)
  {
      std::cout << v1[i] << std::endl;
  }
  
 std::cout << v1.size() << std::endl;

   

    return 0;
}