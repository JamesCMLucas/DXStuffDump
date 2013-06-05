#include "nEncoding.h"
#include <sstream>

namespace nEncoding
{
	std::string UNICODEtoASCII( std::wstring wStr )
	{
		std::stringstream ss;   
		for ( std::wstring::iterator itChar = wStr.begin(); itChar != wStr.end(); itChar++ )   
		{
			char theChar = static_cast<char>( *itChar );
			ss << theChar;   
		}   
		return ss.str();
	}

	std::wstring ASCIItoUNICODE( std::string str )
	{
		std::wstringstream wss;
		for ( std::string::iterator itChar = str.begin(); itChar != str.end(); itChar++ )   
		{
			wchar_t theChar = static_cast<wchar_t>( *itChar );
			wss << theChar;   
		}   
		return wss.str();
	}
};