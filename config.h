#ifndef CONSTANTS
#define CONSTANTS

#include <string>

namespace Config {
constexpr int TCP_SERVER_PORT = 4345;
constexpr bool ENABLE_LOGS = true;
constexpr int PASSKEY_LENGTH = 10;
constexpr char PASSKEY_PICKER[30] = "abcdefghkmnpqrstwxyz123456789";
} // namespace Config

#endif
