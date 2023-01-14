#include <iostream>
#include <sstream>
#include <map>
#include "util.h"
#include "texture.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//load a file with AssImp
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <assimp/Importer.hpp>
#include <stdlib.h> // memory management
#include <assert.h>
#include "maths_funcs.h"
#include "animation.h"
#define MAX_BONES 20
#define _USE_MATH_DEFINES

using namespace glm;
using namespace std;


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
    Skeleton_Node* temp = (Skeleton_Node*)malloc( sizeof( Skeleton_Node ) );

    // get node properties out of AssImp
    strcpy( temp->name, assimp_node->mName.C_Str() );
    printf( "-node name = %s\n", temp->name );
    temp->num_children = 0;

    /* initialise key-frame info */
    temp->pos_keys      = NULL;
    temp->rot_keys      = NULL;
    temp->sca_keys      = NULL;
    temp->pos_key_times = NULL;
    temp->rot_key_times = NULL;
    temp->sca_key_times = NULL;
    temp->num_pos_keys  = 0;
    temp->num_rot_keys  = 0;
    temp->num_sca_keys  = 0;

    printf( "node has %i children\n", (int)assimp_node->mNumChildren );
    temp->bone_index = -1;
    for ( int i = 0; i < MAX_BONES; i++ ) { temp->children[i] = NULL; }

    // look for matching bone name
    bool has_bone = false;
    for ( int i = 0; i < bone_count; i++ ) {
    if ( strcmp( bone_names[i], temp->name ) == 0 ) {
        printf( "node uses bone %i\n", i );
        temp->bone_index = i;
        has_bone         = true;
        break;
    }
    }
    if ( !has_bone ) { printf( "no bone found for node\n" ); }

    bool has_useful_child = false;
    for ( int i = 0; i < (int)assimp_node->mNumChildren; i++ ) {
    if ( import_skeleton_node( assimp_node->mChildren[i], &temp->children[temp->num_children], bone_count, bone_names ) ) {
        has_useful_child = true;
        temp->num_children++;
    } else {
        printf( "useless child culled\n" );
    }
    }
    if ( has_useful_child || has_bone ) {
    // point parameter to our allocated node
    *skeleton_node = temp;
    return true;
    }
    // no bone or good children - cull self
    free( temp );
    temp = NULL;
    return false;
}

vec3 Animation::get_root(vec3 prev_translation) {
    glm::vec3 translation;
    if (prev_translation.x<=18.0f){
        translation.x = prev_translation.x += 0.01;
        translation.y = 2.0f;
        translation.z = prev_translation.z += 0.01;
    }
    else translation=prev_translation;
    return translation;
}


void Animation::skeleton_animate (
    Skeleton_Node* node,
    double anim_time,
    my_mat4 parent_mat,
    my_mat4* bone_offset_mats,
    my_mat4* bone_animation_mats) {

    assert (node);
    /* the animation of a node after inheriting its parent's animation */
    my_mat4 our_mat = parent_mat;
    /* the animation for a particular bone at this time */
    my_mat4 local_anim = identity_mat4 ();

    my_mat4 node_T = identity_mat4 ();
    if (node->num_pos_keys > 0) {
        int prev_key = 0;
        int next_key = 0;
        for (int i = 0; i < node->num_pos_keys - 1; i++) {
            prev_key = i;
            next_key = i + 1;
            if (node->pos_key_times[next_key] >= anim_time) {
                break;
            }
        } // endfor
        float total_t = node->pos_key_times[next_key] - node->pos_key_times[prev_key];
        float t = (anim_time - node->pos_key_times[prev_key]) / total_t;
        my_vec3 vi = node->pos_keys[prev_key];
        my_vec3 vf = node->pos_keys[next_key];
        my_vec3 lerped = vi * (1.0f - t) + vf * t;
        node_T = my_translate (identity_mat4 (), lerped);
    } // endif num_pos_keys > 0

    my_mat4 node_R = identity_mat4 ();
    if (node->num_rot_keys > 0) {
        // find next and previous keys
        int prev_key = 0;
        int next_key = 0;
        for (int i = 0; i < node->num_rot_keys - 1; i++) {
            prev_key = i;
            next_key = i + 1;
            if (node->rot_key_times[next_key] >= anim_time) break;
        }
        float total_t = node->rot_key_times[next_key] - node->rot_key_times[prev_key];
        float t = (anim_time - node->rot_key_times[prev_key]) / total_t;
        versor qi = node->rot_keys[prev_key];
        versor qf = node->rot_keys[next_key];
        versor slerped = slerp (qi, qf, t);
        node_R = quat_to_mat4 (slerped);
    } // endif num_rot_keys > 0

    local_anim = node_T * node_R;

    // if node has a weighted bone...
    int bone_i = node->bone_index;
    if (bone_i > -1) {
        // ... then get offset matrices
        my_mat4 bone_offset = bone_offset_mats[bone_i];
        // no more inverse
        our_mat = parent_mat * local_anim;
        bone_animation_mats[bone_i] = parent_mat * local_anim * bone_offset;
    } // endif
    for (int i = 0; i < node->num_children; i++) {
        skeleton_animate (
            node->children[i],
            anim_time,
            our_mat,
            bone_offset_mats,
            bone_animation_mats
    );
    } // endfor
}


/* load a mesh using the assimp library */
void load_mesh( const char* file_name, GLuint* vao, int* point_count, my_mat4* bone_offset_mats, int* bone_count, Skeleton_Node** root_node, double* anim_duration ) {
  const aiScene* scene = aiImportFile( file_name, aiProcess_Triangulate );
  if ( !scene ) {
    fprintf( stderr, "ERROR: reading mesh %s\n", file_name );
    return;
  }
  printf( "  %i animations\n", scene->mNumAnimations );
  printf( "  %i cameras\n", scene->mNumCameras );
  printf( "  %i lights\n", scene->mNumLights );
  printf( "  %i materials\n", scene->mNumMaterials );
  printf( "  %i meshes\n", scene->mNumMeshes );
  printf( "  %i textures\n", scene->mNumTextures );

  /* get first mesh in file only */
  const aiMesh* mesh = scene->mMeshes[0];
  printf( "    %i vertices in mesh[0]\n", mesh->mNumVertices );

  /* pass back number of vertex points in mesh */
  *point_count = mesh->mNumVertices;

  /* generate a VAO, using the pass-by-reference parameter that we give to the
  function */
  glGenVertexArrays( 1, vao );
  glBindVertexArray( *vao );

  /* we really need to copy out all the data from AssImp's funny little data
  structures into pure contiguous arrays before we copy it into data buffers
  because assimp's texture coordinates are not really contiguous in memory.
  i allocate some dynamic memory to do this. */
  GLfloat* points    = NULL; // array of vertex points
  GLfloat* normals   = NULL; // array of vertex normals
  GLfloat* texcoords = NULL; // array of texture coordinates
  GLint* bone_ids    = NULL; // array of bone IDs
  if ( mesh->HasPositions() ) {
    points = (GLfloat*)malloc( *point_count * 3 * sizeof( GLfloat ) );
    for ( int i = 0; i < *point_count; i++ ) {
      const aiVector3D* vp = &( mesh->mVertices[i] );
      points[i * 3]        = (GLfloat)vp->x;
      points[i * 3 + 1]    = (GLfloat)vp->y;
      points[i * 3 + 2]    = (GLfloat)vp->z;
    }
  }
  if ( mesh->HasNormals() ) {
    normals = (GLfloat*)malloc( *point_count * 3 * sizeof( GLfloat ) );
    for ( int i = 0; i < *point_count; i++ ) {
      const aiVector3D* vn = &( mesh->mNormals[i] );
      normals[i * 3]       = (GLfloat)vn->x;
      normals[i * 3 + 1]   = (GLfloat)vn->y;
      normals[i * 3 + 2]   = (GLfloat)vn->z;
    }
  }
  if ( mesh->HasTextureCoords( 0 ) ) {
    texcoords = (GLfloat*)malloc( *point_count * 2 * sizeof( GLfloat ) );
    for ( int i = 0; i < *point_count; i++ ) {
      const aiVector3D* vt = &( mesh->mTextureCoords[0][i] );
      texcoords[i * 2]     = (GLfloat)vt->x;
      texcoords[i * 2 + 1] = (GLfloat)vt->y;
    }
  }

  /* extract bone weights */
  if ( mesh->HasBones() ) {
    *bone_count = (int)mesh->mNumBones;
    /* an array of bones names. max 256 bones, max name length 64 */
    char bone_names[256][64];

    /* here I allocate an array of per-vertex bone IDs.
    each vertex must know which bone(s) affect it
    here I simplify, and assume that only one bone can affect each vertex,
    so my array is only one-dimensional
    */
    bone_ids = (int*)malloc( *point_count * sizeof( int ) );

    for ( int b_i = 0; b_i < *bone_count; b_i++ ) {
      const aiBone* bone = mesh->mBones[b_i];

      /* get bone names */
      strcpy( bone_names[b_i], bone->mName.data );
      printf( "bone_names[%i]=%s\n", b_i, bone_names[b_i] );

      /* get [inverse] offset matrix for each bone */
      bone_offset_mats[b_i] = convert_assimp_matrix( bone->mOffsetMatrix );

      /* get bone weights
      we can just assume weight is always 1.0, because we are just using 1 bone
      per vertex. but any bone that affects a vertex will be assigned as the
      vertex' bone_id */
      int num_weights = (int)bone->mNumWeights;
      for ( int w_i = 0; w_i < num_weights; w_i++ ) {
        aiVertexWeight weight = bone->mWeights[w_i];
        int vertex_id         = (int)weight.mVertexId;
        // ignore weight if less than 0.5 factor
        if ( weight.mWeight >= 0.5f ) { bone_ids[vertex_id] = b_i; }
      }

    } // endfor

    /* get the skeleton hierarchy from a separate AssImp data structure */

    // there should always be a 'root node', even if no skeleton exists
    aiNode* assimp_node = scene->mRootNode;

    if ( !import_skeleton_node( assimp_node, root_node, *bone_count, bone_names ) ) { fprintf( stderr, "ERROR: could not import node tree from mesh\n" ); } // endif

    /* get the first animation out and into keys */
    if ( scene->mNumAnimations > 0 ) {
      // get just the first animation
      aiAnimation* anim = scene->mAnimations[0];
      printf( "animation name: %s\n", anim->mName.C_Str() );
      printf( "animation has %i node channels\n", anim->mNumChannels );
      printf( "animation has %i mesh channels\n", anim->mNumMeshChannels );
      printf( "animation duration %f\n", anim->mDuration );
      printf( "ticks per second %f\n", anim->mTicksPerSecond );

      *anim_duration = anim->mDuration;
      printf( "anim duration is %f\n", anim->mDuration );

      // get the node channels
      for ( int i = 0; i < (int)anim->mNumChannels; i++ ) {
        aiNodeAnim* chan = anim->mChannels[i];
        // find the matching node in our skeleton by name
        Skeleton_Node* sn = find_node_in_skeleton( *root_node, chan->mNodeName.C_Str() );
        if ( !sn ) {
          fprintf( stderr,
            "WARNING: did not find node named %s in skeleton."
            "animation broken.\n",
            chan->mNodeName.C_Str() );
          continue;
        }

        sn->num_pos_keys = chan->mNumPositionKeys;
        sn->num_rot_keys = chan->mNumRotationKeys;
        sn->num_sca_keys = chan->mNumScalingKeys;

        // allocate memory
        sn->pos_keys      = (my_vec3*)malloc( sizeof( my_vec3 ) * sn->num_pos_keys );
        sn->rot_keys      = (versor*)malloc( sizeof( versor ) * sn->num_rot_keys );
        sn->sca_keys      = (my_vec3*)malloc( sizeof( my_vec3 ) * sn->num_sca_keys );
        sn->pos_key_times = (double*)malloc( sizeof( double ) * sn->num_pos_keys );
        sn->rot_key_times = (double*)malloc( sizeof( double ) * sn->num_rot_keys );
        sn->sca_key_times = (double*)malloc( sizeof( double ) * sn->num_sca_keys );

        // add position keys to node
        for ( int i = 0; i < sn->num_pos_keys; i++ ) {
          aiVectorKey key      = chan->mPositionKeys[i];
          sn->pos_keys[i].v[0] = key.mValue.x;
          sn->pos_keys[i].v[1] = key.mValue.y;
          sn->pos_keys[i].v[2] = key.mValue.z;
          // TODO -- forgot this
          sn->pos_key_times[i] = key.mTime;
        }
        // add rotation keys to node
        for ( int i = 0; i < sn->num_rot_keys; i++ ) {
          aiQuatKey key        = chan->mRotationKeys[i];
          sn->rot_keys[i].q[0] = key.mValue.w;
          sn->rot_keys[i].q[1] = key.mValue.x;
          sn->rot_keys[i].q[2] = key.mValue.y;
          sn->rot_keys[i].q[3] = key.mValue.z;
          sn->rot_key_times[i] = key.mTime;
        }
        // add scaling keys to node
        for ( int i = 0; i < sn->num_sca_keys; i++ ) {
          aiVectorKey key      = chan->mScalingKeys[i];
          sn->sca_keys[i].v[0] = key.mValue.x;
          sn->sca_keys[i].v[1] = key.mValue.y;
          sn->sca_keys[i].v[2] = key.mValue.z;
          sn->sca_key_times[i] = key.mTime;
        } // endfor
      }   // endfor mNumChannels
    } else {
      fprintf( stderr, "WARNING: no animations found in mesh file\n" );
    } // endif mNumAnimations > 0

  } // endif hasbones

  /* copy mesh data into VBOs */
  if ( mesh->HasPositions() ) {
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, 3 * *point_count * sizeof( GLfloat ), points, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 0 );
    free( points );
  }
  if ( mesh->HasNormals() ) {
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, 3 * *point_count * sizeof( GLfloat ), normals, GL_STATIC_DRAW );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 1 );
    free( normals );
  }
  if ( mesh->HasTextureCoords( 0 ) ) {
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, 2 * *point_count * sizeof( GLfloat ), texcoords, GL_STATIC_DRAW );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
    glEnableVertexAttribArray( 2 );
    free( texcoords );
  }
  if ( mesh->HasTangentsAndBitangents() ) {
    // NB: could store/print tangents here
  }
  if ( mesh->HasBones() ) {
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, *point_count * sizeof( GLint ), bone_ids, GL_STATIC_DRAW );
    glVertexAttribIPointer( 3, 1, GL_INT, 0, NULL );
    glEnableVertexAttribArray( 3 );
    free( bone_ids );
  }

  aiReleaseImport( scene );
  printf( "mesh loaded\n" );
}

Animation::Animation(const char*  path){
    for ( int i = 0; i < MAX_BONES; i++ ) {
        bone_animation_mats[i]  = identity_mat4();
        bone_offset_matrices[i] = identity_mat4();
    }
    load_mesh(
        path, 
        &vao, 
        &point_count, 
        bone_offset_matrices, 
        &bone_count, 
        &root_node,
        &anim_duration);
}

Animation::~Animation(){
    glDeleteBuffers(1, &vao);
}

void Animation::loadTexture(const std::string& filename){
    if (filename.length() == 0) return;
    Texture = loadSOIL(filename.c_str());
}

void Animation::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, Texture);
}

void Animation::bind(){
    glBindVertexArray( vao );
}
void Animation::draw(){
    glDrawArrays( GL_LINES, 0, point_count );
}

void Animation::update(){
    float size = 1.0f;
    mat4 Translate = glm::translate(mat4(), vec3(10.0f,3.0f,10.0f));
    mat4 Rotate = glm::rotate(mat4(),radians(-90.0f),vec3(0.0f,0.0f,1.0f));
    // modelMatrix = Translate * Rotate * Scaling;
    // mat4 Scaling = glm::scale(mat4(), vec3(1.0f,1.0f,size));
    mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
    modelMatrix = Scaling;
}




