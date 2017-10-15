#include "stdafx.h"
#include "constant.h"
#include "Automaton/Automaton.h"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < ARG_COUNT)
	{
		std::cerr << "Ошибка: не указан файл ввода автомата\n";
		return 1;
	}
	string commandToViewDiagrams;
	if (argc > ARG_COUNT)
	{
		commandToViewDiagrams = argv[ARG_COUNT];
	}
	try
	{
		CAutomaton automaton(argv[1], cout);
		automaton.CreateDiagram(diagramBeforeFileName);
		while (automaton.MinimizeAutomatonStep(diagramAfterFileName))
		{
		}
		automaton.PrintEqualClassesToStatesVectorMap();
	}
	catch (invalid_argument & exception)
	{
		cerr << "Неверный параметр: " << exception.what() << "\n";
		return 1;
	}
	catch (runtime_error & exception)
	{
		cerr << "Ошибка выполнения: " << exception.what() << "\n";
		return 1;
	}

	if (!commandToViewDiagrams.empty())
	{
		size_t argumentsCount = argc - ARG_COUNT;
		char *arguments[argumentsCount + 2];
		for (size_t i = 0; i < argumentsCount; ++i)
		{
			arguments[i] = argv[i + ARG_COUNT];
		}
		arguments[argumentsCount] = (char*)diagramAfterFileName.c_str();
		arguments[argumentsCount + 1] = nullptr;
		pid_t pid = fork();
		if (pid == 0)
		{
			if (execvp(commandToViewDiagrams.c_str(), arguments) == -1)
			{
				cerr << "Ошибка запуска команды для отображения диаграммы конечного автомата, она доступны по следующему относительному путю: \n"
				     << diagramAfterFileName << "\n";
			}
		}
		else if (pid > 0)
		{
			arguments[argumentsCount] = (char*)diagramBeforeFileName.c_str();
			if (execvp(commandToViewDiagrams.c_str(), arguments) == -1)
			{
				cerr << "Ошибка запуска команды для отображения диаграммы исходного автомата, она доступны по следующему относительному путю: \n"
				     << diagramBeforeFileName << "\n";
			}
		}
	}
	else
	{
		cout << "Не указан параметр-команда для отображения диаграмм, они доступны по следующим относительным путям: \n"
		     << diagramBeforeFileName << "\n" << diagramAfterFileName << "\n";
	}
	return 0;
}
