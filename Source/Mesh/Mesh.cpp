#include "Mesh.hpp"
namespace Ez
{
	Mesh::Mesh()
	{
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<Face>& faces)
		: mVertices(vertices), mFaces(faces)
	{
	}

	const std::vector<Vertex>& Mesh::_Vertices() const
	{
		return mVertices;
	}

	const std::vector<Face>& Mesh::_Faces() const
	{
		return mFaces;
	}
}
