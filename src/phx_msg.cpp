//
// Created by xianglizhao on 2018/6/4.
//

#include "phx_msg.h"
#include <sstream>

string Base64Codec::Base64Decode(const string &src) {
    stringstream result;
    copy(Base64DecodeIterator(src.begin()),
         Base64DecodeIterator(src.end()),
         ostream_iterator<char>(result));
    return result.str();
}

string Base64Codec::Base64Encode(const string &src) {
    stringstream result;
    copy(Base64EncodeIterator(src.begin()),
         Base64EncodeIterator(src.end()),
         ostream_iterator<char>(result));
    size_t equal_count = (3 - src.length() % 3) % 3;
    for (size_t i = 0; i < equal_count; i++) {
        result.put('=');
    }
    return result.str();
}