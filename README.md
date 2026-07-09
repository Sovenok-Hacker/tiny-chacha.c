# tiny-chacha.c

## A small **[RFC 8439](https://www.rfc-editor.org/info/rfc8439/) ChaCha20** C implementation

Mostly optimized for **size** and **portability**, rather for **speed**, but it should be still fast enough

## Usage
Include **`chacha20.h`** in your C code, link against the library, and **you are ready** to use it in code!

The only function you will likely need is `chacha20_cipher`

Its signature is already explaining enough:
```c
void chacha20_cipher(uint8_t key_bytes[32], uint8_t nonce_bytes[12], uint32_t counter, uint8_t *msg, unsigned long len);
```

You pass it your key and nonce as byte arrays, a pointer to your data to encrypt, and the data length in bytes.

As for `counter`, it is the inital block counter. If you don't know what this does - no problem, just set it to **0** :)

If the word `nonce` is also not really familiar to you: it is made with only one purpose - to make the same plaintext result in different ciphertexts. You can just **increment it by 1** every encrypted message. Until you don't plan to send **4294967296** messages this is okay.

Is this function encrypting or decrypting the data - it actually does both. It XORs the input with the ChaCha keystream, so it is mathematically an **involution** - **decrypting data is just encrypting it again**.
