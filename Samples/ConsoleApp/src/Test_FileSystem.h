#pragma once
#include "file_system.h"
#include <cstdio>
#include <cstring>

namespace mark
{
    namespace test_filesystem_detail
    {
        static int g_total = 0;
        static int g_passed = 0;
        static int g_failed = 0;

        inline void check(bool cond, const char* tag)
        {
            ++g_total;
            if (cond)
            {
                ++g_passed;
            }
            else
            {
                ++g_failed;
                printf("  [FAIL] %s\n", tag);
            }
        }

        inline void reset_counters()
        {
            g_total = 0;
            g_passed = 0;
            g_failed = 0;
        }

        inline void print_summary()
        {
            printf("========================================\n");
            printf("  Total : %d   Passed : %d   Failed : %d\n", g_total, g_passed, g_failed);
            printf("========================================\n");
        }

#define FS_CHECK(expr, tag) mark::test_filesystem_detail::check((expr), (tag))
    }

    // ========================================================================
    // Initialize / Destroy
    // ========================================================================
    inline void TestFS_InitializeDestroy()
    {
        printf("[TestFS_InitializeDestroy]\n");

        // destroy any prior state
        file_system::destroy();

        // first initialize should succeed
        FS_CHECK(file_system::initialize("."), "initialize first call");

        // double initialize should fail
        FS_CHECK(!file_system::initialize("."), "initialize double call fails");

        // destroy
        file_system::destroy();

        // re-initialize after destroy should succeed
        FS_CHECK(file_system::initialize("."), "initialize after destroy");

        file_system::destroy();
    }

    // ========================================================================
    // Directory Operations
    // ========================================================================
    inline void TestFS_Directory()
    {
        printf("[TestFS_Directory]\n");

        file_system::destroy();
        FS_CHECK(file_system::initialize("."), "dir: initialize");

        // cleanup from previous runs
        file_system::remove_directory("_fs_test_dir_renamed");
        file_system::remove_directory("_fs_test_dir");

        // create directory
        FS_CHECK(file_system::create_directory("_fs_test_dir"), "create_directory");

        // exist directory
        FS_CHECK(file_system::exist_directory("_fs_test_dir"), "exist_directory after create");

        // create nested directory
        FS_CHECK(file_system::create_directory("_fs_test_dir/sub1/sub2"), "create_directory nested");
        FS_CHECK(file_system::exist_directory("_fs_test_dir/sub1/sub2"), "exist_directory nested");

        // rename directory
        FS_CHECK(file_system::rename_directory("_fs_test_dir", "_fs_test_dir_renamed"), "rename_directory");
        FS_CHECK(!file_system::exist_directory("_fs_test_dir"), "old dir gone after rename");
        FS_CHECK(file_system::exist_directory("_fs_test_dir_renamed"), "new dir exists after rename");

        // remove directory (recursive)
        FS_CHECK(file_system::remove_directory("_fs_test_dir_renamed"), "remove_directory");
        FS_CHECK(!file_system::exist_directory("_fs_test_dir_renamed"), "exist_directory after remove");

        // invalid args
        FS_CHECK(!file_system::create_directory(nullptr), "create_directory nullptr");
        FS_CHECK(!file_system::create_directory(""), "create_directory empty");
        FS_CHECK(!file_system::exist_directory(nullptr), "exist_directory nullptr");
        FS_CHECK(!file_system::remove_directory("_fs_nonexistent_dir"), "remove_directory nonexistent");

        file_system::destroy();
    }

    // ========================================================================
    // File Create / Remove / Exist
    // ========================================================================
    inline void TestFS_FileCreateRemove()
    {
        printf("[TestFS_FileCreateRemove]\n");

        file_system::destroy();
        FS_CHECK(file_system::initialize("."), "file: initialize");

        // cleanup
        file_system::remove_file("_fs_test_file.txt");

        // create file
        FS_CHECK(file_system::create_file("_fs_test_file.txt"), "create_file");
        FS_CHECK(file_system::exist_file("_fs_test_file.txt"), "exist_file after create");

        // create duplicate should fail
        FS_CHECK(!file_system::create_file("_fs_test_file.txt"), "create_file duplicate fails");

        // remove file
        FS_CHECK(file_system::remove_file("_fs_test_file.txt"), "remove_file");
        FS_CHECK(!file_system::exist_file("_fs_test_file.txt"), "exist_file after remove");

        // remove nonexistent should fail
        FS_CHECK(!file_system::remove_file("_fs_test_file.txt"), "remove_file nonexistent");

        // invalid args
        FS_CHECK(!file_system::create_file(nullptr), "create_file nullptr");
        FS_CHECK(!file_system::create_file(""), "create_file empty");
        FS_CHECK(!file_system::exist_file(nullptr), "exist_file nullptr");

        file_system::destroy();
    }

    // ========================================================================
    // File Open / Close / Write / Read
    // ========================================================================
    inline void TestFS_FileReadWrite()
    {
        printf("[TestFS_FileReadWrite]\n");

        file_system::destroy();
        FS_CHECK(file_system::initialize("."), "rw: initialize");

        // cleanup
        file_system::remove_file("_fs_test_rw.bin");

        // open for write (flag 1)
        HANDLE wh = file_system::open_file("_fs_test_rw.bin", 1);
        FS_CHECK(wh != nullptr, "open_file write");

        // write data
        const char write_data[] = "Hello, FileSystem!";
        size_t write_len = strlen(write_data);
        size_t written = file_system::write_file(wh, write_data, write_len);
        FS_CHECK(written == write_len, "write_file byte count");

        file_system::close_file(wh);
        wh = nullptr;

        // file should exist
        FS_CHECK(file_system::exist_file("_fs_test_rw.bin"), "exist_file after write");

        // open for read (flag 0)
        HANDLE rh = file_system::open_file("_fs_test_rw.bin", 0);
        FS_CHECK(rh != nullptr, "open_file read");

        // read data
        char read_buf[64] = { 0 };
        size_t read_bytes = file_system::read_file(rh, read_buf, write_len);
        FS_CHECK(read_bytes == write_len, "read_file byte count");
        FS_CHECK(memcmp(read_buf, write_data, write_len) == 0, "read_file data matches");

        file_system::close_file(rh);
        rh = nullptr;

        // invalid args
        FS_CHECK(file_system::open_file(nullptr, 0) == nullptr, "open_file nullptr");
        FS_CHECK(file_system::open_file("", 0) == nullptr, "open_file empty");
        FS_CHECK(file_system::open_file("_fs_test_rw.bin", 99) == nullptr, "open_file invalid flag");
        FS_CHECK(file_system::read_file(nullptr, read_buf, 1) == 0, "read_file null handle");
        FS_CHECK(file_system::write_file(nullptr, write_data, 1) == 0, "write_file null handle");

        // cleanup
        file_system::remove_file("_fs_test_rw.bin");
        file_system::destroy();
    }

    // ========================================================================
    // Seek / Tell
    // ========================================================================
    inline void TestFS_SeekTell()
    {
        printf("[TestFS_SeekTell]\n");

        file_system::destroy();
        FS_CHECK(file_system::initialize("."), "seek: initialize");

        // cleanup
        file_system::remove_file("_fs_test_seek.bin");

        // write test data
        {
            HANDLE wh = file_system::open_file("_fs_test_seek.bin", 1);
            FS_CHECK(wh != nullptr, "seek: open_file write");
            const char data[] = "ABCDEFGHIJ"; // 10 bytes
            file_system::write_file(wh, data, 10);
            file_system::close_file(wh);
        }

        // open for read
        HANDLE rh = file_system::open_file("_fs_test_seek.bin", 0);
        FS_CHECK(rh != nullptr, "seek: open_file read");

        // tell at start
        FS_CHECK(file_system::tell_file(rh) == 0, "tell_file at start == 0");

        // seek from start (origin 0 = SEEK_SET)
        FS_CHECK(file_system::seek_file(rh, 5, 0), "seek_file SEEK_SET 5");
        FS_CHECK(file_system::tell_file(rh) == 5, "tell_file after SEEK_SET 5");

        // read one byte to verify position
        char ch = 0;
        file_system::read_file(rh, &ch, 1);
        FS_CHECK(ch == 'F', "read at position 5 == 'F'");

        // seek from current (origin 1 = SEEK_CUR)
        FS_CHECK(file_system::seek_file(rh, 2, 1), "seek_file SEEK_CUR +2");
        file_system::read_file(rh, &ch, 1);
        FS_CHECK(ch == 'I', "read after SEEK_CUR +2 == 'I'");

        // seek from end (origin 2 = SEEK_END)
        FS_CHECK(file_system::seek_file(rh, -3, 2), "seek_file SEEK_END -3");
        file_system::read_file(rh, &ch, 1);
        FS_CHECK(ch == 'H', "read after SEEK_END -3 == 'H'");

        // invalid origin
        FS_CHECK(!file_system::seek_file(rh, 0, 99), "seek_file invalid origin");
        FS_CHECK(!file_system::seek_file(nullptr, 0, 0), "seek_file null handle");
        FS_CHECK(file_system::tell_file(nullptr) == 0, "tell_file null handle");

        file_system::close_file(rh);

        // cleanup
        file_system::remove_file("_fs_test_seek.bin");
        file_system::destroy();
    }

    // ========================================================================
    // Read All
    // ========================================================================
    inline void TestFS_ReadAll()
    {
        printf("[TestFS_ReadAll]\n");

        file_system::destroy();
        FS_CHECK(file_system::initialize("."), "readall: initialize");

        // cleanup
        file_system::remove_file("_fs_test_readall.bin");

        // write test data
        const char data[] = "ReadAll Test Data 1234567890";
        size_t data_len = strlen(data);
        {
            HANDLE wh = file_system::open_file("_fs_test_readall.bin", 1);
            FS_CHECK(wh != nullptr, "readall: open_file write");
            file_system::write_file(wh, data, data_len);
            file_system::close_file(wh);
        }

        // open for read
        HANDLE rh = file_system::open_file("_fs_test_readall.bin", 0);
        FS_CHECK(rh != nullptr, "readall: open_file read");

        // read_all
        FS_CHECK(file_system::read_all(rh), "read_all success");

        // get_read_all_buffer / get_read_all_size
        void* buf = file_system::get_read_all_buffer(rh);
        size_t buf_size = file_system::get_read_all_size(rh);
        FS_CHECK(buf != nullptr, "get_read_all_buffer not null");
        FS_CHECK(buf_size == data_len, "get_read_all_size matches");
        FS_CHECK(memcmp(buf, data, data_len) == 0, "get_read_all_buffer data matches");

        // null handle
        FS_CHECK(!file_system::read_all(nullptr), "read_all null handle");
        FS_CHECK(file_system::get_read_all_buffer(nullptr) == nullptr, "get_read_all_buffer null handle");
        FS_CHECK(file_system::get_read_all_size(nullptr) == 0, "get_read_all_size null handle");

        file_system::close_file(rh);

        // cleanup
        file_system::remove_file("_fs_test_readall.bin");
        file_system::destroy();
    }

    // ========================================================================
    // Read/Write mode (flag 2)
    // ========================================================================
    inline void TestFS_ReadWriteMode()
    {
        printf("[TestFS_ReadWriteMode]\n");

        file_system::destroy();
        FS_CHECK(file_system::initialize("."), "rwmode: initialize");

        // cleanup & create initial file
        file_system::remove_file("_fs_test_rwmode.bin");
        {
            HANDLE wh = file_system::open_file("_fs_test_rwmode.bin", 1);
            FS_CHECK(wh != nullptr, "rwmode: create file");
            const char init_data[] = "AAAAAAAAAA"; // 10 bytes
            file_system::write_file(wh, init_data, 10);
            file_system::close_file(wh);
        }

        // open for read/write (flag 2)
        HANDLE rwh = file_system::open_file("_fs_test_rwmode.bin", 2);
        FS_CHECK(rwh != nullptr, "open_file r+b");

        // seek to position 3 and overwrite
        FS_CHECK(file_system::seek_file(rwh, 3, 0), "rwmode: seek to 3");
        const char patch[] = "XYZ";
        size_t written = file_system::write_file(rwh, patch, 3);
        FS_CHECK(written == 3, "rwmode: write 3 bytes");

        // seek back and read all
        FS_CHECK(file_system::seek_file(rwh, 0, 0), "rwmode: seek to 0");
        char buf[16] = { 0 };
        size_t read_bytes = file_system::read_file(rwh, buf, 10);
        FS_CHECK(read_bytes == 10, "rwmode: read 10 bytes");
        FS_CHECK(memcmp(buf, "AAAXYZAAAA", 10) == 0, "rwmode: patched data matches");

        file_system::close_file(rwh);

        // cleanup
        file_system::remove_file("_fs_test_rwmode.bin");
        file_system::destroy();
    }

    // ========================================================================
    // Operations without initialize
    // ========================================================================
    inline void TestFS_WithoutInit()
    {
        printf("[TestFS_WithoutInit]\n");

        file_system::destroy();

        // all operations should fail without initialize
        FS_CHECK(!file_system::create_directory("_fs_noinit"), "create_directory without init");
        FS_CHECK(!file_system::exist_directory("_fs_noinit"), "exist_directory without init");
        FS_CHECK(!file_system::remove_directory("_fs_noinit"), "remove_directory without init");
        FS_CHECK(!file_system::rename_directory("a", "b"), "rename_directory without init");
        FS_CHECK(!file_system::create_file("_fs_noinit.txt"), "create_file without init");
        FS_CHECK(!file_system::exist_file("_fs_noinit.txt"), "exist_file without init");
        FS_CHECK(!file_system::remove_file("_fs_noinit.txt"), "remove_file without init");
        FS_CHECK(file_system::open_file("_fs_noinit.txt", 0) == nullptr, "open_file without init");
    }

    // ========================================================================
    // close_file null safety
    // ========================================================================
    inline void TestFS_CloseNull()
    {
        printf("[TestFS_CloseNull]\n");

        // should not crash
        file_system::close_file(nullptr);
        FS_CHECK(true, "close_file nullptr no crash");
    }

    // ========================================================================
    // get_file_extension
    // ========================================================================
    inline void TestFS_GetFileExtension()
    {
        printf("[TestFS_GetFileExtension]\n");

        char ext[64] = { 0 };

        // normal case
        file_system::get_file_extension("folder/file.txt", ext, sizeof(ext));
        FS_CHECK(strcmp(ext, ".txt") == 0, "ext: folder/file.txt -> .txt");

        file_system::get_file_extension("archive.tar.gz", ext, sizeof(ext));
        FS_CHECK(strcmp(ext, ".gz") == 0, "ext: archive.tar.gz -> .gz");

        file_system::get_file_extension("path\\to\\image.png", ext, sizeof(ext));
        FS_CHECK(strcmp(ext, ".png") == 0, "ext: backslash path .png");

        // no extension
        file_system::get_file_extension("Makefile", ext, sizeof(ext));
        FS_CHECK(ext[0] == '\0', "ext: no extension -> empty");

        // hidden file (no extension)
        file_system::get_file_extension(".gitignore", ext, sizeof(ext));
        FS_CHECK(ext[0] == '\0', "ext: .gitignore -> empty");

        // trailing dot
        file_system::get_file_extension("file.", ext, sizeof(ext));
        FS_CHECK(ext[0] == '\0', "ext: file. -> empty");

        // null / empty
        file_system::get_file_extension(nullptr, ext, sizeof(ext));
        FS_CHECK(ext[0] == '\0', "ext: nullptr -> empty");

        file_system::get_file_extension("", ext, sizeof(ext));
        FS_CHECK(ext[0] == '\0', "ext: empty -> empty");

        // small buffer
        file_system::get_file_extension("file.txt", ext, 3);
        FS_CHECK(strcmp(ext, ".t") == 0, "ext: small buffer truncation");
    }

    // ========================================================================
    // change_extension
    // ========================================================================
    inline void TestFS_ChangeExtension()
    {
        printf("[TestFS_ChangeExtension]\n");

        char out[128] = { 0 };

        file_system::change_extension("folder/file.txt", ".bin", out, sizeof(out));
        FS_CHECK(strcmp(out, "folder/file.bin") == 0, "change_ext: .txt -> .bin");

        file_system::change_extension("archive.tar.gz", ".zip", out, sizeof(out));
        FS_CHECK(strcmp(out, "archive.tar.zip") == 0, "change_ext: .gz -> .zip");

        // null / empty
        file_system::change_extension(nullptr, ".bin", out, sizeof(out));
        FS_CHECK(out[0] == '\0', "change_ext: nullptr path");

        file_system::change_extension("file.txt", nullptr, out, sizeof(out));
        FS_CHECK(out[0] == '\0', "change_ext: nullptr ext");

        file_system::change_extension("", ".bin", out, sizeof(out));
        FS_CHECK(out[0] == '\0', "change_ext: empty path");
    }

    // ========================================================================
    // get_filename_without_extension
    // ========================================================================
    inline void TestFS_GetFilenameWithoutExtension()
    {
        printf("[TestFS_GetFilenameWithoutExtension]\n");

        char out[128] = { 0 };

        file_system::get_filename_without_extension("folder/file.txt", out, sizeof(out));
        FS_CHECK(strcmp(out, "file") == 0, "stem: folder/file.txt -> file");

        file_system::get_filename_without_extension("archive.tar.gz", out, sizeof(out));
        FS_CHECK(strcmp(out, "archive.tar") == 0, "stem: archive.tar.gz -> archive.tar");

        file_system::get_filename_without_extension("path\\to\\image.png", out, sizeof(out));
        FS_CHECK(strcmp(out, "image") == 0, "stem: backslash path");

        file_system::get_filename_without_extension("Makefile", out, sizeof(out));
        FS_CHECK(strcmp(out, "Makefile") == 0, "stem: no extension");

        file_system::get_filename_without_extension(".gitignore", out, sizeof(out));
        FS_CHECK(strcmp(out, ".gitignore") == 0, "stem: hidden file");

        file_system::get_filename_without_extension("file.", out, sizeof(out));
        FS_CHECK(strcmp(out, "file.") == 0, "stem: trailing dot");

        // null / empty
        file_system::get_filename_without_extension(nullptr, out, sizeof(out));
        FS_CHECK(out[0] == '\0', "stem: nullptr");

        file_system::get_filename_without_extension("", out, sizeof(out));
        FS_CHECK(out[0] == '\0', "stem: empty");
    }

    // ========================================================================
    // get_filename
    // ========================================================================
    inline void TestFS_GetFilename()
    {
        printf("[TestFS_GetFilename]\n");

        char out[128] = { 0 };

        file_system::get_filename("folder/file.txt", out, sizeof(out));
        FS_CHECK(strcmp(out, "file.txt") == 0, "filename: folder/file.txt -> file.txt");

        file_system::get_filename("path\\to\\image.png", out, sizeof(out));
        FS_CHECK(strcmp(out, "image.png") == 0, "filename: backslash path");

        file_system::get_filename("justfile.bin", out, sizeof(out));
        FS_CHECK(strcmp(out, "justfile.bin") == 0, "filename: no directory");

        // null / empty
        file_system::get_filename(nullptr, out, sizeof(out));
        FS_CHECK(out[0] == '\0', "filename: nullptr");

        file_system::get_filename("", out, sizeof(out));
        FS_CHECK(out[0] == '\0', "filename: empty");
    }

    // ========================================================================
    // get_directory
    // ========================================================================
    inline void TestFS_GetDirectory()
    {
        printf("[TestFS_GetDirectory]\n");

        char out[128] = { 0 };

        file_system::get_directory("folder/file.txt", out, sizeof(out), false);
        FS_CHECK(strcmp(out, "folder/") == 0, "dir: folder/file.txt -> folder/");

        file_system::get_directory("a/b/c/file.txt", out, sizeof(out), false);
        FS_CHECK(strcmp(out, "a/b/c/") == 0, "dir: nested path");

        file_system::get_directory("path\\to\\file.txt", out, sizeof(out), false);
        FS_CHECK(strcmp(out, "path\\to\\") == 0, "dir: backslash path");

        // no directory
        file_system::get_directory("file.txt", out, sizeof(out), false);
        FS_CHECK(out[0] == '\0', "dir: no directory -> empty");

        // null / empty
        file_system::get_directory(nullptr, out, sizeof(out), false);
        FS_CHECK(out[0] == '\0', "dir: nullptr");

        file_system::get_directory("", out, sizeof(out), false);
        FS_CHECK(out[0] == '\0', "dir: empty");
    }

    // ========================================================================
    // Entry Point: Test_FileSystem()
    // ========================================================================
    inline void Test_FileSystem()
    {
        using namespace test_filesystem_detail;
        reset_counters();

        printf("========================================\n");
        printf("  FileSystem Test Suite\n");
        printf("========================================\n");

        TestFS_InitializeDestroy();
        TestFS_Directory();
        TestFS_FileCreateRemove();
        TestFS_FileReadWrite();
        TestFS_SeekTell();
        TestFS_ReadAll();
        TestFS_ReadWriteMode();
        TestFS_WithoutInit();
        TestFS_CloseNull();
        TestFS_GetFileExtension();
        TestFS_ChangeExtension();
        TestFS_GetFilenameWithoutExtension();
        TestFS_GetFilename();
        TestFS_GetDirectory();

        print_summary();
    }

} // namespace mark
