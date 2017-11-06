/**
* \file Mesh.hpp
* \brief todo
* \author Elekhyr
* \version 1.0
* \date 06/11/17
* \copyright Copyright (c) 2017 Thomas Margier
*  This file is licensed under the MIT License, see https://opensource.org/licenses/MIT
*/
#pragma once
#include <vector>
#include "Vertex.hpp"
#include "Face.hpp"

namespace Ez
{
	class Mesh
	{
		class IncidentFaceCirculator;
	public:
		Mesh();
		Mesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);

	private:
		std::vector<Vertex> mVertices;
		std::vector<Face> mFaces;
	};
}

