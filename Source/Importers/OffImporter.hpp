/**
* \file OffImporter.hpp
* \brief todo
* \author Elekhyr
* \version 1.0
* \date 06/11/17
* \copyright Copyright (c) 2017 Thomas Margier
*  This file is licensed under the MIT License, see https://opensource.org/licenses/MIT
*/
#pragma once
#include <string>
#include "Importer.hpp"
#include "../Mesh/Mesh.hpp"

namespace Ez
{
	class OffImporter
		: public Importer
	{
	public:
		Mesh ReadFromFile(const std::string& path, Postprocessing pp = Postprocessing::None) override;
	private:
		static Mesh LoadWithNoPostProcessing(std::stringstream& stream, unsigned nbVertices, unsigned nbFaces);
		static Mesh LoadWithNaiveTriangulation(std::stringstream& stream, const unsigned nbVertices, const unsigned nbFaces);
	};
}

