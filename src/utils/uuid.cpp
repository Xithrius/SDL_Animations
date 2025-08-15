#include <random>
#include <sstream>
#include <string>

std::string generateUUID() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);
  static const char* hex_chars = "0123456789abcdef";

  std::stringstream ss;
  ss << std::hex;

  // Generate 32 hex characters
  for (int i = 0; i < 32; ++i) {
    if (i == 8 || i == 12 || i == 16 || i == 20) {
      ss << "-";
    }
    ss << hex_chars[dis(gen)];
  }

  return ss.str();
}
