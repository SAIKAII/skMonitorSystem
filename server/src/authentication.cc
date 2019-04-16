#include "../include/authentication.h"
#include "../include/crypto.h"
#include <fstream>
#include <utility>

std::unordered_map<std::string, std::string> Authentication::acounts_;
std::unordered_set<std::string> Authentication::auth_;
std::string Authentication::token_magic_string_ = "saikaii";

void Authentication::init(const std::string &filename){
  std::ifstream ifs(filename);
  if(!ifs)
    exit(1);

  std::string username, password;
  while(ifs >> username, !ifs.eof()){
    ifs >> password;
    acounts_.insert(std::make_pair(username, password));
  }

  ifs.close();
}

bool Authentication::verificate(const std::string &username, const std::string &password){
  std::string u = Crypto::sha1_verity(username);
  auto it = acounts_.find(u);
  if(acounts_.end() == it)
    return false;

  if(it->second == Crypto::sha1_verity(password))
    return true;

  return false;
}

std::string Authentication::generator_token(const std::string &id_card){
  std::string token = Crypto::sha1_verity(id_card + token_magic_string_);
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
