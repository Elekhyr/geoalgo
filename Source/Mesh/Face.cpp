#include "Face.hpp"

size_t Ez::Face::Size() const
{
	return vertices.size();
}

unsigned Ez::Face::Next(const unsigned j) const
{
	return (j + 1) % vertices.size();
}

unsigned Ez::Face::Previous(unsigned j) const
{
	return (j - 1) % vertices.size();
}
