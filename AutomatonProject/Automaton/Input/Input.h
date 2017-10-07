#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <limits>
#include <functional>
#include "ReadVectorMethod.h"
#include "ReadLimit.h"
#include "BaseSettings.h"
#include "VectorSettings.h"
#include "MatrixSettings.h"

class CInput
{
public:
	static const wchar_t ENDL_SYMBOL = '\n';

	explicit CInput(std::wistream & wis)
			: m_wis(wis)
	{
		if (IsStreamEmpty())
		{
			throw std::invalid_argument("Поток пуст");
		}
	}

	explicit CInput(const std::string & inputFileName)
	{
		m_inputFile.open(inputFileName);
		if (!m_inputFile.good())
		{
			throw std::invalid_argument("Файл \"" + inputFileName + "\" не существует");
		}
		if (IsStreamEmpty())
		{
			throw std::invalid_argument("Файл \"" + inputFileName + "\" пуст");
		}
	}

	bool SkipLine()
	{
		m_wis.ignore(std::numeric_limits<std::streamsize>::max(), ENDL_SYMBOL);
		return !IsStreamEmpty();
	}

	bool SkipLines(size_t count)
	{
		while (count != 0)
		{
			if (!SkipLine())
			{
				return false;
			}
			--count;
		}
		return true;
	}

	void SkipSymbols(const std::vector<wchar_t> & symbols)
	{
		bool symbolSkipped = true;
		while(symbolSkipped)
		{
			symbolSkipped = false;
			for (wchar_t symbol : symbols)
			{
				if (m_wis.peek() == symbol)
				{
					m_wis.get();
					symbolSkipped = true;
				}
			}
		}
	}

	template<typename... Targs>
	bool ReadArguments(Targs & ... args)
	{
		return GetArgumentsFromStream(args...);
	}

	template<typename T>
	bool ReadVector(std::vector<T> & vect, const VectorSettings<T> & settings = VectorSettings<T>())
	{
		ReadVectorBase<T, T>(vect, settings);
	}

	bool ReadVector(std::vector<bool> & vect, const VectorSettings<bool> & settings = VectorSettings<bool>())
	{
		ReadVectorBase<bool, wchar_t>(vect, settings);
	}

	template<typename T>
	bool ReadMatrix(std::vector<std::vector<T>> & matrix, const MatrixSettings & matrixSettings = MatrixSettings(), const VectorSettings<T> & vectorSettings = VectorSettings<T>())
	{
		bool enoughRows = true;
		for (size_t i = 0; i < matrixSettings.readLimit; ++i)
		{
			SkipSymbols(matrixSettings.skipSymbols);
			BasePush(matrix, std::vector<T>(), matrixSettings.readMethod);
			size_t activeContainerIndex = GetActiveContainerIndex(matrix, matrixSettings.readMethod);
			if (!ReadVector(matrix[activeContainerIndex], vectorSettings))
			{
				if (matrix[activeContainerIndex].empty())
				{
					VectorPop(matrix, matrixSettings.readMethod);
				}
				enoughRows = false;
				break;
			}
			SkipLine();
		}
		return !matrix.empty() && (enoughRows || matrixSettings.readLimit == ReadLimit::UNLIMITED);
	}

private:
	bool IsStreamEmpty()
	{
		return m_wis.peek() == std::wifstream::traits_type::eof();
	}

	template<class T>
	bool GetArgumentFromStream(T & arg)
	{
		return (m_wis.peek() != ENDL_SYMBOL && !m_wis.eof() && m_wis >> arg);
	}

	bool GetArgumentFromStream(wchar_t & arg)
	{
		return (m_wis.peek() != ENDL_SYMBOL && !m_wis.eof() && m_wis.get(arg));
	}

	bool GetArgumentsFromStream() { return true; }

	template<typename T, typename... Targs>
	bool GetArgumentsFromStream(T & arg, Targs & ... args)
	{
		return GetArgumentFromStream(arg) && GetArgumentsFromStream(args...);
	}

	template<typename T>
	void BasePush(std::vector<T> & vect, const T & elem, const ReadVectorMethod readVectorMethod)
	{
		if (readVectorMethod == PUSH_BACK)
		{
			vect.emplace_back(elem);
		}
		else if (readVectorMethod == PUSH_FRONT)
		{
			vect.insert(vect.begin(), elem);
		}
	}

	template<typename T>
	bool VectorPush(std::vector<T> & vect, const T & elem, const VectorSettings<T> & settings = VectorSettings<T>())
	{
		T elemToPush = elem;
		if (!settings.rules.empty())
		{
			if (settings.rules.find(elem) == settings.rules.end())
			{
				return false;
			}
			elemToPush = settings.rules.at(elem);
		}
		BasePush(vect, elemToPush, settings.readMethod);
		return true;
	}

	bool VectorPush(std::vector<bool> & vect, const wchar_t & elem, const VectorSettings<bool> & settings = VectorSettings<bool>())
	{
		if (settings.trueChar == NOT_A_CHARACTER && settings.rules.empty())
		{
			throw(std::invalid_argument("True char and rules are not specified"));
		}
		bool elemToPush;
		if (settings.trueChar != NOT_A_CHARACTER)
		{
			elemToPush = elem == settings.trueChar;
		}
		else
		{
			if (settings.rules.find(elem) == settings.rules.end())
			{
				return false;
			}
			elemToPush = settings.rules.at(elem);
		}
		BasePush(vect, elemToPush, settings.readMethod);
		return true;
	}

	template<typename T>
	void VectorPop(std::vector<T> & vect, ReadVectorMethod readVectorMethod)
	{
		if (readVectorMethod == PUSH_BACK)
		{
			vect.pop_back();
		}
		else if (readVectorMethod == PUSH_FRONT)
		{
			vect.erase(vect.begin());
		}
	}

	template <typename T>
	size_t GetActiveContainerIndex(std::vector<T> & vect, ReadVectorMethod readVectorMethod)
	{
		if (readVectorMethod == PUSH_BACK)
		{
			return vect.size() - 1;
		}
		else if (readVectorMethod == PUSH_FRONT)
		{
			return 0;
		}
	}

	template<typename TVectorElement, typename TReadElement>
	bool ReadVectorBase(std::vector<TVectorElement> & vect, const VectorSettings<TVectorElement> & settings)
	{
		bool result = false;
		TReadElement elem;
		while (vect.size() != settings.readLimit && GetArgumentFromStream(elem))
		{
			if (!VectorPush(vect, elem, settings))
			{
				result = false;
				break;
			}
			SkipSymbols(settings.skipSymbols);
			result = true;
		}
		return result && (vect.size() == settings.readLimit || settings.readLimit == ReadLimit::UNLIMITED);
	}

	std::wifstream m_inputFile;
	std::wistream & m_wis = m_inputFile;
};
