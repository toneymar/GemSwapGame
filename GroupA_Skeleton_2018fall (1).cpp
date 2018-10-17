
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <\Users\marty\Desktop\GL\glew.h>		// must be downloaded 
#include <\Users\marty\Desktop\GL\freeglut.h>	// must be downloaded unless you have an Apple
#endif

const unsigned int windowWidth = 512, windowHeight = 512;

// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;

void getErrorInfo(unsigned int handle) 
{
	int logLen;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) 
	{
		char * log = new char[logLen];
		int written;
		glGetShaderInfoLog(handle, logLen, &written, log);
		printf("Shader log:\n%s", log);
		delete log;
	}
}

// check if shader could be compiled
void checkShader(unsigned int shader, const char * message) 
{
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) 
	{
		printf("%s!\n", message);
		getErrorInfo(shader);
	}
}

// check if shader could be linked
void checkLinking(unsigned int program) 
{
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) 
	{
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
	}
}

// vertex shader in GLSL
/*const char *vertexSource = R"( 
	#version 130 
	precision highp float; 
	
	in vec2 vertexPosition;		// variable input from Attrib Array selected by glBindAttribLocation 
	uniform vec3 vertexColor;
	uniform mat4 M;
	out vec3 color;			// output attribute 
	
	void main() 
	{ 
		color = vertexColor;				 		// set vertex color 
		gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1) * M;  	// copy position from input to output 
	} 
)";*/ 

// fragment shader in GLSL
/*const char *fragmentSource = R"( 
	#version 130 
	precision highp float; 
	
	in vec3 color;			// variable input: interpolated from the vertex colors
	out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation 
	
	void main() 
	{ 
		fragmentColor = vec4(color, 1); // extend RGB to RGBA 
	} 
)";*/

// row-major matrix 4x4
struct mat4 
{
	float m[4][4];
public:
	mat4() {}
	mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) 
	{
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}

	mat4 operator*(const mat4& right) 
	{
		mat4 result;
		for (int i = 0; i < 4; i++) 
		{
			for (int j = 0; j < 4; j++) 
			{
				result.m[i][j] = 0;
				for (int k = 0; k < 4; k++) result.m[i][j] += m[i][k] * right.m[k][j];
			}
		}
		return result;
	}
	operator float*() { return &m[0][0]; }
};


// 3D point in homogeneous coordinates
struct vec4 
{
	float v[4];

	vec4(float x = 0, float y = 0, float z = 0, float w = 1) 
	{
		v[0] = x; v[1] = y; v[2] = z; v[3] = w;
	}

	vec4 operator*(const mat4& mat) 
	{
		vec4 result;
		for (int j = 0; j < 4; j++) 
		{
			result.v[j] = 0;
			for (int i = 0; i < 4; i++) result.v[j] += v[i] * mat.m[i][j];
		}
		return result;
	}

	vec4 operator+(const vec4& vec) 
	{
		vec4 result(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2], v[3] + vec.v[3]);
		return result;
	}

	vec4 operator*(const float& s)
	{
		vec4 result(v[0] * s, v[1] * s, v[2] * s, v[3] * s);
		return result;
	}
};

// 2D point in Cartesian coordinates
struct vec2 
{
	float x, y;

	vec2(float x = 0.0, float y = 0.0) : x(x), y(y) {}

	vec2 operator+(const vec2& v) 
	{
		return vec2(x + v.x, y + v.y);
	}
};


// handle of the shader program
//unsigned int shaderProgram;

bool keyboardState[256];

void onKeyboardUp(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		keyboardState[97] = false;
	}
	else if (key == 'd')
	{
		keyboardState[100] = false;
	}
	else if (key == 'i')
	{
		keyboardState[105] = false;
	}
	else if (key == 'j')
	{
		keyboardState[106] = false;
	}
	else if (key == 'k')
	{
		keyboardState[107] = false;
	}
	else if (key == 'l')
	{
		keyboardState[108] = false;
	}
	else if (key == 'q')
	{
		keyboardState[81] = false;
	}
	else if (key == 'b')
	{
		keyboardState[98] = false;
	}
	glutPostRedisplay();
}

void onKeyboard(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		keyboardState[97] = true;
	}
	else if (key == 'd')
	{
		keyboardState[100] = true;
	}
	else if (key == 'i')
	{
		keyboardState[105] = true;
	}
	else if (key == 'j')
	{
		keyboardState[106] = true;
	}
	else if (key == 'k')
	{
		keyboardState[107] = true;
	}
	else if (key == 'l')
	{
		keyboardState[108] = true;
	}
	else if (key == 'q')
	{
		keyboardState[81] = true;
	}
	else if (key == 'b')
	{
		keyboardState[98] = true;
	}
	glutPostRedisplay();
}


/*class Shader 
{
protected:
	unsigned int shaderProgram;

	const char *vertexSource = R"( 
	#version 130 
	precision highp float; 
	
	in vec2 vertexPosition;		// variable input from Attrib Array selected by glBindAttribLocation 
	uniform vec3 vertexColor;
	uniform mat4 M;
	out vec3 color;			// output attribute 
	
	void main() 
	{ 
		color = vertexColor;				 		// set vertex color 
		gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1) * M;  	// copy position from input to output 
	} 
)";

	const char *fragmentSource = R"( 
	#version 130 
	precision highp float; 
	
	in vec3 color;			// variable input: interpolated from the vertex colors
	out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation 
	
	void main() 
	{ 
		fragmentColor = vec4(color, 1); // extend RGB to RGBA 
	} 
)";

public:
	Shader()
	{
		// create vertex shader from string
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (!vertexShader) { printf("Error in vertex shader creation\n"); exit(1); }

		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		checkShader(vertexShader, "Vertex shader error");
		// create fragment shader from string
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fragmentShader) { printf("Error in fragment shader creation\n"); exit(1); }

		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);
		checkShader(fragmentShader, "Fragment shader error");

		// attach shaders to a single program
		shaderProgram = glCreateProgram();
		if (!shaderProgram) { printf("Error in shader program creation\n"); exit(1); }

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		// connect Attrib Array to input variables of the vertex shader
		glBindAttribLocation(shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0

		// connect the fragmentColor to the frame buffer memory
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor"); // fragmentColor goes to the frame buffer memory

		// program packaging
		glLinkProgram(shaderProgram);
		checkLinking(shaderProgram);
	}
	void Run()
	{
		glUseProgram(shaderProgram);
	}
	virtual void UploadColor(vec4 color)
	{
		int location = glGetUniformLocation(shaderProgram, "vertexColor");
		if (location >= 0) glUniform3fv(location, 1, &color.v[0]); // set uniform variable vertexColor 
		else printf("uniform vertex color cannot be set\n");
	}
	void UploadM(mat4 M)
	{
		int location = glGetUniformLocation(shaderProgram, "M");
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, M);
		else printf("uniform M cannot be set\n");
	}

	virtual void UploadSamplerID()
	{

	}

	~Shader()
	{
		glDeleteProgram(shaderProgram);
	}
};*/

class Shader
{
protected:
	unsigned int shaderProgram;

public:
	Shader()
	{
		shaderProgram = 0;
	}
	void CompileProgram(const char *vertexSource, const char *fragmentSource) 
	{
		// create vertex shader from string
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (!vertexShader) { printf("Error in vertex shader creation\n"); exit(1); }

		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		checkShader(vertexShader, "Vertex shader error");

		// create fragment shader from string
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (!fragmentShader) { printf("Error in fragment shader creation\n"); exit(1); }

		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);
		checkShader(fragmentShader, "Fragment shader error");

		// attach shaders to a single program
		shaderProgram = glCreateProgram();
		if (!shaderProgram) { printf("Error in shader program creation\n"); exit(1); }

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
	}

	void LinkProgram()
	{
		// program packaging
		glLinkProgram(shaderProgram);
		checkLinking(shaderProgram);
	}

	void Run()
	{
		glUseProgram(shaderProgram);
	}
	virtual void UploadColor(vec4 color)
	{

	}
	void UploadM(mat4 M)
	{
		int location = glGetUniformLocation(shaderProgram, "M");
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, M);
		else printf("uniform M cannot be set\n");
	}

	virtual void UploadSamplerID()
	{

	}

	~Shader()
	{
		glDeleteProgram(shaderProgram);
	}
};

class StandardShader : public Shader
{
public: 
	StandardShader()
	{
		const char *vertexSource = R"( 
	#version 130 
	precision highp float; 
	
	in vec2 vertexPosition;		// variable input from Attrib Array selected by glBindAttribLocation 
	uniform vec3 vertexColor;
	uniform mat4 M;
	out vec3 color;			// output attribute 
	
	void main() 
	{ 
		color = vertexColor;				 		// set vertex color 
		gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1) * M;  	// copy position from input to output 
	} 
)";

		const char *fragmentSource = R"( 
	#version 130 
	precision highp float; 
	
	in vec3 color;			// variable input: interpolated from the vertex colors
	out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation 
	
	void main() 
	{ 
		fragmentColor = vec4(color, 1); // extend RGB to RGBA 
	} 
)";

		CompileProgram(vertexSource, fragmentSource);

		// connect Attrib Array to input variables of the vertex shader
		glBindAttribLocation(shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0

		// connect the fragmentColor to the frame buffer memory
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor"); // fragmentColor goes to the frame buffer memory

		LinkProgram();

	}

	void UploadColor(vec4 color)
	{
		int location = glGetUniformLocation(shaderProgram, "vertexColor");
		if (location >= 0) glUniform3fv(location, 1, &color.v[0]); // set uniform variable vertexColor 
		else printf("uniform vertex color cannot be set\n");
	}
};

class TexturedShader : public Shader
{
public:
	TexturedShader()
	{
		const char *vertexSource = R"(
		#version 130
			precision highp float;

		in vec2 vertexPosition;
		in vec2 vertexTexCoord;
		uniform mat4 M;
		out vec2 texCoord;

		void main()
		{
			texCoord = vertexTexCoord;
			gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1) * M;
		}
	)";

		const char *fragmentSource = R"(
		#version 130
		precision highp float;

		uniform sampler2D samplerUnit;
		in vec2 texCoord;
		out vec4 fragmentColor;

		void main()
		{
			fragmentColor = texture(samplerUnit, texCoord);
		}
      )";

		CompileProgram(vertexSource, fragmentSource);

		glBindAttribLocation(shaderProgram, 0, "vertexPosition");
		glBindAttribLocation(shaderProgram, 1, "vertexTexCoord");
		glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

		LinkProgram();
	}

	void UploadSamplerID()
	{
		int samplerUnit = 0;
		int location = glGetUniformLocation(shaderProgram, "samplerUnit");
		glUniform1i(location, samplerUnit);
		glActiveTexture(GL_TEXTURE0 + samplerUnit);
	}
};

class Camera
{
	vec2 center;
	vec2 size;
	float orientation;
public:
	void Create(vec2 c, vec2 s, float o)
	{
		center = c;
		size = s;
		orientation = o;
	}

	float alpha = orientation / 180.0 * M_PI;

	mat4 GetViewTransformationMatrix()
	{

		float alpha = orientation / 180.0 * M_PI;
		mat4 translation = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -center.x, -center.y, 0, 1);
		mat4 scaling = mat4((1 / size.x), 0, 0, 0, 0, (1 / size.y), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		mat4 rotation = mat4(cos(alpha), sin(alpha), 0.0, 0.0,
			-sin(alpha), cos(alpha), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		return translation * scaling * rotation;
	}

	mat4 GetInverseViewTransformationMatrix()
	{
		float alpha = orientation / 180.0 * M_PI;
		mat4 translationInv = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, center.x, center.y, 0, 1);
		mat4 scalingInv = mat4(size.x, 0, 0, 0, 0, size.y, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
		mat4 rotationInv = mat4(cos(alpha), -sin(alpha), 0.0, 0.0,
			sin(alpha), cos(alpha), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);
		return translationInv * scalingInv * rotationInv;
	}

	void move()
	{
		if (keyboardState[105])
		{
			center.y += -0.001;
		}
		else if (keyboardState[97])
		{
			orientation += 0.5;
		}
		else if (keyboardState[107])
		{
			center.y += 0.001;
		}
		else if (keyboardState[100])
		{
			orientation += -0.5;
		}
		else if (keyboardState[106])
		{
			center.x += -0.001;
		}
		else if (keyboardState[108])
		{
			center.x += 0.001;
		}
	}

	void Quake() {
		if (keyboardState[81]) {
			float radius = 0.01;
			float angle = (rand() % 360) * M_PI / 180;
			vec2 change = vec2(sin(angle) * radius, cos(angle) * radius);
			center = center + change;
			while (radius > 0) {
				radius = radius - 0.01;
				angle = (angle + (150 + rand() % 60)) * M_PI / 180;
				change = vec2(-sin(angle) * radius, -cos(angle) * radius);
				center = center + change;
				change = vec2(sin(angle) * radius, cos(angle) * radius);
				center = center + change;
			}
		}
		else {
			Reset();
		}
	}

	void Reset() {
		center = vec2(0.45, 0.45);
	}
};

Camera camera;

//Elapsed time
double T = 0.0;
double DT = 0.0;



extern "C" unsigned char* stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);

class Texture {
	unsigned int textureId;
public:
	Texture(const std::string& inputFileName) {
		unsigned char* data;
		int width; int height; int nComponents = 4;

		data = stbi_load(inputFileName.c_str(), &width, &height, &nComponents, 0);

		if (data == NULL) { return; }

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		delete data;
	}

	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
};



class Material
{
protected:
	Shader * shader;
	vec4 color;
	Texture* texture;

public:
	Material(Shader * s, vec4 c, Texture* t = 0)
	{
		shader = s;
		color = c;
		texture = t;
	}
	virtual void UploadAttributes()
	{
		if (texture)
		{
			shader->UploadSamplerID();
			texture->Bind();
		}
		else
			shader->UploadColor(color);
	}

	virtual int getType()
	{
		return 0;
	}

	~Material()
	{

	}
};

class AnimatedMaterial : public Material
{
public:

	AnimatedMaterial(Shader * s, vec4 c) : Material(s, c)
	{
	}

	void UploadAttributes()
	{
		float intensity = (sin(T) + 1) / 2.0;
		shader->UploadColor(color * intensity);
	}

	int getType()
	{
		return 10;
	}
};

class Geometry 
{
protected: unsigned int vao;

public:
	Geometry()
	{
		glGenVertexArrays(1, &vao);
	}

	virtual void Draw() = 0;

	virtual int getType()
	{
		return 0;
	}

	~Geometry()
	{

	}
};

class Mesh
{
	Material * material;
	Geometry * geometry;

public:
	Mesh(Geometry * g, Material * m)
	{
		material = m;
		geometry = g;
	}
	void Draw()
	{
		material->UploadAttributes();
		geometry->Draw();
	}
	int getType()
	{
		return geometry->getType() + material->getType();
	}
	~Mesh()
	{

	}

};

class Object
{
protected:
	Shader * shader;
	Mesh * mesh;
	vec2 position;
	vec2 scaling;
	float orientation;

public:
	Object(Shader * s, Mesh * m, vec2 p, vec2 sc, float o)
	{
		shader = s;
		mesh = m;
		position = p;
		scaling = sc;
		orientation = o;
	}

	virtual void UploadAttributes()
	{
		mat4 T = mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			position.x, position.y, 0.0, 1.0);

		mat4 S = mat4(
			scaling.x, 0.0, 0.0, 0.0,
			0.0, scaling.y, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		float alpha = orientation / 180.0 * M_PI;

		mat4 R = mat4(
			cos(alpha), sin(alpha), 0.0, 0.0,
			-sin(alpha), cos(alpha), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		mat4 V = camera.GetViewTransformationMatrix();

		mat4 M = S * R * T * V; // scaling, rotation, and translation

		shader->UploadM(M);
	}

	void Draw()
	{
		shader->Run();
		UploadAttributes();
		mesh->Draw();
	}

	vec2 GetPosition() { 
		return position; 
	}

	void SetPosition(vec2 pos) {
		position = pos;
	}

	virtual int getType()
	{
		return mesh->getType();
	}

	~Object()
	{

	}

};

class RotatingObject : public Object
{
public:
	RotatingObject(Shader * s, Mesh * m, vec2 p, vec2 sc, float o) : Object(s, m, p, sc, o) {}

	void UploadAttributes() 
	{
		mat4 T = mat4(
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			position.x, position.y, 0.0, 1.0);

		mat4 S = mat4(
			scaling.x, 0.0, 0.0, 0.0,
			0.0, scaling.y, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		orientation += (45 * DT);
		float alpha = orientation / 180.0 * M_PI;

		mat4 R = mat4(
			cos(alpha), sin(alpha), 0.0, 0.0,
			-sin(alpha), cos(alpha), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0);

		mat4 V = camera.GetViewTransformationMatrix();

		mat4 M = S * R * T * V; // scaling, rotation, and translation

		shader->UploadM(M);
	}

	int getType()
	{
		return mesh->getType() + 20;
	}
	
};

class Star : public Geometry
{
	int type = 4;
	unsigned int vbo;

public:
	Star()
	{
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		float R = 1.0;
		float r = R * cos(2 * M_PI / 5) / cos(M_PI / 5);
		static float vertexCoords[24];

		vertexCoords[0] = 0.0;
		vertexCoords[1] = 0.0;

		double phi = 0.0;

		for (int i = 1; i < 12; i++) {
			float a = phi * (M_PI / 180);
			if (i % 2 != 0) {
				vertexCoords[i * 2] = R * sin(a);
				vertexCoords[i * 2 + 1] = R * cos(a);
			}
			else {
				vertexCoords[i * 2] = r * sin(a);
				vertexCoords[i * 2 + 1] = r * cos(a);
			}
			phi += 36.0;
		}


		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
	}

	int getType()
	{
		return type;
	}
};

class Heart : public Geometry
{
	int type = 3;
	unsigned int vbo;

public:
	Heart()
	{
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		static float vertexCoords[100];

		float t = -1 * M_PI;
		double change = M_PI / 20;

		vertexCoords[0] = 0;
		vertexCoords[1] = 0;

		for (int i = 2; i < 100; i += 2) {
			vertexCoords[i] = (16 * pow(sin(t), 3)) * 0.05;
			vertexCoords[i + 1] = (13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t)) * 0.05;
			t += change;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 50);
	}

	int getType()
	{
		return type;
	}
};


class Quad : public Geometry
{
	int type = 2;
	unsigned int vbo;

public:
	Quad()
	{
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		static float vertexCoords[] = { 0.7, 0.7, -0.7, 0.7, 0.7, -0.7, -0.7, -0.7 };
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	int getType()
	{
		return type;
	}
};

class TexturedQuad : public Quad
{
	unsigned int vboTex;

public:
	TexturedQuad()
	{
		glBindVertexArray(vao);
		glGenBuffers(1, &vboTex);

		// define the texture coordinates here
		glBindBuffer(GL_ARRAY_BUFFER, vboTex);
		static float vertexTexCoords[] = { 0,0  ,1,0  ,0,1  ,1,1 };
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexTexCoords), vertexTexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		// assign the array of texture coordinates to 
		// vertex attribute array 1
	}

	void Draw()
	{
		glEnable(GL_BLEND); // necessary for transparent pixels
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisable(GL_BLEND);
	}
};

class Triangle : public Geometry
{
	int type = 1;
	unsigned int vbo;	// vertex array object id

public:	
	Triangle()
	{

		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		static float vertexCoords[] = { 0, 0.8, -0.8, -0.8, 0.8, -0.8};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	}
	
	void Draw() 
	{
		glBindVertexArray(vao);	// make the vao and its vbos active playing the role of the data source
		glDrawArrays(GL_TRIANGLES, 0, 3); // draw a single triangle with vertices defined in vao
	}

	int getType()
	{
		return type;
	}

};



// Global vars
/*Triangle triangle0;
Triangle triangle1;
Triangle triangle2;*/

class Scene {
	StandardShader* shader;
	TexturedShader* textureShader;
	std::vector<Material*> materials;
	std::vector<Geometry*> geometries;
	std::vector<Mesh*> meshes;
	std::vector<Object*> objects;
	std::vector<Texture*> textures;
	Object* grid[10][10];
	int x;
	int y;
	Texture* asteroid;
	Texture* fireball;
public:
	Scene() 
	{ 
		shader = 0;
		textureShader = 0;
	}

	void Initialize() {
		
		shader = new StandardShader();
		textureShader = new TexturedShader();
		/*textureShader = new TexturedShader();
		textures.push_back(new Texture("asteroid.png"));

		materials.push_back(new Material(textureShader, vec4(1, 0, 0), textures[0]));

		geometries.push_back(new TexturedQuad());

		meshes.push_back(new Mesh(geometries[0], materials[0]));*/

		asteroid = new Texture("/Users/marty/Desktop/Project2/GemSwapGame/sprites/asteroid.png");
		fireball = new Texture("/Users/marty/Desktop/Project2/GemSwapGame/sprites/fireball.png");

		materials.push_back(new Material(shader, vec4(1, 0, 0)));
		materials.push_back(new Material(shader, vec4(0, 1, 0)));
		materials.push_back(new Material(shader, vec4(0, 0, 1)));
		materials.push_back(new Material(shader, vec4(0, 1, 1)));
		materials.push_back(new AnimatedMaterial(shader, vec4(1, 0, 0)));
		materials.push_back(new Material(textureShader, vec4(0, 0, 0), asteroid));
		materials.push_back(new Material(textureShader, vec4(0, 0, 0), fireball));

		geometries.push_back(new Triangle());
		geometries.push_back(new Star());
		geometries.push_back(new Heart());
		geometries.push_back(new Quad());
		geometries.push_back(new Heart());
		geometries.push_back(new TexturedQuad());

		meshes.push_back(new Mesh(geometries[0], materials[2]));
		meshes.push_back(new Mesh(geometries[1], materials[1]));
		meshes.push_back(new Mesh(geometries[2], materials[0]));
		meshes.push_back(new Mesh(geometries[3], materials[3]));
		meshes.push_back(new Mesh(geometries[4], materials[4]));
		meshes.push_back(new Mesh(geometries[5], materials[5]));
		meshes.push_back(new Mesh(geometries[5], materials[6]));


		/*objects.push_back(new Object(shader, meshes[0], vec2(1,-.25), vec2(0.5, 0.5), 0));
		objects.push_back(new Object(shader, meshes[1], vec2(0, 0), vec2(0.5, 0.5), 0));
		objects.push_back(new Object(shader, meshes[1], vec2(0, 0), vec2(0.05, 0.05), 0));*/

		/*for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				int random = rand() % 5;
				if (random == 1) 
				{
					grid[i][j] = new RotatingObject(shader, meshes[1], vec2((float)i / 10, (float)j / 10), vec2(0.05, 0.05), 0);
				}
				else
				{
					grid[i][j] = new Object(shader, meshes[random], vec2((float)i / 10.0, (float)j / 10.0), vec2(0.05, 0.05), 0);
				}
			}
		}*/

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				int random = rand() % 7;
				if (random == 6)
				{
					grid[i][j] = new Object(textureShader, meshes[6], vec2((float)i / 10, (float)j / 10), vec2(0.05, 0.05), 0);
				}
				else if (random == 5)
				{
					grid[i][j] = new Object(textureShader, meshes[5], vec2((float)i / 10, (float)j / 10), vec2(0.05, 0.05), 0);
				}
				else if (random == 1)
				{
					grid[i][j] = new RotatingObject(shader, meshes[1], vec2((float)i / 10, (float)j / 10), vec2(0.05, 0.05), 0);
				}
				else
				{
					grid[i][j] = new Object(shader, meshes[random], vec2((float)i / 10.0, (float)j / 10.0), vec2(0.05, 0.05), 0);
				}
			}
		}

		//shader->Run();
		//textureShader->Run();
	}

	void Select(int u, int v) 
	{
		x = u;
		y = v;
	}

	void Swap(int u, int v) 
	{
		vec2 pos1 = grid[u][v]->GetPosition();
		vec2 pos2 = grid[x][y]->GetPosition();
		grid[u][v]->SetPosition(pos2);
		grid[x][y]->SetPosition(pos1);
		std::swap(grid[u][v], grid[x][y]);
	}

	void legal(int u, int v)
	{
		int type = grid[x][y]->getType();

		if (v + 2 <= 9 && v - 2 >= 0 && u + 2 <= 9 && u - 2 >= 0)
		{
			if (type == grid[u][v + 1]->getType() && type == grid[u][v + 2]->getType() || 
				type == grid[u][v - 1]->getType() && type == grid[u][v - 2]->getType() ||
				type == grid[u + 1][v]->getType() && type == grid[u + 2][v]->getType() ||
				type == grid[u - 1][v]->getType() && type == grid[u - 2][v]->getType())
			{
				Swap(u, v);
			}

			else if (v + 1 <= 9 && v - 1 >= 0 && u + 1 <= 9 && u - 1 >= 0)
			{
				if (type == grid[u][v + 1]->getType() && type == grid[u][v - 1]->getType() ||
					type == grid[u + 1][v]->getType() && type == grid[u - 1][v]->getType())
				{
					Swap(u, v);
				}
			}
			
		}

		/*if (v + 1 <= 9 && v - 1 >= 0 && u + 1 <= 9 && u - 1 >= 0)
		{
			if (type == grid[u][v + 1]->getType() && type == grid[u][v - 1]->getType() ||
				type == grid[u + 1][v]->getType() && type == grid[u - 1][v]->getType())
			{
				Swap(u, v);
			}
		}*/
	}

	void Bomb(int u, int v) 
	{
		if (keyboardState[98])
			grid[u][v]->SetPosition(vec2(1000, 1000));
	}
	
	void QuakeGone()
	{
		if (keyboardState[81])
		{
			int i = rand() % 10;
			int j = rand() % 10;
			int r = rand() % 1000;
			if (r == 1)
			{
				grid[i][j]->SetPosition(vec2(200, 200));
			}
		}
	}

	~Scene() {
		for (int i = 0; i < materials.size(); i++) delete materials[i];
		for (int i = 0; i < geometries.size(); i++) delete geometries[i];
		for (int i = 0; i < meshes.size(); i++) delete meshes[i];
		for (int i = 0; i < objects.size(); i++) delete objects[i];
		if (shader) delete shader;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				delete grid[i][j];
			}
		}
	}

	void Draw()
	{
		//for (int i = 0; i < objects.size(); i++) objects[i]->Draw();
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				grid[i][j]->Draw();
			}
		}
	}
};


Shader * gShader = 0;
Material * gMaterial = 0;
Mesh * gMesh = 0;
Geometry * gGeometry = 0;
Object * gObject = 0;
Scene scene;

// initialization, create an OpenGL context
void onInitialization()
{
	glViewport(0, 0, windowWidth, windowHeight);

	camera.Create({ 0.45,0.45 }, { 0.5,0.5 }, 0);

	scene.Initialize();

}

void onIdle()
{
	double t = glutGet(GLUT_ELAPSED_TIME) * 0.001;
	T = t;
	static double lastTime = 0.0;
	double dt = t - lastTime;
	DT = dt;
	lastTime = t;
	camera.Quake();
	scene.QuakeGone();
	camera.move();
	glutPostRedisplay();
}

void onMouse(int button, int state, int i, int j) {

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float x = ((float)i / viewport[2]) * 10.0;
	float y = ((float)j / viewport[3]) * 10.0;

	int u = (int)floor(x);
	int v = 9 - (int)floor(y);

	if ((u < 0) || (u > 9) || (v < 0) || (v > 9)) return;

	if (state == GLUT_DOWN)
		scene.Select(u, v);
	else if (state == GLUT_UP)
	{
		scene.legal(u, v);
		//scene.Swap(u, v);
		scene.Bomb(u, v);
	}
	
}


void onDisplay()
{
	glClearColor(0, 0, 0, 0); // background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen

	scene.Draw();

	glutSwapBuffers(); // exchange the two buffers
}

void onExit()	
{
	//glDeleteProgram(shaderProgram);
	delete gObject;
	delete gGeometry;
	delete gMesh;
	delete gMaterial;
	delete gShader;
	printf("exit");
}

// window has become invalid: redraw



int main(int argc, char * argv[]) 
{
	glutInit(&argc, argv);
#if !defined(__APPLE__)
	glutInitContextVersion(majorVersion, minorVersion);
#endif
	glutInitWindowSize(windowWidth, windowHeight); 	// application window is initially of resolution 512x512
	glutInitWindowPosition(50, 50);			// relative location of the application window
#if defined(__APPLE__)
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);  // 8 bit R,G,B,A + double buffer + depth buffer
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutCreateWindow("Triangle Rendering");

#if !defined(__APPLE__)
	glewExperimental = true;	
	glewInit();
#endif
	printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	printf("GL Version (integer) : %d.%d\n", majorVersion, minorVersion);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	onInitialization();

	glutDisplayFunc(onDisplay);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMouseFunc(onMouse);

	glutMainLoop();
	onExit();
	return 1;
}

