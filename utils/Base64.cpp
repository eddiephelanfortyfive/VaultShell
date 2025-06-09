//
// Created by eddie phelan on 09/06/2025.
//
#include "Base64.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdexcept>

namespace Base64 {

    std::string base64_encode(const std::vector<uint8_t>& data) {
        BIO* bio, * b64;
        BUF_MEM* bufferPtr;

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);  // No newlines
        BIO_write(bio, data.data(), static_cast<int>(data.size()));
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);

        std::string result(bufferPtr->data, bufferPtr->length);
        BIO_free_all(bio);
        return result;
    }

    std::vector<uint8_t> base64_decode(const std::string& input) {
        BIO* bio, * b64;
        int maxLen = static_cast<int>(input.length());
        std::vector<uint8_t> buffer(maxLen);

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_mem_buf(input.data(), maxLen);
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);  // No newlines
        int decodedLen = BIO_read(bio, buffer.data(), maxLen);
        if (decodedLen <= 0) {
            BIO_free_all(bio);
            throw std::runtime_error("Base64 decode failed.");
        }

        buffer.resize(decodedLen);
        BIO_free_all(bio);
        return buffer;
    }

}
