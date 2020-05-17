#pragma once 

#pragma once

#include <string>

namespace x {

namespace tool {

void GenerateRSAKey(std::string& privKey, std::string& pubKey);
std::string RSAEncrypt(const std::string& pubKey, const std::string& message);
std::string RSADecrypt(const std::string& privKey, const std::string& ciphertext);
std::string PBKDFEncrypt(const std::string& password, const std::string& salt);
std::string SHA3_256Hash(const std::string& password);
}

}