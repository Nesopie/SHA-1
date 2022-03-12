#include<iostream>
#include<algorithm>
#include<vector>
#include<sstream>
#include<iomanip>
#include<string>
#include<boost/dynamic_bitset.hpp>

std::string stringToBinary(std::string message) {
    std::string toBinary = "";
    // std::cout << message.length() << "\n";
    for(int i = 0; i < message.length(); i++) {
        int characterASCII = (int)message[i];
        std::string characterInBinary = "";

        while(characterASCII) {
            characterInBinary += characterASCII & 1 ? '1' : '0';
            characterASCII /= 2;
        }

        // Making sure the size of each character in binary is 8 otherwise
        // pad it with zeroes
        while(characterInBinary.length() < 8) {
            characterInBinary += '0';
        }

        // cout << characterInBinary << " ";

        // Add the answer in reverse order
        for(int i = characterInBinary.length() - 1; i >= 0; i--) {
            toBinary += characterInBinary[i];
        }
    }
    return toBinary;
}

std::string integerToBinary(int decimal) {
    std::string binaryString = "";
    while(decimal) {
        binaryString += (decimal & 1) ? '1' : '0';
        decimal /= 2;
    }

    return binaryString;
}

int binaryToDecimal(boost::dynamic_bitset<> &bitset) {
    int decimal = 0;
    for(int i = bitset.size() - 1; i >= 0; i--) {
        decimal *= 2;
        decimal += bitset[i];
    }

    return decimal;
}

int leftRotate(unsigned int decimal, int rotateOffset) {
    return (decimal << rotateOffset) | (decimal >> (32 - rotateOffset));
}

std::string binaryToHex(boost::dynamic_bitset<> binary) {
    std::string hexString = "";
    for(int i = binary.size() - 1; i >= 0; i -= 4) {
        int hex = (binary[i    ] << 3) +
                  (binary[i - 1] << 2) +
                  (binary[i - 2] << 1) +
                  (binary[i - 3]     ) ;
        if(hex <= 9) 
            hexString += (char)(hex + '0');
        else 
            hexString += (char)('A' + hex - 10);
    }
    return hexString;
}

std::string decimalToHex(int decimal) {
    std::stringstream stream;
    stream << std::setfill ('0')
           << std::setw(sizeof(decimal) * 2)
           << std::hex
           << decimal;

    return stream.str();
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string message;

    std::cout << "Enter the message\n";
    getline(std::cin, message);

    // std::cout << message;

    unsigned long long int size = message.length();
    int bitsetSize = size * 8;

    // If it is a multiple of 512 then keep it the same way else add
    // the remaining space to make it a multiple of 512
    bitsetSize = bitsetSize % 512 == 0 ? bitsetSize : bitsetSize + 512 - bitsetSize % 512;
    std::string messageInBinaryString = stringToBinary(message);

    boost::dynamic_bitset<> messageInBinary(bitsetSize);

    // fill the bitset in reverse order, first letter from messageInBinaryString
    // goes to the last place in messageInBinary
    for(int i = 0; i < messageInBinaryString.length(); i++) {
        messageInBinary[bitsetSize - i - 1] = messageInBinaryString[i] == '1' ? 1 : 0;
    }

    // Append 1 after the message, the bitset is already padded with zeroes
    messageInBinary[bitsetSize - 1 - messageInBinaryString.length()] = 1;
    // std::cout << bitsetSize << "\n";
    std::string messageLengthInBinaryString = integerToBinary(size * 8);
    // std::cout << messageLengthInBinaryString << "\n";
    for(int i = messageLengthInBinaryString.length() - 1; i >= 0; i--) {
        messageInBinary[i] = messageLengthInBinaryString[i] == '1' ? 1 : 0;
    }

    std::cout << messageInBinary << "\n";

    int counter = 0;
    std::cout << messageInBinary;
    for(int i = messageInBinary.size() - 1; i >= 0; i--) {
        if(counter == 8) {
            counter = 0;
            // std::cout << " ";
        }
        std::cout << messageInBinary[i];
        counter++;
    }

    std::cout << "\n";

    // Pair of nothing-up-my-sleeve numbers in hex and binary representation
    std::vector<std::pair<unsigned int, boost::dynamic_bitset<> > > H(5);

    H[0] = make_pair(0x67452301,boost::dynamic_bitset<> (160,0x67452301));
    H[1] = make_pair(0xEFCDAB89,boost::dynamic_bitset<> (160,0xEFCDAB89));
    H[2] = make_pair(0x98BADCFE,boost::dynamic_bitset<> (160,0x98BADCFE));
    H[3] = make_pair(0x10325476,boost::dynamic_bitset<> (160,0x10325476));
    H[4] = make_pair(0xC3D2E1F0,boost::dynamic_bitset<> (160,0xC3D2E1F0));

    std::cout << messageInBinary << "\n";

    for(int i = 0; i < bitsetSize; i += 512) {
        // A, B, C, D, E, F, K, H, HH, W have their usual meaning
        std::vector<boost::dynamic_bitset<> > binaryW(80);
        std::vector<unsigned int> W(80);
        int messageIterator = messageInBinary.size() - 1;
        std::cout << "\n";

        for(int j = 0; j < W.size(); j++) {
            binaryW[j].resize(32);
            if(j < 16) {
                for(int k = binaryW[j].size() - 1; k >= 0; k--) {
                    // as messageInBinary is a bitset, place the elements
                    // in reverse order
                    binaryW[j][k] = messageInBinary[messageIterator--];
                }
                W[j] = binaryToDecimal(binaryW[j]);
            }else {
                W[j] = W[j - 3] ^ W[j - 8] ^ W[j - 14] ^ W[j - 16];
                std::cout << j << " " << W[j] << " ";
                // W[j] = (W[j] << 1) | (W[j] >> (32 - 1));
                W[j] = leftRotate(W[j],1);
                std::cout << W[j] << "\n";
            }
        }

        unsigned int A = H[0].first,
                     B = H[1].first,
                     C = H[2].first,
                     D = H[3].first,
                     E = H[4].first;

        for(int j = 0; j < W.size(); j++) {
            unsigned int F,K;
            if(j >= 0 && j <= 19) {
                F = (B & C) | ((~B) & D);
                K = 0x5A827999;
            }else if(j >= 20 && j <= 39) {
                F = B ^ C ^ D;
                K = 0x6ED9EBA1;
            }else if(j >= 40 && j <= 59) {
                F = (B & C) | (B & D) | (C & D);
                K = 0x8F1BBCDC;
            }else if(j >= 60 && j <= 79) {
                F = B ^ C ^ D;
                K = 0xCA62C1D6;
            }

            unsigned int temp = leftRotate(A,5) + F + E + W[j] + K;
            E = D;
            D = C;
            C = leftRotate(B,30);
            B = A;
            A = temp;

            std::cout << j << " "
                      << A << " "
                      << B << " "
                      << C << " "
                      << D << " "
                      << E << " "
                      << K << "\n";
        }

        // Setting the decimal value in the pair
        H[0].first += A;
        H[1].first += B;
        H[2].first += C;
        H[3].first += D;
        H[4].first += E;

        // Setting the binary value from the already set decimal value
        H[0].second = boost::dynamic_bitset<> (160,H[0].first);
        H[1].second = boost::dynamic_bitset<> (160,H[1].first);
        H[2].second = boost::dynamic_bitset<> (160,H[2].first);
        H[3].second = boost::dynamic_bitset<> (160,H[3].first);
        H[4].second = boost::dynamic_bitset<> (160,H[4].first);

        std::cout << decimalToHex(H[0].first) << " " <<
                     decimalToHex(H[1].first) << " " <<
                     decimalToHex(H[2].first) << " " <<
                     decimalToHex(H[3].first) << " " <<
                     decimalToHex(H[4].first) << " " << "\n";
    }

    // BinaryHH is binary output of HH
    boost::dynamic_bitset<> BinaryHH(160);
    BinaryHH = (H[0].second << 128) |
               (H[1].second << 96 ) |
               (H[2].second << 64 ) |
               (H[3].second << 32 ) |
               (H[4].second       ) ;
    // std::cout << (H[0].second) << "\n" <<
    //              (H[1].second) << "\n" <<
    //              (H[2].second) << "\n" <<
    //              (H[3].second) << "\n" <<
    //              (H[4].second) << "\n" << "\n";

    // std::cout << (H[0].second << 128) << "\n" <<
    //              (H[1].second << 96 ) << "\n" <<
    //              (H[2].second << 64 ) << "\n" <<
    //              (H[3].second << 32 ) << "\n" <<
    //              (H[4].second << 00 ) << "\n" << "\n";

    // std::cout << BinaryHH << "\n\n";
    // ans stores the hash result from the binaryHH value
    std::string ans = binaryToHex(BinaryHH);

    std::cout << ans << "\n";

    // HH stores the hash result from the decimal numbers
    std::string HH = decimalToHex(H[0].first) +
                     decimalToHex(H[1].first) +
                     decimalToHex(H[2].first) +
                     decimalToHex(H[3].first) +
                     decimalToHex(H[4].first) ;

    for(char bit : HH) {
        if(bit >= 'a' && bit <= 'z') {
            std::cout << (char)(bit - 'a' + 'A');
        }else {
            std::cout << bit;
        }
    }

    return EXIT_SUCCESS;
}
