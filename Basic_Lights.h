#ifndef __JL_Basic_Lights_HG_20130318__
#define __JL_Basic_Lights_HG_20130318__

#include "Colours.h"

/// Describes a directional light
/// A directional light has a colour and a direction
struct sDirectionalLight
{
	sDirectionalLight() : Colour(), Direction(0.f, -1.f, 0.f), unused(1.f) {}
	sDirectionalLight( const cColour4& ambient, const cColour4& diffuse, const cColour4& specular, const cVector3& direction )
		: Colour(ambient,diffuse,specular), Direction(direction), unused(1.f) {}
	sDirectionalLight( const sDirectionalLight& other )
		: Colour(other.Colour), Direction(other.Direction), unused(other.unused) {}
	sDirectionalLight& operator = ( const sDirectionalLight& other )
	{
		if ( &other != this )
		{
			Colour = other.Colour;
			Direction = other.Direction;
			unused = other.unused;
		}
		return *this;
	}

	sColour		Colour;
	cVector3	Direction;
	float		unused;
};

/// Describes a point light
/// A point light has a colour, position, range, and attenuation
struct sPointLight
{
	sColour		Colour;
	cVector3	Position;
	float		Range;
	cVector3	Attenuation;
	float		unused;
};

/// Describes a spot light
/// A spot light has a colour, position, direction, range, and attenuation
/// Just light a point light, but has a direction
struct sSpotLight
{
	sColour		Colour;
	cVector3	Position;
	float		Range;
	cVector3	Direction;
	float		Spot;
	cVector3	Attenuation;
	float		unused;
};

// default is an unshadowed directional light pointing straight down
struct sGeneralLight
{
	sGeneralLight() : Colour(), Direction(0.f,1.f,0.f)
	{
		posPad = 1.f;
		dirPad = 1.f;
		attPad = 0.f;
		morePad = 0.f;

		Shadowed = 0;
		ShadowFactor = 0.f;
		ShadowMapID = 0;
		OnOff = 0;
		Spot = 0.1f;
		Range = 1000.f;
	}

	sGeneralLight( const sGeneralLight& other )
	{
		Colour = other.Colour;

		Position = other.Position;
		posPad = other.posPad;
		Direction = other.Direction;
		dirPad = other.dirPad;
		Attenuation = other.Attenuation;
		attPad = other.attPad;

		Spot = other.Spot;
		Range = other.Range;
		ShadowFactor = other.ShadowFactor;
		morePad = other.morePad;

		Shadowed = other.Shadowed;
		ShadowMapID = other.ShadowMapID;
		Type = other.Type;
		OnOff = other.OnOff;
	}

	sGeneralLight& operator = ( const sGeneralLight& other )
	{
		if ( &other != this )
		{
			Colour = other.Colour;

			Position = other.Position;
			posPad = other.posPad;
			Direction = other.Direction;
			dirPad = other.dirPad;
			Attenuation = other.Attenuation;
			attPad = other.attPad;

			Spot = other.Spot;
			Range = other.Range;
			ShadowFactor = other.ShadowFactor;
			morePad = other.morePad;

			Shadowed = other.Shadowed;
			ShadowMapID = other.ShadowMapID;
			Type = other.Type;
			OnOff = other.OnOff;
		}
		return *this;
	}

	void MakeShadowed( unsigned int mapID, float factor )
	{
		Shadowed = 1;
		ShadowFactor = factor;
		ShadowMapID = mapID;
	}

	bool IsOn()
	{
		return ( OnOff != 0 );
	}

	bool IsOff()
	{
		return ( OnOff == 0 );
	}

	bool IsShadowed()
	{
		return ( Shadowed == 1 );
	}

	void TurnOn()
	{
		OnOff = 1;
	}

	void TurnOff()
	{
		OnOff = 0;
	}

	sColour		Colour;
	cVector3	Position;
	float		posPad;
	cVector3	Direction;
	float		dirPad;
	cVector3	Attenuation;
	float		attPad;
	
	float		Spot;
	float		Range;
	float		ShadowFactor;
	float		morePad;
	
	unsigned int	Shadowed;
	unsigned int	ShadowMapID;
	unsigned int	Type;
	unsigned int	OnOff;
};

#endif  //  __JL_Basic_Lights_HG_20130318__