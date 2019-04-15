#include "../include/authentication.h"
#include "../include/crypto.h"

std::unordered_set<std::string> Authentication::auth_;
std::string Authentication::token_magic_string_ = "saikaii";

std::string Authentication::generator_token(const std::string &id_card){
  std::string token = Crypto::base64_encode(Crypto::sha1(id_card + token_magic_string_));
  auth_.insert(token);
  return token;
}

bool Authentication::auth_token(const std::string &token){
  if(0 == token.length())
    return false;
  if(auth_.end() != auth_.find(token))
    return true;
  return false;
}

void Authentication::delete_token(const std::string &token){
  auto it = auth_.find(token);
  if(auth_.end() != it)
    auth_.erase(it);
}
