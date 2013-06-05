#ifndef __JL_Colours_HG_20130318__
#define __JL_Colours_HG_20130318__

#include <cVector4.h>

/// Describes a colour
/// Initializes to dull grey
struct sColour
{
	sColour()
		: Ambient( 0.5f, 0.5f, 0.5f, 1.f )
		, Diffuse( 0.5f, 0.5f, 0.5f, 1.f )
		, Specular( 0.2f, 0.2f, 0.2f, 1.f ) {}
	sColour( const cColour4& a, const cColour4& d, const cColour4& s )
		: Ambient(a) , Diffuse(d), Specular(s) {}
	sColour( const sColour& other )
		: Ambient(other.Ambient), Diffuse(other.Diffuse), Specular(other.Specular) {}
	sColour& operator = ( const sColour& other )
	{
		if ( &other != this )
		{
			Ambient = other.Ambient;
			Diffuse = other.Diffuse;
			Specular = other.Specular;
		}
		return *this;
	}

	cColour4 Ambient;
	cColour4 Diffuse;
	cColour4 Specular;
};

/// Describes a colour material
struct sColourMaterial
{
	sColourMaterial()
		: Ambient( 0.5f, 0.5f, 0.5f, 1.f )
		, Diffuse( 0.5f, 0.5f, 0.5f, 1.f )
		, Specular( 0.2f, 0.2f, 0.2f, 1.f )
		, Emissive( 0.f, 0.f, 0.f, 0.f ) {}
	sColourMaterial( const cColour4& a, const cColour4& d, const cColour4& s )
		: Ambient(a) , Diffuse(d), Specular(s), Emissive( 0.f, 0.f, 0.f, 0.f ) {}
	sColourMaterial( const cColour4& a, const cColour4& d, const cColour4& s, const cColour4& e )
		: Ambient(a) , Diffuse(d), Specular(s), Emissive(e) {}
	sColourMaterial( const sColourMaterial& other )
		: Ambient(other.Ambient), Diffuse(other.Diffuse), Specular(other.Specular), Emissive(other.Emissive) {}
	sColourMaterial& operator = ( const sColourMaterial& other )
	{
		if ( &other != this )
		{
			Ambient = other.Ambient;
			Diffuse = other.Diffuse;
			Specular = other.Specular;
			Emissive = other.Emissive;
		}
		return *this;
	}
	cColour4 Ambient;
	cColour4 Diffuse;
	cColour4 Specular;  // w = power
	cColour4 Emissive;
};

namespace nColour
{
	const cColour4 WHITE = cColour4( 1.f, 1.f, 1.f, 1.f );
	const cColour4 BLACK = cColour4( 0.f, 0.f, 0.f, 1.f );
	const cColour4 RED = cColour4( 1.f, 0.f, 0.f, 1.f );
	const cColour4 GREEN = cColour4( 0.f, 1.f, 0.f, 1.f );
	const cColour4 BLUE = cColour4( 0.f, 0.f, 1.f, 1.f );
	const cColour4 YELLOW = cColour4( 1.f, 1.f, 0.f, 1.f );
	const cColour4 CYAN = cColour4( 0.f, 1.f, 1.f, 1.f );
	const cColour4 MAGENTA = cColour4( 1.f, 0.f, 1.f, 1.f );
	const cColour4 SILVER = cColour4( 0.75f, 0.75f, 0.75f, 1.f );
	const cColour4 LIGHT_STEEL_BLUE = cColour4( 0.69f, 0.77f, 0.87f, 1.f );
};

#endif