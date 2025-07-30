#ifndef PEERDATA
#define PEERDATA

#include <string>
#include <unordered_map>
using namespace std;

bool isUserNameTaken(
    string username,
    unordered_map<string, unordered_map<string, string>> &peerData);

void generatePasskey(string &passkey);

int storePasskey(
    string &passkey, string &username,
    unordered_map<string, unordered_map<string, string>> &peerData);

int updateIP(string &username, string &IP, string &passkey,
             unordered_map<string, unordered_map<string, string>> &peerData);

void fetchIP(string &username, string &IP, string &passkey,
             unordered_map<string, unordered_map<string, string>> &peerData);

bool createUser(string &username,
                unordered_map<string, unordered_map<string, string>> &peerData);

#endif
