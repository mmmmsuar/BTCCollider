#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Open CSV file for output
std::ofstream outfile("private_keys_p2pkh.csv");
outfile << "PrivateKey,Address" << std::endl; // CSV headers

// Write to CSV in the loop
outfile << currentKey.toString() << "," << p2pkhAddress << std::endl;

#include "Timer.h"
#include "BTCCollider.h"
#include "SECP256K1.h"
#include "hash/sha256.h"
#include "hash/ripemd160.h"
#include "Base58.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define RELEASE "1.1"

// Define the key range
const std::string START_KEY = "20000000000000000";
const std::string END_KEY = "3ffffffffffffffff";
const size_t EXPORT_SIZE = 500 * 1024 * 1024;  // 500MB size for exporting keys

// Function to generate private keys
void generatePrivateKeys() {
    Int currentKey(START_KEY);
    Int endKey(END_KEY);
    size_t batchSize = 0;
    std::ofstream outfile("private_keys_p2pkh.txt");
    
    // Loop through the key range
    while (currentKey <= endKey) {
        // Compute public key from private key
        Point publicKey = SECP256K1::privateKeyToPublicKey(currentKey);
        
        // Generate P2PKH address
        std::vector<uint8_t> sha256Hash = sha256(publicKey.serialize());
        std::vector<uint8_t> ripemdHash = ripemd160(sha256Hash);
        
        // Add version byte (0x00 for P2PKH) and checksum
        std::vector<uint8_t> addressData(1, 0x00); // P2PKH address version
        addressData.insert(addressData.end(), ripemdHash.begin(), ripemdHash.end());
        
        // Double SHA-256 for checksum
        std::vector<uint8_t> checksum = sha256(sha256(addressData));
        addressData.insert(addressData.end(), checksum.begin(), checksum.begin() + 4);
        
        // Encode into Base58
        std::string p2pkhAddress = base58Encode(addressData);
        
        // Write to file
        outfile << currentKey.toString() << " " << p2pkhAddress << std::endl;
        batchSize += currentKey.toString().length() + p2pkhAddress.length();
        
        // Check if batch size exceeds export size
        if (batchSize >= EXPORT_SIZE) {
            outfile.flush(); // Write current batch to disk
            batchSize = 0;
        
        // Open CSV file for output
        std::ofstream outfile("private_keys_p2pkh.csv");
        outfile << "PrivateKey,Address" << std::endl; // CSV headers

        // Write to CSV in the loop
        outfile << currentKey.toString() << "," << p2pkhAddress << std::endl;
        
        }

        // Increment to next private key
        currentKey = currentKey + Int("1");
    }
    
    outfile.close();
}

int main(int argc, char* argv[]) {
    Timer timer;
    timer.start();
    
    try {
        generatePrivateKeys();
        timer.stop();
        std::cout << "Finished generating keys in " << timer.elapsedSeconds() << " seconds." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
