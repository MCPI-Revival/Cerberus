#include "mod.h"
#include "SHA256.h"

// Hashing
std::string hash_password(const std::string &password) {
    SHA256 sha;
    sha.update(password);
    const std::array<uint8_t, 32> digest = sha.digest();
    return SHA256::toString(digest);
}