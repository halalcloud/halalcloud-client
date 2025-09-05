/*
 * PROJECT:    Halal Cloud Client
 * FILE:       HalalCloud.Specification.Multiformats.h
 * PURPOSE:    Definition for Multiformats Specification
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef HALALCLOUD_SPECIFICATION_MULTIFORMATS
#define HALALCLOUD_SPECIFICATION_MULTIFORMATS

/**
 * @brief The type of Multibase.
 */
typedef enum _MULTIBASE_TYPE
{
    /**
     * @brief Hexadecimal (lowercase)
     */
    MULTIBASE_TYPE_BASE16 = 'f',

    /**
     * @brief Hexadecimal (uppercase)
     */
    MULTIBASE_TYPE_BASE16_UPPER = 'F',

    /**
     * @brief RFC4648 case-insensitive - no padding (lowercase)
     */
    MULTIBASE_TYPE_BASE32 = 'b',

    /**
     * @brief RFC4648 case-insensitive - no padding (uppercase)
     */
    MULTIBASE_TYPE_BASE32_UPPER = 'B',

    /**
     * @brief Base58 Bitcoin
     */
    MULTIBASE_TYPE_BASE58_BTC = 'z',

    /**
     * @brief RFC4648 no padding
     */
    MULTIBASE_TYPE_BASE64 = 'm',

    /**
     * @brief RFC4648 no padding
     */
    MULTIBASE_TYPE_BASE64_URL = 'u',

    /**
     * @brief RFC4648 with padding
     */
    MULTIBASE_TYPE_BASE64_URL_PAD = 'U',

} MULTIBASE_TYPE, *PMULTIBASE_TYPE;

/**
 * @brief The type of Multicodec.
 */
typedef enum _MULTICODEC_TYPE
{
    /**
     * @brief CIDv1
     */
    MULTICODEC_TYPE_CIDV1 = 0x01,

    /**
     * @brief InterPlanetary Linked Data Format - CBOR
     */
    MULTICODEC_TYPE_IPLD_CBOR = 0x51,

    /**
     * @brief InterPlanetary Linked Data Format - Raw binary
     */
    MULTICODEC_TYPE_IPLD_RAW = 0x55,

    /**
     * @brief InterPlanetary Linked Data Format - MerkleDAG protobuf
     */
    MULTICODEC_TYPE_IPLD_DAG_PB = 0x70,

    /**
     * @brief InterPlanetary Linked Data Format - MerkleDAG cbor
     */
    MULTICODEC_TYPE_IPLD_DAG_CBOR = 0x71,

    /**
     * @brief InterPlanetary Linked Data Format - libp2p Public Key
     */
    MULTICODEC_TYPE_IPLD_LIBP2P_KEY = 0x72,

    /**
     * @brief InterPlanetary Linked Data Format - Raw Git object
     */
    MULTICODEC_TYPE_IPLD_GIT_RAW = 0x78,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Header (RLP)
     */
    MULTICODEC_TYPE_IPLD_ETH_BLOCK = 0x90,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Block List (RLP)
     */
    MULTICODEC_TYPE_IPLD_ETH_BLOCK_LIST = 0x91,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Transaction Trie
     *        (Eth-Trie)
     */
    MULTICODEC_TYPE_IPLD_ETH_TX_TRIE = 0x92,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Transaction
     *        (MarshalBinary)
     */
    MULTICODEC_TYPE_IPLD_ETH_TX = 0x93,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Transaction Receipt
     *        Trie (Eth-Trie)
     */
    MULTICODEC_TYPE_IPLD_ETH_TX_RECEIPT_TRIE = 0x94,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Transaction Receipt
     *        (MarshalBinary)
     */
    MULTICODEC_TYPE_IPLD_ETH_TX_RECEIPT = 0x95,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum State Trie
     *        (Eth-Secure-Trie)
     */
    MULTICODEC_TYPE_IPLD_ETH_STATE_TRIE = 0x96,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Account Snapshot
     *        (RLP)
     */
    MULTICODEC_TYPE_IPLD_ETH_ACCOUNT_SNAPSHOT = 0x97,

    /**
     * @brief InterPlanetary Linked Data Format - Ethereum Contract Storage Trie
     *        (Eth-Secure-Trie)
     */
    MULTICODEC_TYPE_IPLD_ETH_STORAGE_TRIE = 0x98,

    /**
     * @brief InterPlanetary Linked Data Format - Bitcoin Block
     */
    MULTICODEC_TYPE_IPLD_BITCOIN_BLOCK = 0xB0,

    /**
     * @brief InterPlanetary Linked Data Format - Bitcoin Tx
     */
    MULTICODEC_TYPE_IPLD_BITCOIN_TX = 0xB1,

    /**
     * @brief InterPlanetary Linked Data Format - Bitcoin Witness Commitment
     */
    MULTICODEC_TYPE_IPLD_BITCOIN_WITNESS_COMMITMENT = 0xB2,

    /**
     * @brief InterPlanetary Linked Data Format - Zcash Block
     */
    MULTICODEC_TYPE_IPLD_ZCASH_BLOCK = 0xC0,

    /**
     * @brief InterPlanetary Linked Data Format - Zcash Tx
     */
    MULTICODEC_TYPE_IPLD_ZCASH_TX = 0xC1,

    /**
     * @brief InterPlanetary Linked Data Format - MerkleDAG json
     */
    MULTICODEC_TYPE_IPLD_DAG_JSON = 0x0129,

    /**
     * @brief InterPlanetary Linked Data Format - JSON (UTF-8-encoded)
     */
    MULTICODEC_TYPE_IPLD_JSON = 0x0200,

} MULTICODEC_TYPE, *PMULTICODEC_TYPE;

/**
 * @brief The type of Multihash.
 */
typedef enum _MULTIHASH_TYPE
{
    /**
     * @brief Raw binary
     */
    MULTIHASH_TYPE_IDENTITY = 0x00,

    /**
     * @brief SHA-1
     */
    MULTIHASH_TYPE_SHA1 = 0x11,

    /**
     * @brief SHA2-256
     */
    MULTIHASH_TYPE_SHA2_256 = 0x12,

    /**
     * @brief SHA2-512
     */
    MULTIHASH_TYPE_SHA2_512 = 0x13,

    /**
     * @brief SHA3-512
     */
    MULTIHASH_TYPE_SHA3_512 = 0x14,

    /**
     * @brief SHA3-384
     */
    MULTIHASH_TYPE_SHA3_384 = 0x15,

    /**
     * @brief SHA3-256
     */
    MULTIHASH_TYPE_SHA3_256 = 0x16,

    /**
     * @brief SHA3-224
     */
    MULTIHASH_TYPE_SHA3_224 = 0x17,

    /**
     * @brief A.k.a SHA-384; as specified by FIPS 180-4.
     */
    MULTIHASH_TYPE_SHA2_384 = 0x20,

    /**
     * @brief SHA2-256 with the two most significant bits from the last byte
     *        zeroed (as via a mask with 0b00111111), used for proving trees
     *        as in Filecoin.
     */
    MULTIHASH_TYPE_SHA2_256_TRUNC254_PADDED = 0x1012,

    /**
     * @brief A.k.a SHA-224; as specified by FIPS 180-4.
     */
    MULTIHASH_TYPE_SHA2_224 = 0x1013,

    /**
     * @brief A.k.a SHA-512/224; as specified by FIPS 180-4.
     */
    MULTIHASH_TYPE_SHA2_512_224 = 0x1014,

    /**
     * @brief A.k.a SHA-512/256; as specified by FIPS 180-4.
     */
    MULTIHASH_TYPE_SHA2_512_256 = 0x1015,

    /**
     * @brief BLAKE2b-256
     */
    MULTIHASH_TYPE_BLAKE2B_256 = 0xB220,

    /**
     * @brief Poseidon using BLS12-381 and arity of 2 with Filecoin parameters.
     */
    MULTIHASH_TYPE_POSEIDON_BLS12_381_A2_FC1 = 0xB401,

} MULTIHASH_TYPE, *PMULTIHASH_TYPE;

#endif // !HALALCLOUD_SPECIFICATION_MULTIFORMATS
