#pragma once
#include "pch.h"
#include "GothicMemoryLocations.h"

class zSTRING {
public:
    zSTRING( const char* str ) {
        reinterpret_cast<void( __fastcall* )( zSTRING*, int, const char* )>( GothicMemoryLocations::zSTRING::ConstructorCharPtr )( this, 0, str );
    }

    virtual ~zSTRING() {
        reinterpret_cast<void( __fastcall* )( zSTRING* )>( GothicMemoryLocations::zSTRING::DestructorCharPtr )( this );
    }

    const char* ToChar() const {
        XCALL( GothicMemoryLocations::zSTRING::ToChar );
    }

    char data[20];
};
