
#include <iostream>
#include <string>
#include <regex>
#include <map>
using namespace std;
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
 


   

    return 0;
}