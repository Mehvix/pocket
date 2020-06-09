#ifndef POCKET_BASE64_H
#define POCKET_BASE64_H

#include <vector>
#include <string>

using namespace std;

//  https://en.wikipedia.org/wiki/Base64
namespace Base64 {
    string base64_encode(const string &);

    // noise to make decryption very hard
    const string &NOISE1 = "6%9M^bPx";
    const string &NOISE2 = "UidB*#A4";
    const string &NOISE3 = "3m@93eHf";
    const string &NOISE4 = "Xg$FZr9Y";
    const string &NOISE5 = "nQ6d95K&";
    const string &NOISE6 = "u6$CF^p*";
    const string &NOISE7 = "eX74J!AP";
    const string &NOISE8 = "*hKh!8sV";

    // encode with noise (arbitrary amount of times, more == harder)
    string encrypt(string s){
        s = NOISE1 + NOISE2 + NOISE3 + s + NOISE4 + NOISE5 + NOISE6 + NOISE7 + NOISE8;
        s = base64_encode(s);
        s.insert(13, NOISE7);
        s = base64_encode(s);
        s.insert(0, "m");
        return s;
    }

    const string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    string base64_encode(const string &s){
         /* Example:
         Input:             Man
         Bits:              01001101 01100001 01101110
         ---
         Sextets:           010011 010110 000101 101110
         Base64 character:  T      W      F      u
         */
        string ret;
        int val = 0;  // map input to table
        int bits = -6;  // represent group size
        const unsigned int b63 = 0x3F;  // hexadecimal for 63

        for(const auto &c : s) {
            val = (val << 8) + c;  // shift left 8 places, same as 2^8 + c
            bits += 8;  // add 8 to numbers of bits (octets)

            while (bits >= 0) {
                ret.push_back(BASE64_CODES[(val >> bits) & b63]);  // does binary right shift + binary AND comparison
                bits -= 6;
            }
        }

        if(bits > -6){ // (at least) one character has been inserted
            ret.push_back(BASE64_CODES[((val << 8) >> (bits + 8)) & b63]);
        }
        while (ret.size() % 4) ret.push_back('=');

        return ret;
    }
}

#endif //POCKET_BASE64_H
