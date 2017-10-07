#include "stdafx.h"
#include "constant.h"
#include "Automaton/Automaton.h"
#include "SfmlSprite.h"

using namespace std;
using namespace sf;

void Update(RenderWindow & window, SfmlSprite & diagramBefore, SfmlSprite & diagramAfter)
{
	window.clear(Color::White);
	window.draw(diagramBefore.sprite);
	window.draw(diagramAfter.sprite);
	window.display();
}

void HandleEvents(RenderWindow & window)
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed)
			window.close();
	}
}

int main(int argc, char *argv[])
{
	if (argc != ARG_COUNT)
	{
		std::cerr << "Ошибка: не указан файл ввода автомата\n";
	}
	const string diagramBeforeFileName = "diagram_before.png";
	const string diagramAfterFileName = "diagram_after.png";
	try
	{
		CAutomaton automaton("input_moore.csv", cout);
		automaton.CreateDiagram(diagramBeforeFileName);
		while (automaton.MinimizeAutomatonStep(diagramAfterFileName))
		{
		}
	}
	catch (invalid_argument & exception)
	{
		cerr << "Неверный параметр: " << exception.what();
	}
	catch (runtime_error & exception)
	{
		cerr << "Ошибка выполнения: " << exception.what();
	}

	ContextSettings settings;
	settings.antialiasingLevel = ANTIALIASING_LEVEL;

	SfmlSprite diagramBefore(diagramBeforeFileName);
	SfmlSprite diagramAfter(diagramAfterFileName);

	RenderWindow window(
			VideoMode(SCREEN_SIZE.x, SCREEN_SIZE.y),
			L"Минимизация автомата: До и После",
			Style::Close,
			settings
	);
	Vector2f windowCenter = Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
	diagramBefore.sprite.setPosition(
			(windowCenter.x - diagramBefore.GetWidth()) / 2,
			windowCenter.y - diagramBefore.GetHeight() / 2
	);
	diagramAfter.sprite.setPosition(
			windowCenter.x + (windowCenter.x - diagramBefore.GetWidth()) / 2,
			windowCenter.y - diagramAfter.GetHeight() / 2
	);
	window.setFramerateLimit(60);
	while (window.isOpen())
	{
		HandleEvents(window);
		Update(window, diagramBefore, diagramAfter);
	}
	return 0;
}
