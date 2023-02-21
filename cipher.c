#include "main.h"

static char base64_table[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'};
static char base64_pad = '=';

unsigned char *base64_encode(const unsigned char *str, int length, int *ret_length)
{
    const unsigned char *current = str;
    int i = 0;
    unsigned char *result = (unsigned char *)malloc(((length + 3 - length % 3) * 4 / 3 + 1) * sizeof(char));
    while (length > 2)
    { /* keep going until we have less than 24 bits */
        result[i++] = base64_table[current[0] >> 2];
        result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
        result[i++] = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
        result[i++] = base64_table[current[2] & 0x3f];
        current += 3;
        length -= 3; /* we just handle 3 octets of data */
    }
    /* now deal with the tail end of things */
    if (length != 0)
    {
        result[i++] = base64_table[current[0] >> 2];
        if (length > 1)
        {
            result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
            result[i++] = base64_table[(current[1] & 0x0f) << 2];
            result[i++] = base64_pad;
        }
        else
        {
            result[i++] = base64_table[(current[0] & 0x03) << 4];
            result[i++] = base64_pad;
            result[i++] = base64_pad;
        }
    }
    if (ret_length)
    {
        *ret_length = i;
    }
    result[i] = '\0';
    return result;
}

unsigned char *base64_decode(const unsigned char *str, int length, int *ret_length)
{
    const unsigned char *current = str;
    int ch, i = 0, j = 0, k;
    /* this sucks for threaded environments */
    static short reverse_table[256];
    static int table_built;
    unsigned char *result;
    if (++table_built == 1)
    {
        char *chp;
        for (ch = 0; ch < 256; ch++)
        {
            chp = strchr(base64_table, ch);
            if (chp)
            {
                reverse_table[ch] = chp - base64_table;
            }
            else
            {
                reverse_table[ch] = -1;
            }
        }
    }
    result = (unsigned char *)malloc(length + 1);
    if (result == NULL)
    {
        return NULL;
    }
    /* run through the whole string, converting as we go */
    while ((ch = *current++) != '\0')
    {
        if (ch == base64_pad)
            break;
        /* When Base64 gets POSTed, all pluses are interpreted as spaces.
        This line changes them back.  It's not exactly the Base64 spec,
        but it is completely compatible with it (the spec says that
        spaces are invalid).  This will also save many people considerable
        headache.  - Turadg Aleahmad <turadg@wise.berkeley.edu>
        */
        if (ch == ' ')
            ch = '+';
        ch = reverse_table[ch];
        if (ch < 0)
            continue;
        switch (i % 4)
        {
        case 0:
            result[j] = ch << 2;
            break;
        case 1:
            result[j++] |= ch >> 4;
            result[j] = (ch & 0x0f) << 4;
            break;
        case 2:
            result[j++] |= ch >> 2;
            result[j] = (ch & 0x03) << 6;
            break;
        case 3:
            result[j++] |= ch;
            break;
        }
        i++;
    }
    k = j;
    /* mop things up if we ended on a boundary */
    if (ch == base64_pad)
    {
        switch (i % 4)
        {
        case 0:
        case 1:
            free(result);
            return NULL;
        case 2:
            k++;
        case 3:
            result[k++] = 0;
        }
    }
    if (ret_length)
    {
        *ret_length = j;
    }
    result[k] = '\0';
    return result;
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len = 0;
    int ciphertext_len;
    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;
    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;
    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

void *setEncodeStr(char *plaintext, char *encodeText)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    /* A 256 bit key */
    unsigned char *key = (unsigned char *)conf.cipherSK;
    /* A 128 bit IV */
    unsigned char *iv = (unsigned char *)conf.cipherIV;
    int encoding_num = 0;
    unsigned char ciphertext[MAXLINE];
    int ciphertext_len;
    char *restext;
    /* Initialise the library */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    memset(&ciphertext, 0x00, sizeof(ciphertext));
    ciphertext_len = encrypt(plaintext, strlen(plaintext), key, iv, ciphertext);
    ciphertext[ciphertext_len] = '\0';
    restext = base64_encode((unsigned char *)ciphertext, ciphertext_len, &encoding_num);
    strcpy(encodeText, restext);
}

void *setDecodeStr(char *encodeText, char *plaintext)
{
    CONFIG conf;
    memset(&conf, 0x00, sizeof(CONFIG));
    setConfig(&conf, CONFIG_PATH);

    /* A 256 bit key */
    unsigned char *key = (unsigned char *)conf.cipherSK;
    /* A 128 bit IV */
    unsigned char *iv = (unsigned char *)conf.cipherIV;
    int decoding_num = 1;
    unsigned char decodetext[MAXLINE];
    int decodetext_len;
    char *decodetext64;
    /* Initialise the library */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    decodetext64 = base64_decode((unsigned char *)encodeText, strlen(encodeText), &decoding_num);
    memset(&decodetext, 0x00, sizeof(decodetext));
    decodetext_len = decrypt(decodetext64, decoding_num, key, iv, decodetext);
    decodetext[decodetext_len] = '\0';
    strcpy(plaintext, decodetext);
}

/*
int main(int argc, char *argv[]) {

    // 암호화
    char plaintext[] = "{\"accessToken\":\"KyhQ7T9L0QhUx3iDe1IEJcXAbqG0eiWM8eN\", \"in_Oxygen\":\"1\",\"in_Carbon\":\"1\",\"in_Methane\":\"1\",\"in_AirCurrent\":\"1\", \"out_Oxygen\":\"1\",\"out_Carbon\":\"1\",\"out_Methane\":\"1\",\"out_AirCurrent\":\"1\", \"st_Temp\":\"1\",\"st_Humi\":\"1\",\"observedDate\":\"20221012111600\"}";
    unsigned char encodetext[MAXLINE];
    memset(&encodetext,0x00,sizeof(encodetext));
    getEncodeStr(plaintext, encodetext);
    printf("BASE64ENCODING=[%s] \n\n",encodetext);

    // 복호화
    char encodestr[] = "LOcEl9RIr8cRTpw0TnTw09W64ylvAmyoC4EUqHC+AfzPPz5c4as7DgTf7Nf+hRZpawexwyDAtmcVG8iM58ZdzkH9Iu/RaGUM/1iPokTGtTzmsiFvHbW43WFv7xSJy+IDUJyjFWlcequQD5n6Niv61qKAxLyC8zc8AWR7V+njL8+Rr8o4CXpLs+znygeIZKO6v7/atBS5E+odFgEHLic6dj60Ljg330DHp8m3/cdyDIqEL/53s7N0IFWL3uxZJwcBmh54dZFyIVSCQc2qwxhBcu8jY4qWeN00fXT180r3FZHTP7VngZegwMHsvx/mCYHS3cRmocqCudGC6g5j5BUosAn2KQFbL6+qoI3Jbb1Y0MM=";
    unsigned char decodestr[MAXLINE];
    memset(&decodestr,0x00,sizeof(decodestr));
    getDecodeStr(encodestr, decodestr);
    printf("BASE64DECODING=[%s] \n\n",decodestr);

    return 0;
}
*/
