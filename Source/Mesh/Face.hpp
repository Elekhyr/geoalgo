/**
* \file Face.hpp
* \brief todo
* \author Elekhyr
* \version 1.0
* \date 06/11/17
* \copyright Copyright (c) 2017 Thomas Margier
*  This file is licensed under the MIT License, see https://opensource.org/licenses/MIT
*/
#pragma once
#include <array>

namespace Ez
{
	struct Face
	{
		std::array<unsigned, 3> vertices; /// indices of the vertices
		std::array<unsigned, 3> adjacentFaces; /// indices of the adjacent adjacentFaces
	
		size_t Size() const; /// Returns the number of vertices of the face
		unsigned Next(unsigned j) const; /// Returns the next vertex index in a trigonometric order
		unsigned Previous(unsigned j) const; /// Returns the previous vertex index in a trigonometric order
	};
}

