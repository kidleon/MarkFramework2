#include "pch.h"
#include "mpack_file.h"


#pragma pack(push, 1)

struct mpack_file_hader_t
{
	uint32_t magic; // "MPK!"
	uint32_t version; // MAKP_FILE_VERSION(major, minor)
	uint32_t flags;
	uint32_t num_directory;
	uint32_t num_entity;
	uint32_t name_offs;
	uint32_t name_len;
	uint32_t hash_offs;
	uint32_t hash_len;
	uint64_t data_offs;
	uint64_t data_len;
};

struct mpack_uuid_t
{
	uint8_t uuid[32];
};

struct mpack_hash_t
{
	uint32_t hash;
	uint32_t entity_id;
};

struct mpack_directory_t
{
	uint32_t hash;
	uint32_t parent;
	uint32_t name_offs;
	uint32_t name_len;
};

struct mpack_entity_t
{
	uint32_t hash;
	uint32_t parent;
	uint32_t name_offs;
	uint32_t name_len;

	uint32_t flags;
	uint32_t crc32;
	uint32_t num_chunk;

	uint64_t comp_size;
	uint64_t uncomp_size;
	uint64_t data_offs;
};

struct mpack_chunk_t
{
	uint64_t comp_size;
	uint64_t uncomp_size;
};

#pragma pack(pop)

#define MPACK_FILE_MAGIC 0x214B504D  // "MPK!"
#define MPACK_FILE_VERSION(major, minor) (((major) << 16) | (minor))
#define MPACK_FILE_GET_VERSION_MAJIOR(version) (((version) >> 16) & 0xFFFF)
#define MPACK_FILE_GET_VERSION_MINOR(version) ((version) & 0xFFFF)
#define MPACK_FILE_VERSION_MAJOR 0
#define MPACK_FILE_VERSION_MINOR 1 

#define MPACK_FILE_FLAG_NONE 0x00000000
#define MPACK_FILE_FLAG_WRITE_LOCKED 0x00000001 // 쓰기 잠금

#define MPACK_ENITIY_FLAG_NONE 0x00000000
#define MPACK_ENITIY_FLAG_COMPRESS 0x00000001
#define MPACK_ENITIY_FLAG_ENCRYPT 0x00000002
#define MPACK_ENITIY_FLAG_HIDDEN 0x00000004 
#define MPACK_ENITIY_FLAG_DELETED 0x80000000

#define MPACK_FILE_ENITIY_FLAG_COMPRESS_ENCRYPT (MPACK_FILE_FLAG_COMPRESS | MPACK_FILE_FLAG_ENCRYPT)


