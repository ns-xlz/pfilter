//
// Created by xianglizhao on 2018/6/4.
//

#ifndef PHXPF_PHX_MSG_H
#define PHXPF_PHX_MSG_H

#include <iostream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace std;

class Base64Codec {
private:
public:
    typedef boost::archive::iterators::base64_from_binary<
            boost::archive::iterators::transform_width<string::const_iterator, 6, 8> >
            Base64EncodeIterator;
    typedef boost::archive::iterators::transform_width<
            boost::archive::iterators::binary_from_base64<string::const_iterator>, 8, 6>
            Base64DecodeIterator;

    static string Base64Encode(const string & src);
    static string Base64Decode(const string & src);
};


#endif //PHXPF_PHX_MSG_H
