#include "iomesh.h"

bool importMesh(TriMesh &mesh, string filename)
{
	mesh.request_vertex_texcoords2D();
    OpenMesh::IO::Options opt;
	opt += OpenMesh::IO::Options::VertexTexCoord;
	//opt.check(OpenMesh::IO::Options::VertexTexCoord);
	/*mesh.request_face_normals();
	mesh.request_vertex_normals();
	mesh.update_normals();*/
    //opt += OpenMesh::IO::Options::VertexColor;
    if(!OpenMesh::IO::read_mesh(mesh, filename, opt))
        return false;
    return true;
}

bool exportMesh(TriMesh &mesh, string filename)
{
    OpenMesh::IO::Options opt;
    //opt += OpenMesh::IO::Options::VertexColor;
    return OpenMesh::IO::write_mesh(mesh, filename,opt);
}
