// AES_CTR.cpp : Encode an arbitrary long string of bits using AES128 in counter mode
//               Here the nonse is 64 bits and the key is 128 bits
//               Note: The input is a set of ones and zeros describing a binary number.

//  The Authors-2022


#include "../include/aes_ctr.h"

void example()
{
    std::string plain_text = "1111111100000000111111110000000011111111000000001111111100000000111111110000000011111111000000001111111100000000111111110000000011111111000000001111111100000000";
    std::string cipher_text, decrypted_text;
    int len = 160;//int len = plain_text.size();
    int size = len / NBITS;


    //string2byte(pt_data, plain_text);
    printf("\nPlain Text:  \n");  printf(plain_text.c_str()); //phex(pt_data, size);

    // Data Encryption
    uint8_t key[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
    uint64_t nonse = 0;
    cipher_text = aes_ctr(plain_text, key, nonse);
    //string2byte(ct_data, cipher_text);
    printf("\nCipher Text: \n");  printf(cipher_text.c_str()); // phex(ct_data, size);
    //-----------------------------------
    //      .
    //      .
    //      .
    //      .
    //-----------------------------------
    // Data decryption
    // If the key or the nonse don't match with the used with encryption, every bit in the ouput will look random with a probability of 1/2
    uint8_t key2[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
    uint64_t nonse2 = 0;
    decrypted_text = aes_ctr(cipher_text, key2, nonse2);
    printf("\nDecrypted Text:\n");  printf(decrypted_text.c_str());

}

std::string  aes_ctr2(std::string str_pt, uint8_t* key, uint64_t ictr) {
    uint8_t rkey[176] = { 0 };
    AES aes(key);
    aes.KeyExpansion((gf28*)rkey, key);
    std::string str_ct;

    size_t len = str_pt.size();
    size_t size = len / NBITS;
    size_t packs = sizeof(str_pt) / AESBITS;

    uint8_t* pt = new uint8_t[size];
    string2byte(pt, str_pt);
    //printf("\nInput text: \t"); phex(pt, 16);
    size_t maxN = (size_t) ceil((double)size / BUFFER_SIZE);
    for (int i = 0; i < maxN; ++i) {
        uint8_t ctr[BUFFER_SIZE] = { 0 };
        uint8_t pt2[BUFFER_SIZE] = { 0 };
        uint8_t ct2[BUFFER_SIZE] = { 0 };

        IntToByteArray(ctr, ictr + i + 1);
        aes.ECB_encrypt((uint8_t*)ctr);
        extractElements(pt, pt2, i * BUFFER_SIZE);
        //XNOR(ct2, ctr, pt2, AESBITS / NBITS);
        std::string temp = byte2string(ct2, BUFFER_SIZE);
        str_ct.append(temp);
    }

    return  str_ct.substr(0, len);
}

std::string  aes_ctr(std::string str_pt, uint8_t* key, uint64_t ictr) {
   // uint8_t rkey[176] = { 0 };
    AES aes(key);
    //aes.KeyExpansion((gf28*)rkey, key);
    std::string str_ct;

    size_t len = str_pt.size();
    size_t size = len / NBITS;
    //int packs = sizeof(str_pt) / AESBITS;

    uint8_t* pt = new uint8_t[size];
    string2byte(pt, str_pt);
    //printf("\nInput text: \t"); phex(pt, 16);
    size_t maxN = (size_t) ceil((double)size / BUFFER_SIZE);
    for (int i = 0; i < maxN; ++i) {
        uint8_t ctr[BUFFER_SIZE] = { 0 };
        uint8_t pt2[BUFFER_SIZE] = { 0 };
        uint8_t ct2[BUFFER_SIZE] = { 0 };

        IntToByteArray(ctr, (long long)ictr + (long long)i + 1);
        aes.ECB_encrypt((uint8_t*)ctr);
        extractElements(pt, pt2, i * BUFFER_SIZE);
        xnor(ct2, ctr, pt2, AESBITS / NBITS); // Might have to change depending on operation of LCD screens
        std::string temp = byte2string(ct2, BUFFER_SIZE);
        str_ct.append(temp);
    }

    return  str_ct.substr(0, len);
}

std::vector<uint8_t> aes_ctr(std::vector<uint8_t> pt, uint8_t* key, uint64_t ictr) {
    AES aes(key);
    std::vector<uint8_t> ct;
    size_t maxN = pt.size();
    for (int i = 0; i < maxN; i+= BUFFER_SIZE) {
        uint8_t ctr[BUFFER_SIZE] = { 0 };
        uint8_t pt2[BUFFER_SIZE] = { 0 };
        uint8_t ct2[BUFFER_SIZE] = { 0 };

        IntToByteArray(ctr, (long long)ictr + (long long)i + 1);
        aes.ECB_encrypt((uint8_t*)ctr);

        extractElements(pt, pt2, i/ BUFFER_SIZE);
        xnor(ct2, ctr, pt2, AESBITS / NBITS); // Might have to change depending on operation of LCD screens
        
        std::vector<uint8_t> dest(std::begin(ct2), std::end(ct2));
        ct.insert(ct.end(), dest.begin(), dest.end());
    }

    return ct;
}

void extractElements(uint8_t* srcArray, uint8_t subArray[BUFFER_SIZE], int pos) {
    //subArray = new uint8_t[BUFFER_SIZE];
    int maxN = BUFFER_SIZE;
    for (int i = 0; i < maxN; i++)
        subArray[i] = srcArray[pos + i];
}

void extractElements(std::vector<uint8_t> srcArray, uint8_t subArray[BUFFER_SIZE], int pos) {
    std::vector<uint8_t>::const_iterator first = srcArray.begin() + pos * BUFFER_SIZE;
    int end = std::min((pos + 1) * BUFFER_SIZE - 1,(int) srcArray.size());
    std::vector<uint8_t>::const_iterator last = srcArray.begin() + end;
    std::vector<uint8_t> v(first, last);
    std::copy(v.begin(), v.end(), subArray);
}

void string2byte(uint8_t output[], std::string input) {
    size_t len = input.size();
    size_t size = (len - 1) / NBITS + 1;

    int i0 = 0;
    for (int j = 0; j < size; ++j) {
        uint8_t byte = 0;
        i0 = j * 8;
        for (int i = 0; i < 8; ++i)
            if (input[i + i0] == '1') byte |= 1 << (7 - i);
        output[j] = byte;
    }
}

std::string byte2string(uint8_t* input, int len) {
    std::string output;
    std::bitset<8> x;

    uint8_t i0 = 0;
    for (int j = 0; j < len; ++j) {
        x = input[j];
        output.append(x.to_string());
    }
    return output;
}

void XOR(uint8_t outArr[], uint8_t* byteArr1, uint8_t* byteArr2, size_t len) {
    //AESstate outArr(16);
    //int len = sizeof(byteArr1); // / sizeof(byteArr1[0]);
    for (unsigned i = 0; i < len; i++)
        outArr[i] = (byteArr1[i] ^ byteArr2[i]);
    return;
}

void xnor(uint8_t outArr[], uint8_t* byteArr1, uint8_t* byteArr2, size_t len) {
    //AESstate outArr(16);
    //int len = sizeof(byteArr1); // / sizeof(byteArr1[0]);
    for (unsigned i = 0; i < len; i++)
        outArr[i] = ~(byteArr1[i] ^ byteArr2[i]);
    return;
}

void phex(uint8_t str[], uint8_t len) {
    std::cout << "\t";
    if (len == 1)
        printf("%.2x", str[0]);
    else
        for (unsigned i = 0; i < len; ++i) {
            printf("%.2x", str[i]);
            if (i % 16 == 15 && i != len - 1)
                std::cout << "\n\t\t\t";
        }
}

void IntToByteArray(uint8_t* buffer, long long value) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i < 8)
            buffer[i] = ((uint8_t)(value >> (8 * i)) & 0XFF);
        else
            buffer[i] = 0x00;
    }
}