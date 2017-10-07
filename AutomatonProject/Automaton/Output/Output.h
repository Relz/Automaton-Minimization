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
