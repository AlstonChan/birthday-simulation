#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"
#include "../menu.h"
#include "hash_config.h"

const hash_config_t hash_config[] = {
    {HASH_CONFIG_8BIT, "ToyHash8", "8-bit", "~2^4 = 16", "2^8 = 256"},
    {HASH_CONFIG_12BIT, "ToyHash12", "12-bit", "~2^6 = 64", "2^12 = 4096"},
    {HASH_CONFIG_16BIT, "ToyHash16", "16-bit", "~2^8 = 256", "2^16 = 65536"},
    {HASH_CONFIG_RIPEMD160, "RIPEMD-160", "160-bit", "~2^80", "2^160"},
    {HASH_CONFIG_SHA1, "SHA-1", "160-bit", "~2^80", "2^160"},
    {HASH_CONFIG_SHA3_256, "SHA3-256", "256-bit", "~2^128", "2^256"},
    {HASH_CONFIG_SHA256, "SHA-256", "256-bit", "~2^128", "2^256"},
    {HASH_CONFIG_SHA512, "SHA-512", "512-bit", "~2^256", "2^512"},
    {HASH_CONFIG_SHA384, "SHA-384", "384-bit", "~2^192", "2^384"},
    {HASH_CONFIG_KECCAK256, "Keccak-256", "256-bit", "~2^128", "2^256"},
};

const unsigned short hash_config_len = ARRAY_SIZE(hash_config);

struct ListMenuItem *get_hash_config_menu() {
  struct ListMenuItem *hash_config_menu = malloc(hash_config_len * sizeof(struct ListMenuItem));
  if (!hash_config_menu) {
    fprintf(stderr, "Memory allocation failed for hash config menu.\n");
    exit(EXIT_FAILURE);
  }

  for (unsigned short i = 0; i < hash_config_len; i++) {
    hash_config_menu[i].label = hash_config[i].label;

    size_t description_length =
        strlen(hash_config[i].bits) + 3; // 2 for parentheses + 1 for null terminator
    char *description_parentheses = malloc(description_length);
    if (!description_parentheses) {
      fprintf(stderr, "Memory allocation failed for hash config menu description.\n");
      exit(EXIT_FAILURE);
    }

    snprintf(description_parentheses, description_length, "(%s)", hash_config[i].bits);
    hash_config_menu[i].description = description_parentheses;
  }

  return hash_config_menu;
}