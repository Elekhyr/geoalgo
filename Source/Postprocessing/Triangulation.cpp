#include "Triangulation.hpp"
#include "../Logger.h"

void Ez::Triangulation::AddFirstFace(const std::vector<Vertex>& vertices, std::vector<Face>& faces,
	const Face& firstFace, std::list<std::array<unsigned, 3>>& convexHull)
{
#ifdef _DEBUG
	olog(Finest) << "Inserting first face";
#endif

	faces.push_back(firstFace);
	if (!IsTrigo(vertices, faces.back()))
	{
#ifdef _DEBUG
		olog(Finest) << "First face is not trigo" << vertices[firstFace.vertices[0]].position << ", " << vertices[firstFace.vertices[1]].position << ", " << vertices[firstFace.vertices[2]].position;
#endif
		faces.back().vertices[1] = 2;
		faces.back().vertices[2] = 1;
#ifdef _DEBUG
		olog(Finest) << "First is now trigo" << vertices[firstFace.vertices[0]].position << ", " << vertices[firstFace.vertices[1]].position << ", " << vertices[firstFace.vertices[2]].position;
#endif
	}

	convexHull.push_back({faces.back().vertices[0], faces.back().vertices[1], faces.size() - 1});
	convexHull.push_back({faces.back().vertices[1], faces.back().vertices[2], faces.size() - 1});
	convexHull.push_back({faces.back().vertices[2], faces.back().vertices[0], faces.size() - 1});

}

void Ez::Triangulation::AddPointNaively(std::vector<Vertex>& vertices, std::vector<Face>& faces, const Vertex& vertex, std::list<std::array<unsigned, 3>>& convexHull)
{
#ifdef _DEBUG
	olog(Finest) << "Inserting point " << vertex.position << " naively";
#endif

	vertices.push_back(vertex);

	const unsigned pos = FindFace(vertices, faces);

	if (pos == unsigned(-1))
	{
		auto it = convexHull.begin();
		std::pair<unsigned, unsigned> previous;

		while (it != convexHull.end())
		{

			if (IsTrigo(vertices[it->at(0)], vertices.back(), vertices[it->at(1)]))
			{ // The vertex is visible from the edge

				Face face;
				// Setting vertices to the edge
				face.vertices[0] = it->at(0);
				face.vertices[1] = vertices.size() - 1;
				face.vertices[2] = it->at(1);

				//setting the adjacent face of the convex hull face
				Face &adjacent_face = faces[it->at(2)];

				for (unsigned k = 0; k < adjacent_face.Size(); ++k) // for every vertices of the adjacent face
				{
					// We try to find which of its vertex is at the opposite of our face
					if (adjacent_face.vertices[k] != it->at(0)
						&& adjacent_face.vertices[k] != it->at(1))
					{
						adjacent_face.adjacentFaces[k] = faces.size();
						break;
					}
				}

				// Setting the convex hull face to the adjacent face of our face
				face.adjacentFaces[1] = it->at(2);

				// update convex hull
				// Search if an edge on the convex hull matches with the new face
				auto search = convexHull.begin();
				short r = 0;
				while (search != convexHull.end())
				{
					if (search->at(1) == face.vertices[1] && search->at(0) == face.vertices[2])
					{ // If an edge match with the left edge of the new face
						// add the face as an adjacent face
						face.adjacentFaces[0] = search->at(2);

						Face &adj_face = faces[search->at(2)];

						for (unsigned k = 0; k < adj_face.Size(); ++k) // for every vertices of the adjacent face
						{
							// We try to find which of its vertex is at the opposite of our face
							if (adj_face.vertices[k] != search->at(0)
								&& adj_face.vertices[k] != search->at(1))
							{
								adj_face.adjacentFaces[k] = faces.size();
								break;
							}
						}

						// erase the edge
						search = convexHull.erase(search);
						r += 1;
						break;
					} 
					if (search->at(1) == face.vertices[0] && search->at(0) == face.vertices[1]) 
					{// If an edge match with the right edge of the new face
					 // add the face as an adjacent face
						face.adjacentFaces[2] = search->at(2);

						Face &adj_face = faces[search->at(2)];

						for (unsigned k = 0; k < adj_face.Size(); ++k) // for every vertices of the adjacent face
						{
							// We try to find which of its vertex is at the opposite of our face
							if (adj_face.vertices[k] != search->at(0)
								&& adj_face.vertices[k] != search->at(1))
							{
								adj_face.adjacentFaces[k] = faces.size();
								break;
							}
						}

						// erase the edge
						search = convexHull.erase(search);
						r += 2;
						break;
					}
					++search;
				}
				if (r == 0)
				{
					convexHull.push_back({ face.vertices[0], face.vertices[1], faces.size() });
					convexHull.push_back({ face.vertices[1], face.vertices[2], faces.size() });
				}
				else if (r == 1)
					convexHull.push_back({ face.vertices[0], face.vertices[1], faces.size() });
				else if (r == 2)
					convexHull.push_back({ face.vertices[1], face.vertices[2], faces.size() });


				// update faces vector
				faces.push_back(std::move(face));

				// erase edge from the convex hull
				it = convexHull.erase(it);
			}
			else
				++it;
		}
		vertices.back().incidentFace = faces.size() - 1;
	}
	else
	{
		SplitFace(vertices, faces, pos, convexHull);
	}

for (auto it = convexHull.begin(); it != convexHull.end(); ++it)
{
	olog(Fine) << '[' << it->at(0) << ", " << it->at(1) << ", " << it->at(2) << "]";
}
}

bool Ez::Triangulation::isLocallyDelaunay(const std::vector<Vertex>& vertices, const std::vector<Face>& faces, Face& face,
	unsigned index)
{
	auto &p = vertices[face.vertices[index]].position;
	auto &Q = vertices[face.vertices[face.Next(index)]].position;
	auto &R = vertices[face.vertices[face.Next(face.Next(index))]].position;

	const int adj_face_pos = face.adjacentFaces[face.Next(index)];
	if (adj_face_pos == -1)
		return true;

	const auto& adj_face = faces[adj_face_pos];
	
	unsigned pos = 0;
	for (unsigned i = 0; i < adj_face.Size(); ++i)
	{
		if (adj_face.vertices[i] != face.vertices[0]
			&& adj_face.vertices[i] != face.vertices[1])
		{
			pos = i;
			break;
		}
	}

	auto &s = vertices[adj_face.vertices[pos]].position;

	if (-Dot((Q - p) * (R - p), s - p) >= 0)
	{
		return false;
	}
	
	return true;
}

void Ez::Triangulation::Flip(const std::vector<Vertex>& vector, std::vector<Face>& faces, std::array<unsigned, 3> edge)
{
}

void Ez::Triangulation::Lawson(std::vector<Vertex>& vertices, std::vector<Face>& faces)
{
	std::list<std::array<unsigned, 3>> edges_to_flip;

	// Select every non-locally Delaunay edges
	for (unsigned i = 0; i < faces.size(); ++i)
	{
		Face &face = faces[i];

		for (unsigned j = 0; j < faces[i].Size(); ++j)
		{
			if (!isLocallyDelaunay(vertices, faces, face, j))
			{
				auto it = edges_to_flip.begin();
				while (it != edges_to_flip.end())
				{
					if (it->at(0) == j && it->at(1) == face.Next(j))
						break;
					++it;
				}
				if (it == edges_to_flip.end())
					edges_to_flip.push_back({j, face.Next(j), i});
			}
		}
	}

	auto it = edges_to_flip.begin();
	// simulate queue with list
	while (!edges_to_flip.empty())
	{
		const auto edge = *it;
		Flip(vertices, faces, edge);
		it = edges_to_flip.erase(it);

		auto new_delaunay_edges_it = edges_to_flip.begin();
		while (new_delaunay_edges_it != edges_to_flip.end())
		{
			if (isLocallyDelaunay(vertices, faces, faces[new_delaunay_edges_it->at(2)], new_delaunay_edges_it->at(0)))
				new_delaunay_edges_it = edges_to_flip.erase(new_delaunay_edges_it);
			else
				++new_delaunay_edges_it;
		}

		it = edges_to_flip.begin();
	}
}

bool Ez::Triangulation::IsTrigo(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	// The cross product between the two directional vectors (v1-v0) and (v2-v1) 
	// returns a perpendicular vector pointing toward us if the vertices are in the trigonometric order and not aligned
	return ((v1.position - v0.position) * (v2.position - v1.position)).z > 0;
}

bool Ez::Triangulation::IsTrigo(const std::vector<Vertex>& vertices, const Face& face)
{
	// The cross product between the two directional vectors (v1-v0) and (v2-v1) 
	// returns a perpendicular vector pointing toward us if the vertices are in the trigonometric order
	return ((vertices[face.vertices[1]].position - vertices[face.vertices[0]].position) 
		* (vertices[face.vertices[2]].position - vertices[face.vertices[1]].position)).z > 0;
}

unsigned Ez::Triangulation::FindFace(const std::vector<Vertex>& vertices, const std::vector<Face>& faces)
{  
	// Check every face, todo: increase speed with face localization
	for (unsigned i = 0; i < faces.size(); ++i)
	{
		const Face &face = faces[i];

		if (IsTrigo(vertices[face.vertices[0]], vertices[face.vertices[1]], vertices.back())
			&& IsTrigo(vertices[face.vertices[1]], vertices[face.vertices[2]], vertices.back())
			&& IsTrigo(vertices[face.vertices[2]], vertices[face.vertices[0]], vertices.back()))
		{
			return i;
		}
	}

	return unsigned(-1);
}

void Ez::Triangulation::SplitFace(std::vector<Vertex>& vertices, std::vector<Face>& faces, const unsigned pos, std::list<std::array<unsigned, 3>>& convexHull)
{
#ifdef _DEBUG
	olog(Finest) << "Splitting face n°" << pos << " with vertices " << faces[pos].vertices[0] << ", " << faces[pos].vertices[1] << ", " << faces[pos].vertices[2]
	<< " and adjacent faces " << faces[pos].adjacentFaces[0] << ", " << faces[pos].adjacentFaces[1] << ", " << faces[pos].adjacentFaces[2];
#endif
	Face &face = faces[pos];

	Face f0;
	Face f1;
	Face f2;

	// Set the vertices to the face
	f0.vertices[0] = face.vertices[0];
	f0.vertices[1] = face.vertices[1];
	f0.vertices[2] = vertices.size() - 1;	
#ifdef _DEBUG
	olog(Finest) << "Face 0 has vertices " << f0.vertices[0] << ", " << f0.vertices[1] << ", " << f0.vertices[2];
#endif

	f1.vertices[0] = face.vertices[1];
	f1.vertices[1] = face.vertices[2];
	f1.vertices[2] = vertices.size() - 1;
#ifdef _DEBUG
	olog(Finest) << "Face 1 has vertices " << f1.vertices[0] << ", " << f1.vertices[1] << ", " << f1.vertices[2];
#endif

	f2.vertices[0] = face.vertices[2];
	f2.vertices[1] = face.vertices[0];
	f2.vertices[2] = vertices.size() - 1;
#ifdef _DEBUG
	olog(Finest) << "Face 2 has vertices " << f2.vertices[0] << ", " << f2.vertices[1] << ", " << f2.vertices[2];
#endif

	// Set the adjacent face to the new faces
	f0.adjacentFaces[0] = faces.size();
	f0.adjacentFaces[1] = faces.size() + 1;
	f0.adjacentFaces[2] = face.adjacentFaces[2];
#ifdef _DEBUG
	olog(Finest) << "Face 0 has vertices " << f0.adjacentFaces[0] << ", " << f0.adjacentFaces[1] << ", " << f0.adjacentFaces[2];
#endif

	f1.adjacentFaces[0] = faces.size() + 1;
	f1.adjacentFaces[1] = pos;
	f1.adjacentFaces[2] = face.adjacentFaces[0];
#ifdef _DEBUG
	olog(Finest) << "Face 1 has vertices " << f1.adjacentFaces[0] << ", " << f1.adjacentFaces[1] << ", " << f1.adjacentFaces[2];
#endif

	f2.adjacentFaces[0] = pos;
	f2.adjacentFaces[1] = faces.size();
	f2.adjacentFaces[2] = face.adjacentFaces[1];
#ifdef _DEBUG
	olog(Finest) << "Face 2 has vertices " << f2.adjacentFaces[0] << ", " << f2.adjacentFaces[1] << ", " << f2.adjacentFaces[2];
#endif

	// if there is an adjacent face of the face 0 in front of the new vertex
 	if (f0.adjacentFaces[2] != -1)
 	{
		Face& adjacent_face = faces[f0.adjacentFaces[2]];
		for (unsigned k = 0; k < adjacent_face.Size(); ++k) // for every vertices of the adjacent face
		{
			// We try to find which of its vertex is at the opposite of our face
			if (adjacent_face.vertices[k] != f0.vertices[0]
				&& adjacent_face.vertices[k] != f0.vertices[1])
			{
#ifdef _DEBUG
				olog(Finest) << "Face n°" << pos << " is added to the adjacent faces of face n°" << f0.adjacentFaces[2] << " at vertex n°" << adjacent_face.vertices[k];
#endif
				adjacent_face.adjacentFaces[k] = pos;
				break;
			}
		}
 	}
	else
	{ // else we have found an edge of the convex hull
		auto it = convexHull.begin();
		while (it != convexHull.end())
		{
			if (it->at(0) == f0.vertices[0] && it->at(1) == f0.vertices[1])
			{
				it->at(2) = pos;
			}
			++it;
		}
	}

	// if there is an adjacent face of the face 1 in front of the new vertex
	if (f1.adjacentFaces[2] != -1)
	{
		Face& adjacent_face = faces[f1.adjacentFaces[2]];
		for (unsigned k = 0; k < adjacent_face.Size(); ++k) // for every vertices of the adjacent face
		{
			// We try to find which of its vertex is at the opposite of our face
			if (adjacent_face.vertices[k] != f1.vertices[0]
				&& adjacent_face.vertices[k] != f1.vertices[1])
			{
#ifdef _DEBUG
				olog(Finest) << "Face n°" << faces.size() << " is added to the adjacent faces of face n°" << f1.adjacentFaces[2] << " at the opposite of vertex n°" << adjacent_face.vertices[k];
#endif
				adjacent_face.adjacentFaces[k] = faces.size();
				break;
			}
		}
	}
	else
	{ // else we have found an edge of the convex hull
		auto it = convexHull.begin();
		while (it != convexHull.end())
		{
			if (it->at(0) == f1.vertices[0] && it->at(1) == f1.vertices[1])
			{
				it->at(2) = faces.size();
			}
			++it;
		}
	}

	// if there is an adjacent face of the face 2 in front of the new vertex
	if (f2.adjacentFaces[2] != -1)
	{
		Face& adjacent_face = faces[f2.adjacentFaces[2]];
		for (unsigned k = 0; k < adjacent_face.Size(); ++k) // for every vertices of the adjacent face
		{
			// We try to find which of its vertex is at the opposite of our face
			if (adjacent_face.vertices[k] != f2.vertices[0]
				&& adjacent_face.vertices[k] != f2.vertices[1])
			{
#ifdef _DEBUG
				olog(Finest) << "Face n°" << faces.size() + 1 << " is added to the adjacent faces of face n°" << f2.adjacentFaces[2] << " at vertex n°" << adjacent_face.vertices[k];
#endif
				adjacent_face.adjacentFaces[k] = faces.size() + 1;
				break;
			}
		}
	}
	else
	{ // else we have found an edge of the convex hull
		auto it = convexHull.begin();
		while (it != convexHull.end())
		{
			if (it->at(0) == f2.vertices[0] && it->at(1) == f2.vertices[1])
			{
				it->at(2) = faces.size() + 1;
			}
			++it;
		}
	}

	for (unsigned i = 0; i < f0.Size(); ++i)
	{
		vertices[f0.vertices[i]].incidentFace = pos;
		vertices[f1.vertices[i]].incidentFace = faces.size();
		vertices[f2.vertices[i]].incidentFace = faces.size() + 1;
	}

	faces[pos] = f0;
	faces.push_back(f1);
	faces.push_back(f2);
}
