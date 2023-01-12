/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Dr Anton Gerdelan, Trinity College Dublin, Ireland.                          |
| See individual libraries' separate legal notices                             |
|******************************************************************************|
| Commonly-used maths structures and functions                                 |
| Simple-as-possible. No disgusting templates.                                 |
| Structs vec3, mat4, versor. just hold arrays of floats called "v","m","q",   |
| respectively. So, for example, to get values from a mat4 do: my_mat.m        |
| A versor is the proper name for a unit quaternion.                           |
| This is C++ because it's sort-of convenient to be able to use maths operators|
\******************************************************************************/
#ifndef _MATHS_FUNCS_H_
#define _MATHS_FUNCS_H_

// const used to convert degrees into radians
#define TAU 2.0 * M_PI
#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / ( 2.0 * M_PI ) // 57.2957795

struct my_vec2;
struct my_vec3;
struct my_vec4;
struct versor;

struct my_vec2 {
  my_vec2();
  my_vec2( float x, float y );
  float v[2];
};

struct my_vec3 {
  my_vec3();
  // create from 3 scalars
  my_vec3( float x, float y, float z );
  // create from vec2 and a scalar
  my_vec3( const my_vec2& vv, float z );
  // create from truncated vec4
  my_vec3( const my_vec4& vv );
  // add vector to vector
  my_vec3 operator+( const my_vec3& rhs );
  // add scalar to vector
  my_vec3 operator+( float rhs );
  // because user's expect this too
  my_vec3& operator+=( const my_vec3& rhs );
  // subtract vector from vector
  my_vec3 operator-( const my_vec3& rhs );
  // add vector to vector
  my_vec3 operator-( float rhs );
  // because users expect this too
  my_vec3& operator-=( const my_vec3& rhs );
  // multiply with scalar
  my_vec3 operator*( float rhs );
  // because users expect this too
  my_vec3& operator*=( float rhs );
  // divide vector by scalar
  my_vec3 operator/( float rhs );
  // because users expect this too
  my_vec3& operator=( const my_vec3& rhs );

  // internal data
  float v[3];
};

struct my_vec4 {
  my_vec4();
  my_vec4( float x, float y, float z, float w );
  my_vec4( const my_vec2& vv, float z, float w );
  my_vec4( const my_vec3& vv, float w );
  float v[4];
};

/* stored like this:
a d g
b e h
c f i */
struct my_mat3 {
  my_mat3();
  my_mat3( float a, float b, float c, float d, float e, float f, float g, float h, float i );
  float m[9];
};

/* stored like this:
0 4 8  12
1 5 9  13
2 6 10 14
3 7 11 15*/
struct my_mat4 {
  my_mat4();
  // note! this is entering components in ROW-major order
  my_mat4( float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float mm, float n, float o, float p );
  my_vec4 operator*( const my_vec4& rhs );
  my_mat4 operator*( const my_mat4& rhs );
  my_mat4& operator=( const my_mat4& rhs );
  float m[16];
};

struct versor {
  versor();
  versor operator/( float rhs );
  versor operator*( float rhs );
  versor operator*( const versor& rhs );
  versor operator+( const versor& rhs );
  float q[4];
};

void print( const my_vec2& v );
void print( const my_vec3& v );
void print( const my_vec4& v );
void print( const my_mat3& m );
void print( const my_mat4& m );
// vector functions
float length( const my_vec3& v );
float length2( const my_vec3& v );
my_vec3 normalise( const my_vec3& v );
float dot( const my_vec3& a, const my_vec3& b );
my_vec3 cross( const my_vec3& a, const my_vec3& b );
float get_squared_dist( my_vec3 from, my_vec3 to );
float direction_to_heading( my_vec3 d );
my_vec3 heading_to_direction( float degrees );
// matrix functions
my_mat3 zero_mat3();
my_mat3 identity_mat3();
my_mat4 zero_mat4();
my_mat4 identity_mat4();
float determinant( const my_mat4& mm );
my_mat4 inverse( const my_mat4& mm );
my_mat4 transpose( const my_mat4& mm );
// affine functions
my_mat4 my_translate( const my_mat4& m, const my_vec3& v );
my_mat4 rotate_x_deg( const my_mat4& m, float deg );
my_mat4 rotate_y_deg( const my_mat4& m, float deg );
my_mat4 rotate_z_deg( const my_mat4& m, float deg );
my_mat4 my_scale( const my_mat4& m, const my_vec3& v );
// camera functions
my_mat4 look_at( const my_vec3& cam_pos, my_vec3 targ_pos, const my_vec3& up );
my_mat4 perspective( float fovy, float aspect, float near, float far );
// quaternion functions
versor quat_from_axis_rad( float radians, float x, float y, float z );
versor quat_from_axis_deg( float degrees, float x, float y, float z );
my_mat4 quat_to_mat4( const versor& q );
float dot( const versor& q, const versor& r );
versor slerp( const versor& q, const versor& r );
// stupid overloading wouldn't let me use const
versor normalise( versor& q );
void print( const versor& q );
versor slerp( versor& q, versor& r, float t );
#endif
