#ifndef SKMNTSYS_INCLUDE_AUTHENTICATION_H_
#define SKMNTSYS_INCLUDE_AUTHENTICATION_H_

#include <unordered_set>
#include <string>
#include <unordered_map>

class Authentication{
public:
  static void init(const std::string &filename);
  static bool verificate(const std::string &username, const std::string &password);
  static std::string generator_token(const std::string &id_card);
  static bool auth_token(const std::string &);
  static void delete_token(const std::string &);

private:
  static std::unordered_set<std::string> auth_;
  static std::string token_magic_string_;
  static std::unordered_map<std::string, std::string> acounts_;
};

#endif
