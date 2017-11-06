/**
* \file Vertex.hpp
* \brief todo
* \author Elekhyr
* \version 1.0
* \date 06/11/17
* \copyright Copyright (c) 2017 Thomas Margier
*  This file is licensed under the MIT License, see https://opensource.org/licenses/MIT
*/
#pragma once

#include "Math/Vec3.hpp"

namespace Ez
{
	struct Vertex
	{
		Math::Vec3d position; /// Vertex 3D position

		unsigned int incidentFace; /// Index of one incident face


	};
}

