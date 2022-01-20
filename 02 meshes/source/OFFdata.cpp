#include "OFFdata.h"

void OFFdata::appendVertex(float x, float y, float z)
{
    m_vertices.push_back(Point3{ x, y, z });
}
void OFFdata::appendVertex(Point3 vertex)
{
    m_vertices.push_back(vertex);
}

// Does not support colour information
void OFFdata::appendFace(std::initializer_list<int> inputs)
{
    assert(n_verts_per_face > 0 && inputs.size() == n_verts_per_face && "Invalid input: mismatch between number of inputs provided and number of vertices per face");

    std::vector<int> face_verts{ n_verts_per_face };
    for (int v : inputs)
    {
        face_verts.push_back(v);
    }
    m_face_verts.push_back(std::move(face_verts));
}
void OFFdata::appendFace(std::vector<int> face_verts)
{
    assert(n_verts_per_face > 0 && face_verts.size() == n_verts_per_face && "Invalid input: mismatch between number of inputs provided and number of vertices per face");

    face_verts.insert(face_verts.begin(), n_verts_per_face);
    m_face_verts.push_back(std::move(face_verts));
}

bool OFFdata::write()
{
    std::fstream file;
    file.open(m_filepath, std::ios::out);

    if (file.is_open())
    {
        file << "OFF\n";

        for (auto comment : m_comments)
        {
            file << "# " << comment << '\n';
        }

        file << '\n' << n_vertices << ' ' << n_faces << ' ' << n_edges << '\n';

        for (auto vertex : m_vertices)
        {
            file << vertex.x << ' ' << vertex.y << ' ' << vertex.z << '\n';
        }

        for (int i{ 0 }; i < m_face_verts.size(); ++i)
        {
            for (int v : m_face_verts[i])
            {
                file << v << ' ';
            }
            file << '\n';
        }
        file.close();
        return true;
    }
    else return false;
}