#pragma once
#include "pch.h"
#include "GothicMemoryLocations.h"

class zSTRING {
public:

	zSTRING( const char* str ) {
		XCALL( GothicMemoryLocations::zSTRING::ConstructorCharPtr );
	}


	const char* ToChar() const {
		XCALL( GothicMemoryLocations::zSTRING::ToChar );
	}

    int   allocater;
    char* vector;
    int   length;
    int   reserved;

    FORCEINLINE const char& operator[]( unsigned int index ) const {
        return vector[index];
    }
};
