#include "pch.h"
#include "mpack_file.h"
#include "crt_memory.h"
#include "guid.h"
#include "os_file.h"
#include "strings.h"


#pragma pack(push, 1)

/**
* @brief MPACK 파일 헤더 구조체
* @details MPACK 파일의 기본 정보를 담고 있는 헤더 구조체입니다.
*/
struct mpack_header_t
{
	uint32_t magic; // "MPK!"
	uint32_t version; // MPACK_FILE_VERSION(major, minor)
	uint32_t flags; // 파일 플래그
	uint8_t uuid[32];
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
	uint64_t chunk_offs;
	uint32_t chunk_size;
};

struct mpack_chunk_t
{
	uint64_t comp_size;
	uint64_t uncomp_size;
	uint64_t data_offs;
};

#pragma pack(pop)

struct mpack_directory


struct mpack_t
{
	HANDLE file;
	
};


#define MPACK_FILE_MAGIC 0x214B504D  // "MPK!"
#define MPACK_FILE_VERSION(major, minor) (((major) << 16) | (minor))
#define MPACK_FILE_GET_VERSION_MAJIOR(version) (((version) >> 16) & 0xFFFF)
#define MPACK_FILE_GET_VERSION_MINOR(version) ((version) & 0xFFFF)
#define MPACK_FILE_VERSION_MAJOR 0
#define MPACK_FILE_VERSION_MINOR 1 

#define MPACK_FILE_FLAG_NONE 0x00000000
#define MPACK_FILE_FLAG_WRITE_LOCKED 0x00000001 // 쓰기 잠금
#define MPACK_FILE_FLAG_PASSWORD 0x00000002 // 패스워드 보호

#define MPACK_ENITIY_FLAG_NONE 0x00000000
#define MPACK_ENITIY_FLAG_COMPRESS 0x00000001
#define MPACK_ENITIY_FLAG_ENCRYPT 0x00000002
#define MPACK_ENITIY_FLAG_HIDDEN 0x00000004 
#define MPACK_ENITIY_FLAG_DELETED 0x80000000
#define MPACK_ENITIY_FLAG_COMPRESS_ENCRYPT (MPACK_FILE_FLAG_COMPRESS | MPACK_FILE_FLAG_ENCRYPT)

#define MPACK_ENTITY_CHUNK_SIZE 65536 // 64KB 기본 청크 크기


BOOL mpack_create(
	const char* name,
	const char* filepath,
	const char* password,
	const char* uuid
)
{
	if (exist_file(filepath))
		return FALSE;

	struct mpack_header_t* header = (struct mpack_header_t*)crt_malloc_align(sizeof(struct mpack_header_t), 1);
	if (!header)
		goto lb_failed;

	memset(header, 0, sizeof(struct mpack_header_t));

	header->magic = MPACK_FILE_MAGIC;
	header->version = MPACK_FILE_VERSION(MPACK_FILE_VERSION_MAJOR, MPACK_FILE_VERSION_MINOR);
	header->flags = MPACK_FILE_FLAG_NONE;

	if(password && fstrlen(password) > 0)
		header->flags |= MPACK_FILE_FLAG_PASSWORD;

	if (!uuid || !fstrlen(uuid))
	{
		// UUID 생성
		if (!make_guid((char**)&header->uuid, 32))
			goto lb_failed;
	}
	else
	{
		// 전달된 UUID 복사
		fstrlcpy(header->uuid, uuid, 32);
	}

	HANDLE file = open_file(filepath, FILE_MODE_WRITE_BINARY);
	if (!file)
		goto lb_failed;

	write_file(file, header, sizeof(struct mpack_header_t));

	size_t offs = tell_file(file);

	if (name && fstrlen(name) > 0)
	{
		header->name_offs = (uint32_t)offs;
		header->name_len = (uint32_t)(fstrlen(name) + 1);
		write_file(file, name, header->name_len);
		offs += header->name_len;
	}

	seek_file(file, 0, FILE_SEEK_SET);
	write_file(file, header, sizeof(struct mpack_header_t));

	close_file(file);

	return TRUE;

lb_failed:
	if (header)
		crt_free_align(header);

	return FALSE;
}

BOOL mpack_check(
	const char* filepath
)
{
	if (!exist_file(filepath))
		return FALSE;

	HANDLE file = open_file(filepath, FILE_MODE_READ_BINARY);
	if (!file)
		return FALSE;

	struct mpack_header_t header;
	size_t read_bytes = write_file(file, &header, sizeof(struct mpack_header_t));
	close_file(file);

	if (read_bytes != sizeof(struct mpack_header_t))
		return FALSE;

	if (header.magic != MPACK_FILE_MAGIC)
		return FALSE;

	return TRUE;
}

HANDLE mpack_open(
	const char* filepath,
	const char* password
)
{

}