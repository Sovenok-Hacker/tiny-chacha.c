#include <stdint.h>

// Left n-bit roll of x
#define LROLL32(x, n) (x << n) | (x >> (32 - n))

// ChaCha Quarter Round on 4 numbers: a,b,c,d
#define QROUND(a, b, c, d) \
	a += b; d ^= a; d = LROLL32(d, 16); \
	c += d; b ^= c; b = LROLL32(b, 12); \
	a += b; d ^= a; d = LROLL32(d, 8); \
	c += d; b ^= c; b = LROLL32(b, 7); \

// Converts 4 bytes into uint32_t, ensuring Little Endian byte order
static inline uint32_t load32_le(const uint8_t src[4]) {
    return ((uint32_t)src[0]      ) |
           ((uint32_t)src[1] <<  8) |
           ((uint32_t)src[2] << 16) |
           ((uint32_t)src[3] << 24);
}

// Converts uint32_t to 4 bytes, ensuring Little Endian byte order
static inline void dump32_le(const uint32_t n, uint8_t out[4]) {
    out[0] = (uint8_t)(n         & 0xFF);
    out[1] = (uint8_t)((n >> 8)  & 0xFF);
    out[2] = (uint8_t)((n >> 16) & 0xFF);
    out[3] = (uint8_t)((n >> 24) & 0xFF);
}

static inline void _chacha_init(uint32_t s[16], const uint32_t key[8], const uint32_t nonce[3], const uint32_t block_count) {
	// The first four words (0-3) are constants
	s[0] = 0x61707865; s[1] = 0x3320646e;
	s[2] = 0x79622d32; s[3] = 0x6b206574;
	// The next eight words (4-11) are taken from the 256-bit key by
	// reading the bytes in little-endian order, in 4-byte chunks
	s[4]  = key[0]; s[5]  = key[1];
	s[6]  = key[2]; s[7]  = key[3];
	s[8]  = key[4]; s[9]  = key[5];
	s[10] = key[6]; s[11] = key[7];
	// Word 12 is a block counter
	s[12] = block_count;
	// Words 13-15 are a nonce
	s[13] = nonce[0];
	s[14] = nonce[1];
	s[15] = nonce[2];
}

// ChaCha20 block function
void chacha20_block(uint8_t out[64], const uint32_t key[8], const uint32_t nonce[3], const uint32_t block_count) {
    // Create a ChaCha state
    uint32_t s[16];

    // Initialize the state
    _chacha_init(s, key, nonce, block_count);

    // Save the inital state
    uint32_t is[16];
    for (unsigned int i = 0; i < 16; i++) is[i] = s[i];

    // The rounds themselves
    for (unsigned int i = 0; i < 10; i++) {
        // 4 column rounds
        QROUND(s[0], s[4], s[8], s[12]);
        QROUND(s[1], s[5], s[9], s[13]);
        QROUND(s[2], s[6], s[10], s[14]);
        QROUND(s[3], s[7], s[11], s[15]);
        // 4 diogonal rounds
        QROUND(s[0], s[5], s[10], s[15]);
        QROUND(s[1], s[6], s[11], s[12]);
        QROUND(s[2], s[7], s[8], s[13]);
        QROUND(s[3], s[4], s[9], s[14]);
    }

    // Add (carryless) the inital state to the new state
    for (unsigned int i = 0; i < 16; i++) s[i] += is[i];

    // Serialize the state to 64 bytes
    // Just cast it? Sadly, big endian exists :(
    for (unsigned int i = 0; i < 16; i++) dump32_le(s[i], out + i * 4);
}

// Encrypt/decrypt a message, works in-place
void chacha20_cipher(const uint8_t key_bytes[32], const uint8_t nonce_bytes[12], const uint32_t counter, uint8_t *msg, const unsigned long len) {
    // I hate big endian :(
    uint32_t key[8];
    uint32_t nonce[3];
    for (unsigned int i = 0; i < 8; i++) key[i] = load32_le(key_bytes + i * 4);
    for (unsigned int i = 0; i < 3; i++) nonce[i] = load32_le(nonce_bytes + i * 4);

    uint8_t keystream[64];
    for (unsigned long j = 0; j < len / 64; j++) {
        chacha20_block(keystream, key, nonce, counter + j);
        // XOR the data block by the keystream block
        for (unsigned int i = 0; i < 64; i++) msg[j * 64 + i] ^= keystream[i];
    }

    // It is a little more tricky if we can't cut the data into 64-byte blocks evenly
    // Then we need to discard the extra keystream bytes
    if (len % 64) {
        unsigned long j = len / 64;
        chacha20_block(keystream, key, nonce, counter + j);
        // XOR the data block by the keystream block, discarding the extra bytes
        for (unsigned int i = 0; i < len % 64; i++) msg[j * 64 + i] ^= keystream[i];
    }
}
