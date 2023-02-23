#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <windows.h>

int32_t minizip_extract_entry_cb(void* handle, void* userdata, mz_zip_file* file_info, const char* path)
{
    MZ_UNUSED(handle);
    MZ_UNUSED(userdata);
    MZ_UNUSED(path);

    /* Print the current entry extracting */
    std::cout << "Extracting " << file_info->filename << "\n";
    return MZ_OK;
}

bool start(char* cmd)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(NULL,  // No module name (use command line)
            cmd,              // Command line
            NULL,             // Process handle not inheritable
            NULL,             // Thread handle not inheritable
            FALSE,            // Set handle inheritance to FALSE
            0,                // No creation flags
            NULL,             // Use parent's environment block
            NULL,             // Use parent's starting directory
            &si,              // Pointer to STARTUPINFO structure
            &pi)              // Pointer to PROCESS_INFORMATION structure
    )
    {
        std::cout << "CreateProcess failed " << GetLastError() << "\n";
        return false;
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        // Too many or too few arguments
        std::cout << "Usage: " << argv[0] << " <package.zip>\n";
        return 1;
    }

    std::filesystem::path file = argv[1];

    if (!std::filesystem::exists(file))
    {
        std::cout << "File " << file.string() << "does not exist\n";
        return 1;
    }

    // Move zlib since this app depends on zlib
    std::filesystem::path zlib    = "zlib1.dll";
    std::filesystem::path zlibold = "zlib1-old.dll";

    if (std::filesystem::exists(zlibold))
    {
        std::filesystem::remove(zlibold);
    }

    if (std::filesystem::exists(zlib))
    {
        std::filesystem::rename(zlib, zlibold);
    }

    // Unpack
    std::cout << "Unpacking " << file.string() << "...\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    void*   reader    = NULL;
    int32_t err       = MZ_OK;
    int32_t err_close = MZ_OK;

    mz_zip_reader_create(&reader);
    mz_zip_reader_set_entry_cb(reader, NULL, minizip_extract_entry_cb);

    err = mz_zip_reader_open_file(reader, file.string().c_str());

    if (err != MZ_OK)
    {
        std::cout << "Error " << err << " while opening archive " << file.string() << "\n";
    }
    else
    {
        err = mz_zip_reader_save_all(reader, NULL);

        if (err != MZ_OK)
        {
            std::cout << "Error" << err << "while saving entries to disk: " << file.string() << "\n";
        }
    }

    err_close = mz_zip_reader_close(reader);
    if (err_close != MZ_OK)
    {
        std::cout << "Error" << err_close << "while closing archive for reading\n";
        err = err_close;
    }

    mz_zip_reader_delete(&reader);

    if (err == MZ_OK)
    {
        std::cout << "Update complete! Starting Quasar...\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));

        char q[] = "quasar.exe";
        start(q);
    }

    return err;
}
