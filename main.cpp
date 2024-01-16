#include <iostream>
#include <vector>
#include <cstring>

class Memhook {
public:
  // Constructor
  Memhook(void* targetAddress, size_t size)
      : targetAddress_(targetAddress), size_(size) {}

  // Hook function
  void hook(const std::function<void(uint8_t*, size_t)>& callback) {
    originalBytes_ = std::vector<uint8_t>(size_);
    memcpy(originalBytes_.data(), targetAddress_, size_);
    trampoline_ = createTrampoline(targetAddress, callback);
    memcpy(targetAddress_, trampoline_.data(), trampoline_.size());
  }

  // Unhook function
  void unhook() {
    memcpy(targetAddress_, originalBytes_.data(), size_);
  }

  // New method for dynamic search and replace
  static bool searchAndReplace(void* targetAddress, size_t size,
                              const std::vector<uint8_t>& oldBytes,
                              const std::vector<uint8_t>& newBytes) {
    if (!targetAddress || size == 0 || oldBytes.empty() || newBytes.size() != oldBytes.size()) {
      std::cerr << "Invalid parameters for searchAndReplace" << std::endl;
      return false;
    }

    // Perform search and replace logic here
    for (size_t i = 0; i < size - oldBytes.size(); ++i) {
      bool found = true;
      for (size_t j = 0; j < oldBytes.size(); ++j) {
        if (readByte(targetAddress + i + j) != oldBytes[j]) {
          found = false;
          break;
        }
      }
      if (found) {
        for (size_t j = 0; j < newBytes.size(); ++j) {
          writeByte(targetAddress + i + j, newBytes[j]);
        }
        return true; // Replacement successful
      }
    }

    return false; // Pattern not found
  }

private:
  // Pointer to the target memory address
  void* targetAddress_;

  // Size of the target memory region
  size_t size_;

  // Original bytes of the target memory region
  std::vector<uint8_t> originalBytes_;

  // Trampoline code to call the original function and the callback
  std::vector<uint8_t> trampoline_;

  // Function to read a byte from memory
  uint8_t readByte(const void* address) const {
    return *reinterpret_cast<const uint8_t*>(address);
  }

  // Function to write a byte to memory
  void writeByte(void* address, uint8_t value) const {
    *reinterpret_cast<uint8_t*>(address) = value;
  }

  // Function to create the trampoline code
  std::vector<uint8_t> createTrampoline(void* targetAddress, const std::function<void(uint8_t*, size_t)>& callback) {
    // ... (implement trampoline creation)
  }
};

void searchAndReplaceDynamic(void* targetAddress, size_t size,
                             const char* oldBytes, const char* newBytes) {
  if (!targetAddress || size == 0 || !oldBytes || !newBytes) {
    std::cerr << "Invalid parameters for searchAndReplaceDynamic" << std::endl;
    return;
  }

  std::vector<uint8_t> oldBytesVector(oldBytes, oldBytes + strlen(oldBytes));
  std::vector<uint8_t> newBytesVector(newBytes, newBytes + strlen(newBytes));

  if (Memhook::searchAndReplace(targetAddress, size, oldBytesVector, newBytesVector)) {
    std::cout << "Replacement successful" << std::endl;
  } else {
    std::cout << "Pattern not found or error during replacement" << std::endl;
  }
}

int main() {
  // Assuming targetExecutableAddress is the address where your executable is loaded in memory
  void* targetExecutableAddress = reinterpret_cast<void*>(0x1000);

  // Example: Replace oldBytes with newBytes dynamically
  const char oldBytes[] = "\x33\x30\x38\x32\x30\x31\x42\x37";
  const char newBytes[] = "\x33\x30\x38\x32\x30\x31\x42\x37\x41\x42\x43\x44"; // <-- Here it is

  searchAndReplaceDynamic(targetExecutableAddress, sizeof(oldBytes) - 1, oldBytes, newBytes);

  return 0;
}
