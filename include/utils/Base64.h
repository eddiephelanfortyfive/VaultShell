//
// Created by eddie phelan on 09/06/2025.
//

#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <vector>

namespace Base64 {
    std::string encode(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decode(const std::string& input);
}

#endif // BASE64_H

