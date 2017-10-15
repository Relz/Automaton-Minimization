#include <map>
#include <fstream>
#include <iostream>
#include <iterator>
#include <utility>

class COutput
{
public:
	enum PrintContainerMethod
	{
		NORMAL,
		REVERSE
	};

	struct Settings
	{
		PrintContainerMethod printContainerMethod;
		std::string delimiter;

		Settings()
				: printContainerMethod(PrintContainerMethod::NORMAL)
				, delimiter("")
		{}
		Settings(PrintContainerMethod printContainerMethod, std::string delimiter)
				: printContainerMethod(printContainerMethod)
				, delimiter(std::move(delimiter))
		{}
	};

	explicit COutput(std::ostream & wout)
			: m_out(wout)
	{}

	explicit COutput(const std::string & outputFileName)
	{
		m_outputFile.open(outputFileName);
	}

	template<typename... Targs>
	void Print(Targs ... args) const
	{
		PrintSimpleTypeVariable(args...);
	}

	template<typename... Targs>
	void PrintLine(Targs ... args) const
	{
		Print(args...);
		PrintNewLine();
	}

	template<typename valueT, template<typename ... > typename ContainerT>
	void PrintContainer(const ContainerT<valueT, std::allocator<valueT>> & container, const Settings & settings = Settings()) const
	{
		if (settings.printContainerMethod == PrintContainerMethod::NORMAL)
		{
			copy(container.begin(), container.end(), std::ostream_iterator<valueT, char>(m_out, settings.delimiter.c_str()));
		}
		else if (settings.printContainerMethod == PrintContainerMethod::REVERSE)
		{
			copy(container.rbegin(), container.rend(), std::ostream_iterator<valueT, char>(m_out, settings.delimiter.c_str()));
		}
	}

	template<typename TKey, typename TValue>
	void PrintMap(const std::unordered_map<TKey, TValue> & unorderedMap, const std::string keyValueDelimiter, const Settings & settings = Settings()) const
	{
		for (auto pair : unorderedMap)
		{
			Print(pair.first, keyValueDelimiter, pair.second, settings.delimiter);
		}
	}

	template<typename TKey, typename TValue, template<typename ... > typename TContainer>
	void PrintMapContainer(const std::unordered_map<TKey, TContainer<TValue, std::allocator<TValue>>> & unorderedMap, const std::string keyValueDelimiter, const Settings & mapSettings = Settings(), const Settings & containerSettings = Settings()) const
	{
		for (auto pair : unorderedMap)
		{
			Print(pair.first, keyValueDelimiter);
			PrintContainer(pair.second, containerSettings);
			Print(mapSettings.delimiter);
		}
	}

	template<typename TKey0, typename TKey1, typename TValue>
	void PrintMapMap(const std::unordered_map<TKey0, std::unordered_map<TKey1, TValue>> & unorderedMap, const std::string keyValueDelimiter, const Settings & mapSettings = Settings(), const Settings & innerMapSettings = Settings()) const
	{
		for (auto pair : unorderedMap)
		{
			Print(pair.first, keyValueDelimiter);
			PrintMap(pair.second, keyValueDelimiter, innerMapSettings);
			Print(mapSettings.delimiter);
		}
	}

private:
	void PrintSimpleTypeVariable() const {return;}

	template<typename T, typename... Targs>
	void PrintSimpleTypeVariable(T arg, Targs ... args) const
	{
		m_out << arg;
		Print(args...);
	}

	void PrintNewLine() const
	{
		m_out << '\n';
	}

	std::ofstream m_outputFile;
	std::ostream & m_out = m_outputFile;
};
