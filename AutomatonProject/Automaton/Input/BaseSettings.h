#ifndef PROJECT_BASESETTINGS_H
#define PROJECT_BASESETTINGS_H

#include <vector>
#include "ReadVectorMethod.h"
#include "ReadLimit.h"

class BaseSettings
{
public:
	std::vector<wchar_t> skipSymbols;
	ReadVectorMethod readMethod;
	size_t readLimit;

	BaseSettings()
			: skipSymbols({})
			, readMethod(ReadVectorMethod::PUSH_BACK)
			, readLimit(ReadLimit::UNLIMITED)
	{}

	BaseSettings(
			const std::vector<wchar_t> & skipSymbols,
			ReadVectorMethod readMethod,
			size_t readLimit
	)
			: skipSymbols(skipSymbols)
			, readMethod(readMethod)
			, readLimit(readLimit)
	{}
};

#endif //PROJECT_BASESETTINGS_H
