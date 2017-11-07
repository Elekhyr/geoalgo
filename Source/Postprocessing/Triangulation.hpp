/**
* \file Triangulation.hpp
* \brief todo
* \author Elekhyr
* \version 1.0
* \date 07/11/17
* \copyright Copyright (c) 2017 Thomas Margier
*  This file is licensed under the MIT License, see https://opensource.org/licenses/MIT
*/
#pragma once
#include <vector>
#include "../Mesh/Vertex.hpp"
#include "../Mesh/Face.hpp"
#include <list>

namespace Ez
{
	class Triangulation
	{
	public:
		static void AddFirstFace(const std::vector<Vertex>& vertices, std::vector<Face>& faces, const Face& firstFace, std::list<std::array<unsigned, 3>>& convexHull);
		static void AddPointNaively(std::vector<Vertex>& vertices, std::vector<Face>& faces, const Vertex& vertex, std::list<std::array<unsigned, 3>>& convexHull);
	private:

		/**
		 * \brief Returns true if the three vertices are in trigonometric order
		 */
		static bool IsTrigo(const Vertex& v0, const Vertex& v1, const Vertex& v2);

		/**
		* \brief Returns true if the three vertices are in trigonometric order
		*/
		static bool IsTrigo(const std::vector<Vertex>& vertices, const Face& face);

		/**
		 * \brief Find the face where the vertex lies on.
		 */
		static unsigned FindFace(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);

		/**
		 * \brief Split the face in 3 faces
		 * \details The 3 triangles made from the splitted one have the new vertex at the position 2.
		 */
		static void SplitFace(std::vector<Vertex>& vertices, std::vector<Face>& faces, const unsigned pos, std::list<std::array<unsigned, 3>>& convexHull);
	};
}

