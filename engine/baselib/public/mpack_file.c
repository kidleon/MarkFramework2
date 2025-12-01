#include "pch.h"
#include "mpack_file.h"




#pragma pack(push, 1)

/**
* @brief MPACK 파일 헤더 구조체
* @details MPACK 파일의 기본 정보를 담고 있는 헤더 구조체입니다.
*/
struct mpack_file_hader_t
{
	uint32_t magic; // "MPK!"
	uint32_t version; // MPACK_FILE_VERSION(major, minor)
	uint32_t flags; // 파일 플래그
	uint32_t num_directory; // 디렉토리 수
	uint32_t num_entity; // 엔티티(파일) 수
	uint32_t name_offs; // mpack 이름의 이름 테이블 오프셋
	uint32_t name_len; // mpack 이름의 이름 테이블 길이
	uint32_t hash_offs; // 해시 테이블 오프셋
	uint32_t hash_len; // 해시 테이블 길이
	uint64_t data_offs; // 데이터 오프셋
	uint64_t data_len; // 데이터 길이
};

/**
* @brief MPACK UUID 구조체
* @details MPACK 파일의 UUID를 담고 있는 구조체입니다. MPACK자체의 고유 식별자 역할을 합니다. 다만 MPACK내부의 엔티티는 용량상 uuid를 가지고 있지 않습니다.
*/
struct mpack_uuid_t
{
	uint8_t uuid[32];
};

/**
* @brief MPACK 엔티티 해시 구조체
*/
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

	uint64_t comp_size;
	uint64_t uncomp_size;

	uint32_t num_chunk;
	uint32_t chunk_size;
	uint64_t chunk_offs;
	uint64_t chunk_len;
};

struct mpack_chunk_t
{
	uint64_t comp_size;
	uint64_t uncomp_size;
	uint64_t data_offs;
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
#define MPACK_ENITIY_FLAG_COMPRESS_ENCRYPT (MPACK_FILE_FLAG_COMPRESS | MPACK_FILE_FLAG_ENCRYPT)

#define MPACK_ENTITY_CHUNK_SIZE 65536 // 64KB 기본 청크 크기


