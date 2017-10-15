#ifndef AUTOMATON_AUTOMATON_H
#define AUTOMATON_AUTOMATON_H

#include "Input/Input.h"
#include "Output/Output.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <boost/functional/hash.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

enum AutomatonType
{
	None,
	Moore,
	Mealy
};

class CAutomaton
{
public:
	explicit CAutomaton(const std::string & inputFileName, std::ostream & outputStream = std::cout);

/* Возвращает true, если связи <Состояние - Класс эквивалентности> изменились. */
	bool MinimizeAutomatonStep();
	bool MinimizeAutomatonStep(const std::string & diagramFileName);

	void CreateDiagram(const std::string & diagramFileName);

	void PrintStatesToEqualClassesMap() const;
	void PrintEqualClassesToStatesVectorMap() const;
	void PrintSourceStatesToTransitionsMap() const;
	void PrintStatesToTransitionsMap() const;
	void PrintFinalStatesToTransitionsMap() const;
	void PrintStatesToInputSignalsToOutputSignalsMap() const;

private:
	AutomatonType DetermineAutomatonType(const std::string & inputFilename);
	void ReadAutomatonMoore();
	void ReadAutomatonMealy();

	void RefreshStatesToTransitionsMap();
	bool RefreshStatesEqualClassesMaps();

	bool AutomatonsAreEquals();
	void CreateResults();
	void CreateDiagram(
			const std::string & diagramFileName,
			const std::unordered_map<size_t, std::unordered_map<size_t, size_t>> & transitionMap
	);

	void PrintMap(const std::unordered_map<size_t, size_t> & map) const;
	void PrintMapContainer(const std::unordered_map<size_t, std::vector<size_t>> & map) const;
	void PrintMapMap(const std::unordered_map<size_t, std::unordered_map<size_t, size_t>> & map) const;

	CInput _input;
	COutput _output;
	std::unordered_map<size_t, size_t> _statesToEqualClasses;
	std::unordered_map<size_t, std::vector<size_t>> _equalClassesToStatesVectors;
	std::unordered_map<size_t, std::unordered_map<size_t, size_t>> _sourceStatesToTransitions;
	std::unordered_map<size_t, std::unordered_map<size_t, size_t>> _statesToTransitions;
	std::unordered_map<size_t, std::unordered_map<size_t, size_t>> _finalStatesToTransitions;
	std::unordered_map<size_t, std::unordered_map<size_t, size_t>> _statesToInputSignalsToOutputSignals;

	bool _minimized = false;
	AutomatonType _automatonType;
	size_t _startState = SIZE_MAX;

	static size_t ComputeMapHash(std::unordered_map<size_t, size_t> &map);
};

#endif //AUTOMATON_AUTOMATON_H
