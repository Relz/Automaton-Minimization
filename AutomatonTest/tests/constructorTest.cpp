#include "gtest/gtest.h"
#include "../../AutomatonProject/Automaton/Automaton.h"

TEST(constructor, throws_exception_if_input_file_not_found)
{
	EXPECT_THROW(CAutomaton(""), std::invalid_argument);
}

TEST(constructor, throws_exception_if_input_file_is_empty)
{
	EXPECT_THROW(CAutomaton automaton("invalid_input"), std::invalid_argument);
}
