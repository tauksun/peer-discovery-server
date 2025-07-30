#include "config.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>

using namespace std;

bool isUserNameTaken(
    string username,
    unordered_map<string, unordered_map<string, string>> &peerData) {
  auto peer = peerData.find(username);
  if (peer != peerData.end()) {
    return true;
  }
  return false;
}

bool createUser(
    string &username,
    unordered_map<string, unordered_map<string, string>> &peerData) {
  auto peer = peerData.find(username);
  if (peer != peerData.end()) {
    return false;
  }

  unordered_map<string, string> userData;
  userData["lastUpdated"] = time(nullptr);
  peerData[username] = userData;

  return true;
}

void generatePasskey(string &passkey) {
  for (int i = 0; i < Config::PASSKEY_LENGTH; i++) {
    passkey += Config::PASSKEY_PICKER[rand() % 30];
  }
}

int storePasskey(
    string &passkey, string &username,
    unordered_map<string, unordered_map<string, string>> &peerData) {

  // Find peer
  auto peer = peerData.find(username);
  if (peer == peerData.end()) {
    return -1;
  }

  // Find passkey
  peer->second["passkey"] = passkey;

  return 0;
}

int updateIP(string &username, string &IP, string &passkey,
             unordered_map<string, unordered_map<string, string>> &peerData) {
  // Find peer
  auto peer = peerData.find(username);
  if (peer == peerData.end()) {
    return -1;
  }

  auto passkeyData = peer->second.find("passkey");
  if (passkeyData == peer->second.end() || passkey != passkeyData->second) {
    return -1;
  }

  peer->second["ip"] = IP;
  peer->second["lastUpdated"] = time(nullptr);

  return 0;
}

void fetchIP(string &username, string &IP, string &passkey,
             unordered_map<string, unordered_map<string, string>> &peerData) {
  // Find peer
  auto peer = peerData.find(username);
  if (peer == peerData.end()) {
    IP = "";
    return;
  }

  auto passkeyData = peer->second.find("passkey");
  if (passkeyData == peer->second.end() || passkey != passkeyData->second) {
    IP = "";
    return;
  }

  auto ipData = peer->second.find("ip");

  if (ipData == peer->second.end()) {
    IP = "";
    return;
  }

  IP = ipData->second;
}
