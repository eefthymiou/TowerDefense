#include <iostream>
#include <sstream>
#include <map>
#include <tinyxml2.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "util.h"
#include "model.h"
#include "texture.h"

//load a file with AssImp
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdlib.h> // memory management
#include <assert.h>
#include "maths_funcs.h"

using namespace glm;
using namespace std;
using namespace ogl;
using namespace tinyxml2;


my_mat4 convert_assimp_matrix (aiMatrix4x4 m) {
    /* entered in columns! */
    return my_mat4 (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        m.a4, m.b4, m.c4, m.d4
    );
}

Skeleton_Node* find_node_in_skeleton (
    Skeleton_Node* root, const char* node_name) {
        assert (root); // validate self
        // look for match
        if (strcmp (node_name, root->name) == 0) {
        return root;
    }
    // recurse to chidlren
    for (int i = 0; i < root->num_children; i++) {
        Skeleton_Node* child = find_node_in_skeleton (root->children[i], node_name);
        if (child != NULL) {
        return child;
        }
    }
    // no children match and no self match
    return NULL;
}

bool import_skeleton_node (
    aiNode* assimp_node,
    Skeleton_Node** skeleton_node,
    int bone_count,
    char bone_names[][64]
) {
    // allocate memory for node
    Skeleton_Node* temp = (Skeleton_Node*)malloc (sizeof (Skeleton_Node));
    // get node properties out of AssImp
    strcpy (temp->name, assimp_node->mName.C_Str ());
    printf ("-node name = %s\n", temp->name);
    temp->num_children = 0;
    printf ("node has %i children\n", (int)assimp_node->mNumChildren);
    temp->bone_index = -1;
    for (int i = 0; i < 20; i++) {
        temp->children[i] = NULL;
    }
    // look for matching bone name
    bool has_bone = false;
    for (int i = 0; i < bone_count; i++) {
        if (strcmp (bone_names[i], temp->name) == 0) {
            printf ("node uses bone %i\n", i);
            temp->bone_index = i;
            has_bone = true;
            break;
        }
    }
    if (!has_bone) printf ("no bone found for node\n");

    bool has_useful_child = false;
    for (int i = 0; i < (int)assimp_node->mNumChildren; i++) {
        if (import_skeleton_node (
            assimp_node->mChildren[i],
            &temp->children[temp->num_children],
            bone_count,
            bone_names)) {
        has_useful_child = true;
        temp->num_children++;
        } else printf ("useless child culled\n");
    }
    if (has_useful_child || has_bone) {
        // point parameter to our allocated node
        *skeleton_node = temp;
        return true;
    }
    // no bone or good children - cull self
    free (temp);
    temp = NULL;
    return false;
}


bool load_mesh (
        const char* file_name,
        GLuint* vao,
        int* point_count,
        my_mat4* bone_offset_mats,
        int* bone_count,
        Skeleton_Node** root_node,
        double* anim_duration
    ) {
    /* load file with assimp and print some stats */
    const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate);
    if (!scene) {
        fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
    return false;
    }
    printf (" %i animations\n", scene->mNumAnimations);
    printf (" %i cameras\n", scene->mNumCameras);
    printf (" %i lights\n", scene->mNumLights);
    printf (" %i materials\n", scene->mNumMaterials);
    printf (" %i meshes\n", scene->mNumMeshes);
    printf (" %i textures\n", scene->mNumTextures);

    /* get first mesh in file only */
    const aiMesh* mesh = scene->mMeshes[0];
    printf (" %i vertices in mesh[0]\n", mesh->mNumVertices);

    /* pass back number of vertex points in mesh */
    *point_count = mesh->mNumVertices;
    /* generate a VAO, using the pass-by-reference parameter that we give to the
    function */
    glGenVertexArrays (1, vao);
    glBindVertexArray (*vao);

    /* we really need to copy out all the data from AssImp's funny little data
    structures into pure contiguous arrays before we copy it into data buffers
    because assimp's texture coordinates are not really contiguous in memory.
    i allocate some dynamic memory to do this. */
    GLfloat* points = NULL; // array of vertex points
    GLfloat* normals = NULL; // array of vertex normals
    GLfloat* texcoords = NULL; // array of texture coordinates
    GLint* bone_ids = NULL; // array of bone ID

    if (mesh->HasPositions ()) {
        points = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
        for (int i = 0; i < *point_count; i++) {
            const aiVector3D* vp = &(mesh->mVertices[i]);
            points[i * 3] = (GLfloat)vp->x;
            points[i * 3 + 1] = (GLfloat)vp->y;
            points[i * 3 + 2] = (GLfloat)vp->z;
        }
    }
    if (mesh->HasNormals ()) {
        normals = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
        for (int i = 0; i < *point_count; i++) {
            const aiVector3D* vn = &(mesh->mNormals[i]);
            normals[i * 3] = (GLfloat)vn->x;
            normals[i * 3 + 1] = (GLfloat)vn->y;
            normals[i * 3 + 2] = (GLfloat)vn->z;
        }
    }
    if (mesh->HasTextureCoords (0)) {
        texcoords = (GLfloat*)malloc (*point_count * 2 * sizeof (GLfloat));
        for (int i = 0; i < *point_count; i++) {
            const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
            texcoords[i * 2] = (GLfloat)vt->x;
            texcoords[i * 2 + 1] = (GLfloat)vt->y;
        }
    }
    /* copy mesh data into VBOs */
    if (mesh->HasPositions ()) {
        GLuint vbo;
        glGenBuffers (1, &vbo);
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData (
            GL_ARRAY_BUFFER,
            3 * *point_count * sizeof (GLfloat),
            points,
            GL_STATIC_DRAW
        );
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray (0);
    free (points); // free our temporary memory
    }
    if (mesh->HasNormals ()) {
        GLuint vbo;
        glGenBuffers (1, &vbo);
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData (
            GL_ARRAY_BUFFER,
            3 * *point_count * sizeof (GLfloat),
            normals,
            GL_STATIC_DRAW
        );
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray (1);
    free (normals); // free our temporary memory
    }
    if (mesh->HasTextureCoords (0)) {
        GLuint vbo;
        glGenBuffers (1, &vbo);
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData (
            GL_ARRAY_BUFFER,
            2 * *point_count * sizeof (GLfloat),
            texcoords,
            GL_STATIC_DRAW
        );
        glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray (2);
        free (texcoords); // free our temporary memory
    }
    if (mesh->HasTangentsAndBitangents ()) {
    // NB: could store/print tangents here
    }
    /* extract bone weights */
    if (mesh->HasBones ()) {
        *bone_count = (int)mesh->mNumBones;
        /* an array of bones names. max 256 bones, max name length 64 */
        char bone_names[256][64];
        bone_ids = (int*)malloc (*point_count * sizeof (int));
        for (int b_i = 0; b_i < *bone_count; b_i++) {
            const aiBone* bone = mesh->mBones[b_i];
            strcpy (bone_names[b_i], bone->mName.data);
            printf ("bone_names[%i]=%s\n", b_i, bone_names[b_i]);
            bone_offset_mats[b_i] = convert_assimp_matrix(bone->mOffsetMatrix);
            /* get weights here later */
                /* get bone weights
                we can just assume weight is always 1.0, because we are just using 1 bone
                per vertex. but any bone that affects a vertex will be assigned as the
                vertex' bone_id */
                int num_weights = (int)bone->mNumWeights;
                for (int w_i = 0; w_i < num_weights; w_i++) {
                    aiVertexWeight weight = bone->mWeights[w_i];
                    int vertex_id = (int)weight.mVertexId;
                    // ignore weight if less than 0.5 factor
                    if (weight.mWeight >= 0.5f) {
                        bone_ids[vertex_id] = b_i;
                    }
                }
        } // endfor
        GLuint vbo;
        glGenBuffers (1, &vbo);
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData (
        GL_ARRAY_BUFFER,
        *point_count * sizeof (GLint),
        bone_ids,
        GL_STATIC_DRAW
        );
        glVertexAttribIPointer(3, 1, GL_INT, 0, NULL);
        glEnableVertexAttribArray (3);

        aiNode* assimp_node = scene->mRootNode;
        if (!import_skeleton_node (
            assimp_node,
            root_node,
            *bone_count,
            bone_names
        )) {
            printf ("WARNING: no skeleton found in mesh\n");
        } // endi

        if (scene->mNumAnimations > 0) {
            // get just the first animation
            aiAnimation* anim = scene->mAnimations[0];
            printf ("animation name: %s\n", anim->mName.C_Str ());
            printf ("animation has %i node channels\n", anim->mNumChannels);
            printf ("animation has %i mesh channels\n", anim->mNumMeshChannels);
            printf ("animation duration %f\n", anim->mDuration);
            printf ("ticks per second %f\n", anim->mTicksPerSecond);
            *anim_duration = anim->mDuration;

            /* WE WILL GET KEYS HERE */
            // get the node channels
            for (int i = 0; i < (int)anim->mNumChannels; i++) {
                aiNodeAnim* chan = anim->mChannels[i];
                // find the matching node in our skeleton by name
                Skeleton_Node* sn = find_node_in_skeleton (*root_node, chan->mNodeName.C_Str ());
                assert (sn);
                sn->num_pos_keys = chan->mNumPositionKeys;
                sn->num_rot_keys = chan->mNumRotationKeys;
                sn->num_sca_keys = chan->mNumScalingKeys;
                // allocate memory
                sn->pos_keys = (my_vec3*)malloc (sizeof (my_vec3) * sn->num_pos_keys);
                sn->rot_keys = (versor*)malloc (sizeof (versor) * sn->num_rot_keys);
                sn->sca_keys = (my_vec3*)malloc (sizeof (my_vec3) * sn->num_sca_keys);
                sn->pos_key_times = (double*)malloc (sizeof (double) * sn->num_pos_keys);
                sn->rot_key_times = (double*)malloc (sizeof (double) * sn->num_rot_keys);
                sn->sca_key_times = (double*)malloc (sizeof (double) * sn->num_sca_keys);
                // add position keys to node
                for (int i = 0; i < sn->num_pos_keys; i++) {
                aiVectorKey key = chan->mPositionKeys[i];
                sn->pos_keys[i].v[0] = key.mValue.x;
                sn->pos_keys[i].v[1] = key.mValue.y;
                sn->pos_keys[i].v[2] = key.mValue.z;
                sn->pos_key_times[i] = key.mTime;
                }
                // add rotation keys to node
                for (int i = 0; i < sn->num_rot_keys; i++) {
                aiQuatKey key = chan->mRotationKeys[i];
                sn->rot_keys[i].q[0] = key.mValue.w;
                sn->rot_keys[i].q[1] = key.mValue.x;
                sn->rot_keys[i].q[2] = key.mValue.y;
                sn->rot_keys[i].q[3] = key.mValue.z;
                sn->rot_key_times[i] = key.mTime;
                }
                // add scaling keys to node
                for (int i = 0; i < sn->num_sca_keys; i++) {
                aiVectorKey key = chan->mScalingKeys[i];
                sn->sca_keys[i].v[0] = key.mValue.x;
                sn->sca_keys[i].v[1] = key.mValue.y;
                sn->sca_keys[i].v[2] = key.mValue.z;
                sn->sca_key_times[i] = key.mTime;
                } // endfor
            }
        }
        free (bone_ids);
    } // endif

    /* free assimp's copy of memory */
    aiReleaseImport (scene);
    printf ("mesh loaded\n");
    return true;
}

// simple OBJ loader
void loadOBJ(
    const string& path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices
) {
    cout << "Loading OBJ file: " << path << endl;

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<vec3> temp_vertices;
    vector<vec2> temp_uvs;
    vector<vec3> temp_normals;
    indices.clear();

    FILE * file = fopen(path.c_str(), "r");
    if (file == NULL) {
        throw runtime_error("Can't open the file.\n");
    }

    while (1) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            // Invert V coordinate since we will only use DDS texture,
            // which are inverted. Remove if you want to use TGA or BMP loaders.
            uv.y = -uv.y;
            temp_uvs.push_back(uv);
        } else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0) {
            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                fclose(file);
                throw runtime_error("File can't be read by our simple parser.\n");
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        } else {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        vec3 vertex = temp_vertices[vertexIndex - 1];
        vec2 uv = temp_uvs[uvIndex - 1];
        vec3 normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        vertices.push_back(vertex);
        uvs.push_back(uv);
        normals.push_back(normal);
        indices.push_back(indices.size());
    }
    fclose(file);
}

void loadVTP(
    const string& path,
    vector<vec3>& vertices, vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices) {
    indices.clear();
    const char* method = "PolygonalMesh::loadVtpFile()";
    XMLDocument vtp;
    auto res = vtp.LoadFile(path.c_str());
    assert(res == 0);

    XMLElement* root = vtp.FirstChildElement("VTKFile");
    assert(root != nullptr);
    assert(root->Attribute("type", "PolyData"));

    XMLElement* polydata = root->FirstChildElement("PolyData");
    assert(polydata != nullptr);
    XMLElement* piece = polydata->FirstChildElement("Piece");
    assert(piece != nullptr);
    XMLElement* enormals = piece->FirstChildElement("PointData");
    assert(enormals != nullptr);
    XMLElement* points = piece->FirstChildElement("Points");
    assert(points != nullptr);

    int numPoints, numPolys;
    piece->QueryIntAttribute("NumberOfPoints", &numPoints);
    piece->QueryIntAttribute("NumberOfPolys", &numPolys);

    //assert(enormals->Attribute("format", "ascii"));
    const char* normalsStr = enormals->FirstChildElement("DataArray")->FirstChild()->Value();
    stringstream sNorm(normalsStr);
    vector<vec3> tempNormals;
    do {
        vec3 normal;
        sNorm >> normal.x >> normal.y >> normal.z;
        tempNormals.push_back(normal);
    } while (sNorm.good());
    assert(tempNormals.size() == numPoints + 1);

    XMLElement* pointData = points->FirstChildElement("DataArray");
    assert(pointData != nullptr);
    assert(pointData->Attribute("format", "ascii"));
    const char* coordsStr = points->FirstChildElement("DataArray")->FirstChild()->Value();

    stringstream sCoord(coordsStr);
    vector<vec3> coordinates;
    do {
        vec3 coord;
        sCoord >> coord.x >> coord.y >> coord.z;
        coordinates.push_back(coord);
    } while (sCoord.good());
    coordinates.pop_back();  // the last is corrupted
    assert(coordinates.size() == numPoints);

    XMLElement* polys = piece->FirstChildElement("Polys");
    assert(polys != nullptr);

    // TODO beter implementation
    XMLElement *econnectivity, *eoffsets;
    if (polys->FirstChildElement("DataArray")->Attribute("Name", "connectivity")) {
        econnectivity = polys->FirstChildElement("DataArray");
        assert(econnectivity != nullptr);
        assert(econnectivity->Attribute("format", "ascii"));
    } else {
        throw runtime_error("Can't access connectivity");
    }

    if (polys->LastChildElement("DataArray")->Attribute("Name", "offsets")) {
        eoffsets = polys->LastChildElement("DataArray");
        assert(eoffsets != nullptr);
        assert(eoffsets->Attribute("format", "ascii"));
    } else {
        throw runtime_error("Can't access offsets");
    }

    // read offsets
    const char* offsetsStr = eoffsets->FirstChild()->Value();
    stringstream sOffsets(offsetsStr);
    vector<int> offsets;
    do {
        int offset;
        sOffsets >> offset;
        offsets.push_back(offset);
    } while (sOffsets.good());
    assert(offsets.size() == numPolys + 1);

    // read connectivity
    const char* connStr = econnectivity->FirstChild()->Value();
    stringstream sConn(connStr);
    vector<int> connectivity;
    do {
        int conn;
        sConn >> conn;
        connectivity.push_back(conn);
    } while (sConn.good());
    assert(connectivity.size() == offsets.back() + 1);

    // construct vertices
    int startPoly = 0;
    for (int i = 0; i < numPolys; ++i) {
        vector<int> face = slice(connectivity, startPoly, offsets[i]);
        int i1 = 0, i2 = 1, i3 = 2;
        while (i3 != face.size()) {
            vertices.push_back(coordinates[face[i1]]);
            normals.push_back(tempNormals[face[i1]]);
            indices.push_back(indices.size());
            vertices.push_back(coordinates[face[i2]]);
            normals.push_back(tempNormals[face[i2]]);
            indices.push_back(indices.size());
            vertices.push_back(coordinates[face[i3]]);
            normals.push_back(tempNormals[face[i3]]);
            indices.push_back(indices.size());
            i2++;
            i3++;
        }
        startPoly = offsets[i];
    }
}

void loadOBJWithTiny(
    const string& path,
    vector<vec3>& vertices,
    vector<vec2>& uvs,
    vector<vec3>& normals,
    vector<unsigned int>& indices) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
        throw runtime_error(err);
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vec3 vertex = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]};
            if (attrib.texcoords.size() != 0) {
                vec2 uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1 - attrib.texcoords[2 * index.texcoord_index + 1]};
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0) {
                vec3 normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]};
                normals.push_back(normal);
            }

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    // TODO .mtl loader
}

struct PackedVertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex that) const {
        return memcmp((void*) this, (void*) &that, sizeof(PackedVertex)) > 0;
    };
};

bool getSimilarVertexIndex(
    PackedVertex& packed,
    map<PackedVertex, unsigned int>& vertexToOutIndex,
    unsigned int& result) {
    map<PackedVertex, unsigned int>::iterator it = vertexToOutIndex.find(packed);
    if (it == vertexToOutIndex.end()) {
        return false;
    } else {
        result = it->second;
        return true;
    }
}

void indexVBO(
    const vector<vec3>& in_vertices,
    const vector<vec2>& in_uvs,
    const vector<vec3>& in_normals,
    vector<unsigned int>& out_indices,
    vector<vec3>& out_vertices,
    vector<vec2>& out_uvs,
    vector<vec3>& out_normals) {
    map<PackedVertex, unsigned int> vertexToOutIndex;

    // For each input vertex
    for (int i = 0; i < static_cast<int>(in_vertices.size()); i++) {
        vec3 vertices = in_vertices[i];
        vec2 uvs;
        vec3 normals;
        if (in_uvs.size() != 0) uvs = in_uvs[i];
        if (in_normals.size() != 0) normals = in_normals[i];
        PackedVertex packed = {vertices, uvs, normals};

        // Try to find a similar vertex in out_XXXX
        unsigned int index;
        bool found = getSimilarVertexIndex(packed, vertexToOutIndex, index);

        if (found) { // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back(index);
        } else { // If not, it needs to be added in the output data.
            out_vertices.push_back(vertices);
            if (in_uvs.size() != 0) out_uvs.push_back(uvs);
            if (in_normals.size() != 0) out_normals.push_back(normals);
            unsigned int newindex = (unsigned int) out_vertices.size() - 1;
            out_indices.push_back(newindex);
            vertexToOutIndex[packed] = newindex;
        }
    }
}

Drawable::Drawable(string path) {
    if (path.substr(path.size() - 3, 3) == "obj") {
        loadOBJWithTiny(path.c_str(), vertices, uvs, normals, VEC_UINT_DEFAUTL_VALUE);
    } else if (path.substr(path.size() - 3, 3) == "vtp") {
        loadVTP(path.c_str(), vertices, uvs, normals, VEC_UINT_DEFAUTL_VALUE);
    } else {
        throw runtime_error("File format not supported: " + path);
    }

    createContext();
}

Drawable::Drawable(const vector<vec3>& vertices, const vector<vec2>& uvs,
                   const vector<vec3>& normals) : vertices(vertices), uvs(uvs), normals(normals) {
    createContext();
}

Drawable::~Drawable() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteBuffers(1, &VAO);
}

void Drawable::bind() {
    glBindVertexArray(VAO);
}

void Drawable::draw(int mode) {
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Drawable::createContext() {
    indices = vector<unsigned int>();
    indexVBO(vertices, uvs, normals, indices, indexedVertices, indexedUVS, indexedNormals);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3),
                 &indexedVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (indexedNormals.size() != 0) {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3),
                     &indexedNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (indexedUVS.size() != 0) {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedUVS.size() * sizeof(vec2),
                     &indexedUVS[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
}

/*****************************************************************************/

Mesh::Mesh(
    const vector<vec3>& vertices,
    const vector<vec2>& uvs,
    const vector<vec3>& normals,
    const Material& mtl)
    : vertices{vertices}, uvs{uvs}, normals{normals}, mtl{mtl} {
    createContext();
}

Mesh::Mesh(Mesh&& other)
    : vertices{std::move(other.vertices)}, normals{std::move(other.normals)},
    indexedVertices{std::move(other.indexedVertices)}, indexedNormals{std::move(other.indexedNormals)},
    uvs{std::move(other.uvs)}, indexedUVS{std::move(other.indexedUVS)},
    indices{std::move(other.indices)}, mtl{std::move(other.mtl)},
    VAO{other.VAO}, verticesVBO{other.verticesVBO}, normalsVBO{other.normalsVBO},
    uvsVBO{other.uvsVBO}, elementVBO{other.elementVBO} {
    other.VAO = 0;
    other.verticesVBO = 0;
    other.normalsVBO = 0;
    other.uvsVBO = 0;
    other.elementVBO = 0;
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &uvsVBO);
    glDeleteBuffers(1, &normalsVBO);
    glDeleteBuffers(1, &elementVBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh::bind() {
    glBindVertexArray(VAO);
}

void Mesh::draw(int mode) {
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
}

void Mesh::createContext() {
    indices = vector<unsigned int>();
    indexVBO(vertices, uvs, normals, indices, indexedVertices, indexedUVS, indexedNormals);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, indexedVertices.size() * sizeof(vec3),
                 &indexedVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    if (indexedNormals.size() != 0) {
        glGenBuffers(1, &normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedNormals.size() * sizeof(vec3),
                     &indexedNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
    }

    if (indexedUVS.size() != 0) {
        glGenBuffers(1, &uvsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
        glBufferData(GL_ARRAY_BUFFER, indexedUVS.size() * sizeof(vec2),
                     &indexedUVS[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
    }

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
}

Model::Model(string path, Model::MTLUploadFunction* uploader)
    : uploadFunction{uploader} {
    if (path.substr(path.size() - 3, 3) == "obj") {
        loadOBJWithTiny(path.c_str());
    } else {
        throw runtime_error("File format not supported: " + path);
    }
}

Model::~Model() {
    for (const auto& t : textures) {
        glDeleteTextures(1, &t.second);
    }
}

void Model::draw() {
    for (auto& mesh : meshes) {
        mesh.bind();
        if (uploadFunction)
            uploadFunction(mesh.mtl);
        mesh.draw();
    }
}

void Model::loadOBJWithTiny(const std::string& filename) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str())) {
        throw runtime_error(err);
    }

    for (const auto& material : materials) {
        loadTexture(material.ambient_texname);
        loadTexture(material.diffuse_texname);
        loadTexture(material.specular_texname);
        loadTexture(material.specular_highlight_texname);
    }

    for (const auto& shape : shapes) {
        vector<vec3> vertices{};
        vector<vec2> uvs{};
        vector<vec3> normals{};
        for (const auto& index : shape.mesh.indices) {
            int vertex_index = index.vertex_index;
            if (vertex_index < 0) vertex_index += attrib.vertices.size() / 3;
            vec3 vertex = {
                attrib.vertices[3 * vertex_index + 0],
                attrib.vertices[3 * vertex_index + 1],
                attrib.vertices[3 * vertex_index + 2]};
            if (attrib.texcoords.size() != 0) {
                int texcoord_index = index.texcoord_index;
                if (texcoord_index < 0) texcoord_index += attrib.texcoords.size() / 2;
                vec2 uv = {
                    attrib.texcoords[2 * texcoord_index + 0],
                    1 - attrib.texcoords[2 * texcoord_index + 1]};
                uvs.push_back(uv);
            }
            if (attrib.normals.size() != 0) {
                int normal_index = index.normal_index;
                if (normal_index < 0) normal_index += attrib.normals.size() / 3;
                vec3 normal = {
                    attrib.normals[3 * normal_index + 0],
                    attrib.normals[3 * normal_index + 1],
                    attrib.normals[3 * normal_index + 2]};
                normals.push_back(normal);
            }
            vertices.push_back(vertex);
        }
        Material mtl{};
        if (materials.size() > 0 && shape.mesh.material_ids.size() > 0) {
            int idx = shape.mesh.material_ids[0];
            if (idx < 0 || idx >= static_cast<int>(materials.size()))
                idx = static_cast<int>(materials.size()) - 1;
            tinyobj::material_t mat = materials[idx];
            mtl = {
                {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1},
                {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1},
                {mat.specular[0], mat.specular[1], mat.specular[2], 1},
                mat.shininess,
                textures[mat.ambient_texname],
                textures[mat.diffuse_texname],
                textures[mat.specular_texname],
                textures[mat.specular_highlight_texname]
            };
            if (mtl.texKa) mtl.Ka.r = -1.0f;
            if (mtl.texKd) mtl.Kd.r = -1.0f;
            if (mtl.texKs) mtl.Ks.r = -1.0f;
            if (mtl.texNs) mtl.Ns = -1.0f;
        }
        meshes.emplace_back(vertices, uvs, normals, mtl);
    }
}

void Model::loadTexture(const std::string& filename) {
    if (filename.length() == 0) return;
    if (textures.find(filename) == end(textures)) {
        GLuint id = loadSOIL(filename.c_str());
        if (!id) throw std::runtime_error("Failed to load texture: " + filename);
        textures[filename] = id;
    }
}