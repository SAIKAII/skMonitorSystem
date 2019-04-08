#ifndef skMNTSYS_INCLUDE_AUTHENTICATION_H_
#define skMNTSYS_INCLUDE_AUTHENTICATION_H_

#include <unordered_set>
#include <string>

class Authentication{
public:
  static std::string generator_token(const std::string &id_card);
  static bool auth_token(const std::string &);
  static void delete_token(const std::string &);

private:
  static std::unordered_set<std::string> auth_;
  static std::string token_magic_string_;
};

#endif
