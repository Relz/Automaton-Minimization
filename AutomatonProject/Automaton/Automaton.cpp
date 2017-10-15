#include "Automaton.h"

CAutomaton::CAutomaton(const std::string & inputFileName, std::ostream & outputStream)
		: _input(inputFileName)
		, _output(outputStream)
{
	_automatonType = DetermineAutomatonType(inputFileName);
	if (_automatonType == None)
	{
		throw std::invalid_argument("Не удалось обнаружить автомат");
	}
	if (_automatonType == Moore)
	{
		_output.PrintLine("Обнаружен автомат Мура");
		_output.Print("Чтение...");
		ReadAutomatonMoore();
	}
	else if (_automatonType == Mealy)
	{
		_output.PrintLine("Обнаружен автомат Мили");
		_output.Print("Чтение...");
		ReadAutomatonMealy();
	}
	if (_equalClassesToStatesVectors.empty())
	{
		throw std::invalid_argument("Автомат пуст");
	}
	else
	{
		_output.PrintLine("ОК");
	}
}

bool CAutomaton::MinimizeAutomatonStep()
{
	static size_t stepCount = 0;
	++stepCount;
	_output.PrintLine("Шаг: ", stepCount);
	RefreshStatesToTransitionsMap();
	if (!RefreshStatesEqualClassesMaps())
	{
		_output.PrintLine("Автомат минимизирован");
		_minimized = true;
		return false;
	}
	return true;
}

bool CAutomaton::MinimizeAutomatonStep(const std::string & diagramFileName)
{
	bool result = MinimizeAutomatonStep();
	if (!result)
	{
		_output.PrintLine("Подготовка результатов");
		CreateResults();
		_output.PrintLine("Проверка исходного и минимизированного автомата на эквивалентность");
		if (AutomatonsAreEquals())
		{
			_output.PrintLine("Исходный и минимизированный автомат эквивалентны");
		}
		else
		{
			_output.PrintLine("Исходный и минимизированный автомат не эквивалентны");
			return false;
		}
		CreateDiagram(diagramFileName);
	}
	return result;
}

void CAutomaton::CreateDiagram(const std::string & diagramFileName)
{
	if (_minimized)
	{
		_output.PrintLine("Генерация диаграммы минимизированного автомата в файл: " + diagramFileName);
		CreateDiagram(diagramFileName, _finalStatesToTransitions);
	}
	else
	{
		_output.PrintLine("Генерация диаграммы исходного автомата в файл: " + diagramFileName);
		CreateDiagram(diagramFileName, _statesToTransitions);
	}
}

AutomatonType CAutomaton::DetermineAutomatonType(const std::string &inputFilename)
{
	std::vector<size_t> firstLine;
	std::vector<size_t> secondLine;
	try
	{
		CInput input(inputFilename);
		if (!input.ReadVector(firstLine))
		{
			throw std::invalid_argument("");
		}
		input.SkipLine();
		input.ReadVector(secondLine);
		return (firstLine.size() == secondLine.size()) ? Moore : Mealy;
	}
	catch (std::invalid_argument & exception)
	{
		return None;
	}
}

void CAutomaton::ReadAutomatonMoore()
{
	std::vector<size_t> statesOrder;
	std::vector<size_t> outputSignals;
	_input.ReadVector(outputSignals);
	_input.SkipLine();
	{
		size_t i = 0;
		size_t state;
		while (_input.ReadArguments(state))
		{
			_equalClassesToStatesVectors[outputSignals[i]].emplace_back(state);
			_statesToEqualClasses[state] = outputSignals[i];
			statesOrder.emplace_back(state);
			++i;
		}
	}
	_input.SkipLine();
	{
		size_t inputSignal;
		while (_input.ReadArguments(inputSignal))
		{
			size_t i = 0;
			for (size_t state : statesOrder)
			{
				size_t newState;
				_input.ReadArguments(newState);
				_sourceStatesToTransitions[state][inputSignal] = newState;
				_statesToTransitions[state][inputSignal] = newState;
				_statesToInputSignalsToOutputSignals[state][inputSignal] = outputSignals[i];
				++i;
			}
			_input.SkipLine();
		}
	}
	_startState = statesOrder.front();
}

void CAutomaton::ReadAutomatonMealy()
{
	std::vector<size_t> statesOrder;
	_input.ReadVector(statesOrder);
	_input.SkipLine();
	size_t inputSignal;
	while (_input.ReadArguments(inputSignal))
	{
		for (size_t state : statesOrder)
		{
			size_t newState;
			size_t outputSignal;
			_input.ReadArguments(newState);
			_input.SkipSymbols({'/'});
			_input.ReadArguments(outputSignal);
			_sourceStatesToTransitions[state][inputSignal] = newState;
			_statesToTransitions[state][inputSignal] = newState;
			_statesToInputSignalsToOutputSignals[state][inputSignal] = outputSignal;
		}
		_input.SkipLine();
	}
	std::unordered_map<size_t, std::vector<size_t>> newEqualClassStates;
	for (auto & outputSignals : _statesToInputSignalsToOutputSignals)
	{
		newEqualClassStates[ComputeMapHash(outputSignals.second)].emplace_back(outputSignals.first);
	}
	for (auto & equalClassToStates: newEqualClassStates)
	{
		for (size_t state : equalClassToStates.second)
		{
			_statesToEqualClasses[state] = _equalClassesToStatesVectors.size() + 1;
		}
		_equalClassesToStatesVectors[_equalClassesToStatesVectors.size() + 1] = std::move(equalClassToStates.second);
	}
	_startState = statesOrder.front();
}

void CAutomaton::RefreshStatesToTransitionsMap()
{
	for (auto & statesTransition : _statesToTransitions)
	{
		for (auto & transition : statesTransition.second)
		{
			transition.second = _statesToEqualClasses.at(_sourceStatesToTransitions.at(statesTransition.first).at(transition.first));
		}
	}
}

bool CAutomaton::RefreshStatesEqualClassesMaps()
{
	bool result = false;
	size_t indexToStop = _equalClassesToStatesVectors.size();
	size_t index = 0;
	std::vector<std::unordered_map<size_t, std::vector<size_t>>> newEqualClassStatesVector(indexToStop);
	for (auto & equalClassToStatesElement : _equalClassesToStatesVectors)
	{
		for (size_t state : equalClassToStatesElement.second)
		{
			newEqualClassStatesVector[index][ComputeMapHash(_statesToTransitions.at(state))].emplace_back(state);
		}
		if (newEqualClassStatesVector[index].size() != 1)
		{
			result = true;
		}

		++index;
		if (index == indexToStop)
		{
			break;
		}
	}
	_equalClassesToStatesVectors.clear();
	for (auto & newEqualClassStates : newEqualClassStatesVector)
	{
		for (auto & newEqualClassStatesElement : newEqualClassStates)
		{
			for (size_t state : newEqualClassStatesElement.second)
			{
				_statesToEqualClasses[state] = _equalClassesToStatesVectors.size() + 1;
			}
			_equalClassesToStatesVectors[_equalClassesToStatesVectors.size() + 1] = std::move(newEqualClassStatesElement.second);
		}
	}
	return result;
}

bool CAutomaton::AutomatonsAreEquals()
{
	for (auto & equalClassToStatesVector : _equalClassesToStatesVectors)
	{
		std::unordered_set<size_t> processed;
		std::queue<std::pair<size_t, size_t>> queue;
		queue.emplace(equalClassToStatesVector.first, _equalClassesToStatesVectors.at(equalClassToStatesVector.first).at(0));
		while (!queue.empty())
		{
			size_t equalClass = queue.front().first;
			size_t state = queue.front().second;
			queue.pop();
			if (processed.find(equalClass) == processed.end())
			{
				continue;
			}
			if (_finalStatesToTransitions.at(equalClass).size() != _sourceStatesToTransitions.at(state).size())
			{
				return false;
			}
			std::unordered_map<size_t, size_t> expected;
			for (auto & sourceStateToTransitions : _sourceStatesToTransitions.at(state))
			{
				expected.emplace(sourceStateToTransitions.first, _statesToInputSignalsToOutputSignals.at(state).at(sourceStateToTransitions.first));
			}
			for (auto & finalStateToTransitions : _finalStatesToTransitions.at(equalClass))
			{
				if (expected.at(finalStateToTransitions.first) != _statesToInputSignalsToOutputSignals.at(_equalClassesToStatesVectors.at(equalClass).at(0)).at(finalStateToTransitions.first))
				{
					_output.PrintLine(expected.at(finalStateToTransitions.first), " != ", finalStateToTransitions.second);
					return false;
				}
				processed.insert(equalClass);
				queue.emplace(finalStateToTransitions.second, _sourceStatesToTransitions.at(state).at(expected.at(finalStateToTransitions.first)));
			}
		}
	}
	return true;
}

void CAutomaton::CreateResults()
{
	for (auto & equalClassToStatesVector : _equalClassesToStatesVectors)
	{
		size_t finalEqualClass = equalClassToStatesVector.first;
		if (_statesToEqualClasses.at(_startState) == finalEqualClass)
		{
			_startState = finalEqualClass;
		}
		size_t firstEqualClassState = equalClassToStatesVector.second[0];
		for (auto & transition : _statesToTransitions[firstEqualClassState])
		{
			_finalStatesToTransitions[finalEqualClass][transition.first] = _statesToEqualClasses.at(_sourceStatesToTransitions.at(firstEqualClassState).at(transition.first));
		}
	}
}

void CAutomaton::CreateDiagram(
		const std::string & diagramFileName,
		const std::unordered_map<size_t, std::unordered_map<size_t, size_t>> & transitionMap)
{
	using Graph = boost::adjacency_list<
			boost::vecS,
			boost::vecS,
			boost::directedS,
			boost::property<boost::vertex_name_t, std::string>,
			boost::property<boost::edge_name_t, std::string>
	>;

	using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = boost::graph_traits<Graph>::edge_descriptor;

	using VerticesName = boost::property_map<Graph, boost::vertex_name_t>::type;
	using EdgesName = boost::property_map<Graph, boost::edge_name_t>::type;

	Graph graph;
	VerticesName verticesName = boost::get(boost::vertex_name, graph);
	EdgesName edgesName = boost::get(boost::edge_name, graph);

	std::unordered_map<size_t, Vertex> vertices;
	for (auto & transitions : transitionMap)
	{
		vertices[transitions.first] = boost::add_vertex(graph);
	}
	for (auto & vertex : vertices)
	{
		verticesName[vertex.second] = std::to_string(vertex.first);
		size_t outputSignal = 0;
		for (auto & transition : transitionMap.at(vertex.first))
		{
			Edge edge;
			bool inserted;
			std::tie(edge, inserted) = boost::add_edge(vertex.second, vertices.at(transition.second), graph);
			if (!inserted)
			{
				throw std::invalid_argument(
						"Не удалось проложить дугу между вершинами: "
						+ std::to_string(vertex.first)
						+ " и "
						+ std::to_string(transition.second)
				);
			}
			edgesName[edge] = std::to_string(transition.first);
			if (_minimized)
			{
				outputSignal = _statesToInputSignalsToOutputSignals.at(_equalClassesToStatesVectors.at(vertex.first)[0]).at(transition.first);
			}
			else
			{
				outputSignal = _statesToInputSignalsToOutputSignals.at(vertex.first).at(transition.first);
			}
			if (_automatonType == Mealy)
			{
				edgesName[edge] += "/" + std::to_string(outputSignal);
			}
		}
		if (_automatonType == Moore)
		{
			verticesName[vertex.second] += "/" + std::to_string(outputSignal);
		}
	}
	boost::dynamic_properties dp;
	dp.property("node_id", verticesName);
	dp.property("label", edgesName);
	std::ofstream ofs("tmp.dot");
	boost::write_graphviz_dp(ofs, graph, dp);
	ofs.close();
	if (system(("dot -Tpng -o " + diagramFileName + " tmp.dot").c_str()) != 0)
	{
		throw std::runtime_error("Не удалось создать диаграмму. Возможно, программа \"dot\" не установлена");
	}
	if (remove("tmp.dot") == -1)
	{
		throw std::runtime_error("Не удалось удалить мусор за собой (файл \"tmp.dot\")");
	}
}

void CAutomaton::PrintStatesToEqualClassesMap() const
{
	PrintMap(_statesToEqualClasses);
}
//
void CAutomaton::PrintEqualClassesToStatesVectorMap() const
{
	PrintMapContainer(_equalClassesToStatesVectors);
}

void CAutomaton::PrintSourceStatesToTransitionsMap() const
{
	PrintMapMap(_sourceStatesToTransitions);
}

void CAutomaton::PrintStatesToTransitionsMap() const
{
	PrintMapMap(_statesToTransitions);
}

void CAutomaton::PrintFinalStatesToTransitionsMap() const
{
	PrintMapMap(_finalStatesToTransitions);
}

void CAutomaton::PrintStatesToInputSignalsToOutputSignalsMap() const
{
	PrintMapMap(_statesToInputSignalsToOutputSignals);
}

void CAutomaton::PrintMap(const std::unordered_map<size_t, size_t> & map) const
{
	_output.PrintMap(
			map,
			" ",
			{
					COutput::PrintContainerMethod::NORMAL,
					"\n"
			}
	);
}

void CAutomaton::PrintMapContainer(const std::unordered_map<size_t, std::vector<size_t>> & map) const
{
	_output.PrintMapContainer(
			map,
			": ",
			{
					COutput::PrintContainerMethod::NORMAL,
					"\n"
			},
			{
					COutput::PrintContainerMethod::NORMAL,
					" "
			}
	);
}

void CAutomaton::PrintMapMap(const std::unordered_map<size_t, std::unordered_map<size_t, size_t>> & map) const
{
	_output.PrintMapMap(
			map,
			" ",
			{
					COutput::PrintContainerMethod::NORMAL,
					"\n"
			},
			{
					COutput::PrintContainerMethod::NORMAL,
					"\n"
			}
	);
}

size_t CAutomaton::ComputeMapHash(std::unordered_map<size_t, size_t> &map)
{
	return boost::hash_range(map.begin(), map.end());
}
