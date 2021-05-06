#include "Toolbox_fs.h"

namespace Toolbox::fs {
    bool FileExists( const std::string& file ) {
        FILE* f = fopen( file.c_str(), "rb" );

        if ( f ) {
            fclose( f );
            return true;
        }
        return false;
    }

    bool FolderExists( const std::string& dirName_in ) {
        DWORD ftyp = GetFileAttributesA( dirName_in.c_str() );
        if ( ftyp == INVALID_FILE_ATTRIBUTES )
            return false;  //something is wrong with your path!

        if ( ftyp & FILE_ATTRIBUTE_DIRECTORY )
            return true;   // this is a directory!

        return false;    // this is not a directory!
    }

    bool FileExistsW( const std::wstring& file ) {
        FILE* f = _wfopen( file.c_str(), L"rb" );

        if ( f ) {
            fclose( f );
            return true;
        }
        return false;
    }

    bool FolderExistsW( const std::wstring& dirName_in ) {
        DWORD ftyp = GetFileAttributesW( dirName_in.c_str() );
        if ( ftyp == INVALID_FILE_ATTRIBUTES )
            return false;  //something is wrong with your path!

        if ( ftyp & FILE_ATTRIBUTE_DIRECTORY )
            return true;   // this is a directory!

        return false;    // this is not a directory!
    }

    bool CreateDirectoryRecursive( const std::string& dirName ) {
        unsigned int pos = 0;
        do {
            pos = dirName.find_first_of( "\\/", pos + 1 );
            if ( CreateDirectory( dirName.substr( 0, pos ).c_str(), NULL ) == 0 && GetLastError() != ERROR_ALREADY_EXISTS ) {
                return false;
            }
        } while ( pos != std::string::npos );
        return true;
    }

    bool CreateDirectoryRecursiveW( const std::wstring& dirName ) {
        unsigned int pos = 0;
        do {
            pos = dirName.find_first_of( L"\\/", pos + 1 );
            if ( CreateDirectoryW( dirName.substr( 0, pos ).c_str(), NULL ) == 0 && GetLastError() != ERROR_ALREADY_EXISTS ) {
                return false;
            }
        } while ( pos != std::wstring::npos );
        return true;
    }

    std::wstring GetExecutablePathW() {
        std::wstring buf( MAX_PATH, L'\0' );
        DWORD nLen = ::GetModuleFileNameW( NULL, &buf[0], MAX_PATH );
        buf.resize( nLen );
        return buf;
    }
    std::wstring GetExecutableDirectoryW() {
        std::wstring buf( MAX_PATH, L'\0' );
        DWORD nLen = ::GetModuleFileNameW( NULL, &buf[0], MAX_PATH );
        buf.resize( nLen );
        buf = buf.substr( 0, buf.find_last_of( '\\' ) + 1 );
        return buf;
    }
}
