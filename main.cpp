#include "Timer.h"
#include "BTCCollider.h"
#include "SECP256k1.h"
#include "hash/sha256.h"
#include "hash/ripemd160.h"
#include "Base58.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define RELEASE "1.1"

// Default key range
const std::string DEFAULT_START_KEY = "20000000000000000";
const std::string DEFAULT_END_KEY = "3ffffffffffffffff";
const size_t EXPORT_SIZE = 500 * 1024 * 1024;  // 500MB size for exporting keys

// Function to generate private keys with custom range
void generatePrivateKeys(const std::string& startKey, const std::string& endKey) {
    Int currentKey(std::stoll(startKey));  // Convert string to int64_t
    Int endKeyInt(std::stoll(endKey));     // Convert string to int64_t
    size_t batchSize = 0;

    // Open CSV file for output
    std::ofstream outfile("private_keys_p2pkh.csv");
    outfile << "PrivateKey,Address" << std::endl; // CSV headers
    
    // Loop through the key range
    while (currentKey <= endKeyInt) {
        // Compute public key from private key
        Point publicKey = SECP256k1::privateKeyToPublicKey(currentKey);
        
        // Generate P2PKH address
        std::vector<uint8_t> sha256Hash = sha256(publicKey.serialize().data(), publicKey.serialize().size());
        std::vector<uint8_t> ripemdHash = ripemd160(sha256Hash.data(), sha256Hash.size());
        
        // Add version byte (0x00 for P2PKH) and checksum
        std::vector<uint8_t> addressData(1, 0x00); // P2PKH address version
        addressData.insert(addressData.end(), ripemdHash.begin(), ripemdHash.end());
        
        // Double SHA-256 for checksum
        std::vector<uint8_t> checksum = sha256(sha256(addressData.data(), addressData.size()));
        addressData.insert(addressData.end(), checksum.begin(), checksum.begin() + 4);
        
        // Encode into Base58
        std::string p2pkhAddress = base58Encode(addressData);
        
        // Write to CSV
        outfile << currentKey << "," << p2pkhAddress << std::endl;
        batchSize += std::to_string(currentKey).length() + p2pkhAddress.length();
        
        // Check if batch size exceeds export size
        if (batchSize >= EXPORT_SIZE) {
            outfile.flush(); // Write current batch to disk
            batchSize = 0;
        }

        // Increment to next private key
        currentKey = currentKey + 1;
    }
    
    outfile.close();
}

int main(int argc, char* argv[]) {
    Timer timer;
    timer.start();
    
    // Define default start and end keys
    std::string startKey = DEFAULT_START_KEY;
    std::string endKey = DEFAULT_END_KEY;
    
    // Check if the user provided custom range arguments
    if (argc == 3) {
        startKey = argv[1];
        endKey = argv[2];
    } else if (argc != 1) {
        std::cerr << "Usage: ./BTCCollider [startKey endKey]" << std::endl;
        return 1;
    }

    try {
        generatePrivateKeys(startKey, endKey);
        timer.stop();
        std::cout << "Finished generating keys from " << startKey << " to " << endKey
                  << " in " << timer.elapsedSeconds() << " seconds." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

