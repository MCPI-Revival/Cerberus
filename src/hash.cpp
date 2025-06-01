#include "mod.h"
#include "bcrypt.h"

// Hashing
std::string hash_password(const std::string &password) {
    char salt[BCRYPT_HASHSIZE];
    char hash[BCRYPT_HASHSIZE];
    // Generate Salt
    int ret = bcrypt_gensalt(BCRYPT_DEFAULT_WORK_FACTOR, salt);
    if (ret != 0) {
        return "";
    }
    // Hash
    ret = bcrypt_hashpw(password.c_str(), salt, hash);
    if (ret != 0) {
        return "";
    }
    return hash;
}

// Verify Hash
bool hash_check(const std::string &password, const std::string &hash) {
    const int ret = bcrypt_checkpw(password.c_str(), hash.c_str());;
    return ret == 0;
}