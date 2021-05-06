#pragma once

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Toolbox::fs {
    bool FileExists( const std::string& file );
    bool FolderExists( const std::string& dirName_in );
    bool FileExistsW( const std::wstring& file );
    bool FolderExistsW( const std::wstring& dirName_in );

    bool CreateDirectoryRecursive( const std::string& dirName );
    bool CreateDirectoryRecursiveW( const std::wstring& dirName );

    std::wstring GetExecutablePathW();
    std::wstring GetExecutableDirectoryW();
}

