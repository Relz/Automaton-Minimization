#ifndef PROJECT_VECTORSETTINGS_H
#define PROJECT_VECTORSETTINGS_H

#include <unordered_map>
#include <recodext.h>
#include "BaseSettings.h"

template <typename T>
class VectorSettings: public BaseSettings
{
public:
	wchar_t trueChar;
	std::unordered_map<wchar_t, T> rules;

	VectorSettings()
			: trueChar(NOT_A_CHARACTER)
			, rules(std::unordered_map<wchar_t, T>())
	{}

	VectorSettings(
			const std::vector<wchar_t> & skipSymbols,
			ReadVectorMethod readVectorMethod,
			size_t readLimit,
			wchar_t trueChar,
			const std::unordered_map<wchar_t, T> & rules
	)
			: BaseSettings(skipSymbols, readVectorMethod, readLimit)
			, trueChar(trueChar)
			, rules(rules)
	{}
};

#endif //PROJECT_VECTORSETTINGS_H
