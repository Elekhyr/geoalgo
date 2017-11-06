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
}
