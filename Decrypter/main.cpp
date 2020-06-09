#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const string &NOISE1 = "6%9M^bPx";
const string &NOISE2 = "UidB*#A4";
const string &NOISE3 = "3m@93eHf";
const string &NOISE4 = "Xg$FZr9Y";
const string &NOISE5 = "nQ6d95K&";
const string &NOISE6 = "u6$CF^p*";
const string &NOISE7 = "eX74J!AP";
const string &NOISE8 = "*hKh!8sV";
const string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string DecryptB64(string s);
string base64_decode(const string &s);

int main(int argc, char *argv[]){
    if(argc != 3) return cout << "Needs 2 args (input and output)" << endl, 2;
    string in (argv[1]);
    string out (argv[2]);

    ifstream fin (in);
    if(!fin) return cout << "Can't read input: '" << in << "'", 3;
    string data;
    fin >> data;

    if(!fin) return cout << "Input file corrupt: '" << in << "'", 4;

    data = DecryptB64(data);
    ofstream fout (out);
    if (!fout) return cout << "Can't create output file (permissions?): '" << out << "'" << endl, 5;
    fout << data;
    cout << "Decoding was successful!" << endl;
    return 0;
}

string base64_decode(const string &s) {
    string ret;
    vector<int> vec(256, -1);
    for (int i = 0; i < 64; i++) vec[BASE64_CODES[i]] = i;
    int val = 0, bits = -8;
    for (const auto &c : s) {
        if (vec[c] == -1) break;
        val = (val << 6) + vec[c];
        bits += 6;

        if (bits >= 0){
            ret.push_back(char((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return ret;
}

// Reference ./Keylogger/Base64.h -> encrypt
string DecryptB64 (string s) {
    s = s.erase (0, 1);
    s = base64_decode (s);
    s = s.erase(13, NOISE7.length());
    s = base64_decode (s);
    s = s.substr(NOISE1.length() + NOISE2.length() + NOISE3.length());
    s = s.substr(0, s.length() - NOISE4.length() - NOISE5.length() - NOISE6.length() - NOISE7.length() - NOISE8.length());
    return s;
}