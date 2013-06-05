#ifndef __JL_nEncoding_HG_20130317__
#define __JL_nEncoding_HG_20130317__

#include <string>

namespace nEncoding
{
	std::string UNICODEtoASCII( std::wstring wStr );
	std::wstring ASCIItoUNICODE( std::string str );
};

#endif  //  __JL_nEncoding_HG_20130317__