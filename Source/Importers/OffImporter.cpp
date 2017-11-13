#include "OffImporter.hpp"
#include "../Logger.h"
#include "../Mesh/Vertex.hpp"

#include <chrono>
#include <vector>
#include <map>
#include "../Mesh/Face.hpp"
#include "../Postprocessing/Triangulation.hpp"
#include <list>

namespace Ez
{
	Mesh OffImporter::ReadFromFile(const std::string& path, const Postprocessing pp)
	{

#ifdef _DEBUG
		olog(Info) << "Reading Off file : " << path;
		const auto t1 = std::chrono::high_resolution_clock::now();
#endif

		// Read file from path

		std::ifstream input_stream;
		input_stream.exceptions(std::ifstream::badbit | std::ifstream::failbit);

		std::stringstream stream;
		try
		{
			input_stream.open(path);
			stream << input_stream.rdbuf();
			input_stream.close();
		}
		catch (std::ifstream::failure& error) { throw std::ifstream::failure(error); }

		unsigned nb_vertices;
		unsigned nb_faces;
		std::string dump;

		std::string buf;

		stream >> dump;
		stream >> nb_vertices;
		stream >> nb_faces;
		stream >> dump;
		
		Mesh result;
		switch (pp) {
		case Postprocessing::Naive: 
			result = LoadWithNaiveTriangulation(stream, nb_vertices, nb_faces); 
			break;
		case Postprocessing::Lawson:
			result = LoadWithLawson(stream, nb_vertices, nb_faces);
			break;
		case Postprocessing::Delaunay: break;
		default: result = LoadWithNoPostProcessing(stream, nb_vertices, nb_faces);
		}

#ifdef _DEBUG

		const auto t2 = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::microseconds
		>(t2 - t1).count();

		olog(Info) << "File has been read in " << duration << "µs";
#endif
		return std::move(result);
	}

	Mesh OffImporter::LoadWithNoPostProcessing(std::stringstream& stream, const unsigned nbVertices, const unsigned nbFaces)
	{
		// Reading vertices
		std::vector<Vertex> vertices(nbVertices);
		for (unsigned i = 0; i < nbVertices; ++i)
		{
			Vertex vertex;
			stream >> vertex.position.x;
			stream >> vertex.position.y;
			stream >> vertex.position.z;
			vertex.incidentFace = 0;

#ifdef _DEBUG
			olog(Finest) << "Reading Vertex n°" << i << " with coordinates " << vertex.position;
#endif
			vertices[i] = std::move(vertex);
		}

		//Reading faces
		std::vector<Face> faces(nbFaces);
		std::map<std::pair<unsigned, unsigned>, unsigned> mapped_faces; // map an edge with its face, used to check if a face with a specific edge already exists.

		for (unsigned i = 0; i < nbFaces; ++i) // for every faces
		{
			unsigned nb_indices;
			stream >> nb_indices;
			Face face;

			for (unsigned j = 0; j < face.Size(); ++j) // for every vertices in the face
			{
				stream >> face.vertices[j];	// We get the indice of the vertex
				vertices[face.vertices[j]].incidentFace = i; // We set the current face as the incident face of the vertex
			}

#ifdef _DEBUG
			olog(Finest) << "Reading Face n°" << i << " with vertex indices : " 
				<< face.vertices[0] << ", " << face.vertices[1] << ", " << face.vertices[2];
#endif

			// Once we have all the vertices of the face
			for (unsigned j = 0; j < face.Size(); ++j)
			{
				const auto it = mapped_faces.find(
					std::make_pair(
						face.vertices[j],
						face.vertices[face.Next(j)]
					)
				); // We try to find if the edge {j, Next(j)} already exists.

#ifdef _DEBUG
				olog(Finest) << "Trying to find edge {" << face.vertices[j] << ", " << face.vertices[face.Next(j)] << "}";
#endif

				if (it != mapped_faces.end()) // If it does 
				{
#ifdef _DEBUG
					olog(Finest) << "Edge found on face " << it->second << " it becomes the adjacent face of face n°" << i;
#endif

					face.adjacentFaces[face.Previous(j)] = it->second;
					// We get the face that share the edge (aka. an adjacent face) 
					// And we set it to the index of the opposite vertex

					Face &adjacent_face = faces[it->second]; // We get the adjacent face
					for (unsigned k = 0; k < adjacent_face.Size(); ++k) // for every vertices of the adjacent face
					{
						// We try to find which of its vertex is at the opposite of our face
						if (adjacent_face.vertices[k] != face.vertices[j]
							&& adjacent_face.vertices[k] != face.vertices[face.Next(j)])
						{
#ifdef _DEBUG
							olog(Finest) << "Face n°" << i << " is added to the adjacent faces of face n°" << it->second << " at vertex n°" << adjacent_face.vertices[k];
#endif
							adjacent_face.adjacentFaces[k] = i;
							break;
						}
					}
				}

				// We insert the edges of the face in the map
				// We reverse the edge because the edge of the adjacent face will be in the reversed order
				mapped_faces.insert(std::make_pair(
					std::make_pair(
						face.vertices[face.Next(j)],
						face.vertices[j]),
					i));
#ifdef _DEBUG
				olog(Finest) << "Inserting edge {" << face.vertices[face.Next(j)] << ", " << face.vertices[j] << "}";
#endif
			}

			faces[i] = std::move(face);
		}

		return std::move(Mesh(std::move(vertices), std::move(faces)));
	}

	Mesh OffImporter::LoadWithNaiveTriangulation(std::stringstream& stream, const unsigned nbVertices, const unsigned nbFaces)
	{
		// Reading vertices
		std::vector<Vertex> vertices;
		std::vector<Face> faces;
		std::list <std::array<unsigned, 3>> convex_hull;

		vertices.reserve(nbVertices);
		faces.reserve(nbFaces);

#ifdef _DEBUG
		olog(Finest) << "Inserting first face";
#endif
		Face first_face;
		for (unsigned i = 0; i < 3; ++i)
		{
			Vertex vertex;
			stream >> vertex.position.x;
			stream >> vertex.position.y;
			stream >> vertex.position.z;

#ifdef _DEBUG
			olog(Finest) << "Reading Vertex n°" << i << " with coordinates " << vertex.position;
#endif

			first_face.vertices[i] = i;

			vertices.push_back(vertex);
		}

		Triangulation::AddFirstFace(vertices, faces, first_face, convex_hull);

		for (unsigned i = 3; i < nbVertices; ++i)
		{
			Vertex vertex;
			stream >> vertex.position.x;
			stream >> vertex.position.y;
			stream >> vertex.position.z;
#ifdef _DEBUG
			olog(Finest) << "Reading Vertex n°" << i << " with coordinates " << vertex.position;
#endif

			Triangulation::AddPointNaively(vertices, faces, vertex, convex_hull);
		}


		return std::move(Mesh(vertices, faces));
	}
	
	Mesh OffImporter::LoadWithLawson(std::stringstream& stream, const unsigned nbVertices, const unsigned nbFaces)
	{
		// Reading vertices
		std::vector<Vertex> vertices;
		std::vector<Face> faces;
		std::list <std::array<unsigned, 3>> convex_hull;

		vertices.reserve(nbVertices);
		faces.reserve(nbFaces);

#ifdef _DEBUG
		olog(Finest) << "Inserting first face";
#endif
		Face first_face;
		for (unsigned i = 0; i < 3; ++i)
		{
			Vertex vertex;
			stream >> vertex.position.x;
			stream >> vertex.position.y;
			stream >> vertex.position.z;

#ifdef _DEBUG
			olog(Finest) << "Reading Vertex n°" << i << " with coordinates " << vertex.position;
#endif

			first_face.vertices[i] = i;

			vertices.push_back(vertex);
		}

		Triangulation::AddFirstFace(vertices, faces, first_face, convex_hull);

		for (unsigned i = 3; i < nbVertices; ++i)
		{
			Vertex vertex;
			stream >> vertex.position.x;
			stream >> vertex.position.y;
			stream >> vertex.position.z;
#ifdef _DEBUG
			olog(Finest) << "Reading Vertex n°" << i << " with coordinates " << vertex.position;
#endif

			Triangulation::AddPointNaively(vertices, faces, vertex, convex_hull);
		}

		Triangulation::Lawson(vertices, faces);

		return std::move(Mesh(vertices, faces));
	}

}

