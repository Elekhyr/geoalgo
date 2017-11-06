/**
* \file Importer.hpp
* \brief todo
* \author Elekhyr
* \version 1.0
* \date 06/11/17
* \copyright Copyright (c) 2017 Thomas Margier
*  This file is licensed under the MIT License, see https://opensource.org/licenses/MIT
*/
#pragma once
#include <string>
#include "../Mesh/Mesh.hpp"
#include "../Postprocessing/Postprocessing.hpp"

namespace Ez
 {
	 class Importer
	 {
	 public:
		 Importer() = default;
		 virtual ~Importer() = default;
		 virtual Mesh ReadFromFile(const std::string& path, Postprocessing pp = Postprocessing::None);
	 };
 }

