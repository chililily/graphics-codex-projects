#pragma once

#include <G3D/G3D.h>
#include <assert.h>
#include <fstream>
#include <initializer_list>
#include <vector>

typedef std::string string;

// OFFdata class
// Container class to help organize OFF data,
// with write-to-file functionality.
class OFFdata
{
    string m_filepath;
    std::vector<string> m_comments;
    int n_vertices;
    int n_faces;
    int n_edges;
    int n_verts_per_face;
    std::vector<Point3> m_vertices;
    std::vector<std::vector<int>> m_face_verts;

public:
    OFFdata() = default;
    OFFdata(string filepath, int verts, int faces, int edges, int verts_per_face)
        : m_filepath{ std::move(filepath) },
        n_vertices{ verts },
        n_faces{ faces },
        n_edges{ edges },
        n_verts_per_face{ verts_per_face }
    {}

    // Getters
    string* getFilepath() { return &m_filepath; }
    int getnVertices() const { return n_vertices; }
    int getnFaces() const { return n_faces; }
    int getnEdges() const { return n_edges; }
    int getnVertsPerFace() const { return n_verts_per_face; }
    std::vector<Point3>* getVertices() { return &m_vertices; }
    std::vector<std::vector<int>>* getFaceVerts() { return &m_face_verts; }

    // Setters
    void setFilepath(string& filepath) { m_filepath = filepath; }
    void setnVertices(int v) { n_vertices = v; }
    void setnFaces(int f) { n_faces = f; }
    void setnEdges(int e) { n_edges = e; }
    void setnVertsPerFace(int v) { n_verts_per_face = v; }
    void setVertices(std::vector<Point3> verts) { m_vertices = std::move(verts); }
    void setFaceVerts(std::vector<std::vector<int>> face_verts) { m_face_verts = std::move(face_verts); }

    // Add data
    void appendComment(string comment) { m_comments.push_back(std::move(comment)); }
    void appendVertex(float x, float y, float z);
    void appendVertex(Point3 vertex);
    void appendFace(std::initializer_list<int> l);
    void appendFace(std::vector<int> face_verts);

    bool write();
};