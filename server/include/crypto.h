#ifndef SKMNTSYS_INCLUDE_CRYPTO_H_
#define SKMNTSYS_INCLUDE_CRYPTO_H_

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstddef>
#include <string.h>
#include <openssl/sha.h>

static const char kHex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

class Crypto{
public:
  static std::string sha1(const std::string &input, std::size_t iterations = 1) noexcept{
    std::string hash;

    hash.resize(160/8);
    SHA1(reinterpret_cast<const unsigned char *>(&input[0]), input.length(), reinterpret_cast<unsigned char *>(&hash[0]));

    for(std::size_t c = 1; c < iterations; ++c)
      SHA1(reinterpret_cast<const unsigned char *>(&hash[0]), hash.length(), reinterpret_cast<unsigned char *>(&hash[0]));

    return hash;
  }

  static std::string sha1_verity(const std::string &input) noexcept{
    std::string hash;

    hash.resize(160/8);
    SHA1(reinterpret_cast<const unsigned char*>(&input[0]), input.length(), reinterpret_cast<unsigned char *>(&hash[0]));

    char res[128];
    memset(res, 0, 128);
    int i, index = 0;
    for(i = 0; i < hash.length(); ++i){
      res[index++] = kHex[(hash[i]>>4)&0x0f];
      res[index++] = kHex[hash[i]&0x0f];
    }
    res[index] = '\0';
    return std::string(res);
  }

  static std::string base64_encode(const std::string &input){
    typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::\
    transform_width<std::string::const_iterator, 6, 8>> base64_encode_iterator;
    std::stringstream result;
    std::copy(base64_encode_iterator(input.begin()), base64_encode_iterator(input.end()),
              std::ostream_iterator<char>(result));
    size_t equal_count = (3 - input.length() % 3) % 3;
    for(size_t i = 0; i < equal_count; ++i)
      result.put('=');
    return result.str();
  }
};

#endif
