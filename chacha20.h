#include <stdint.h>

static inline uint32_t load32_le(const uint8_t src[4]);
static inline void dump32_le(const uint32_t n, uint8_t out[4]);

typedef uint32_t chacha_state_t[16];

static inline void _chacha_init(chacha_state_t s, const uint32_t key[8], const uint32_t nonce[3], const uint32_t block_count);
void chacha20_block(uint8_t out[64], const uint32_t key[8], const uint32_t nonce[3], const uint32_t block_count);

void chacha20_cipher(const uint8_t key_bytes[32], const uint8_t nonce_bytes[12], const uint32_t counter, uint8_t *msg, const unsigned long len);
