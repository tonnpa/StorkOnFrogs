#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          

#include <iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

#define OBJ_NUM 10
#define PI 3.14
#define POINT_CNT 5
#define U_MAX 20
#define V_MAX 20
#define EPS 0.0001
#define DEBUGGING 0

//--------------------------------------------------------
// 3D Vektor
//--------------------------------------------------------
struct Vector {
	float x, y, z;
	Vector() {
		x = y = z = 0;
	}
	Vector(float x0, float y0, float z0 = 0) {
		x = x0; y = y0; z = z0;
	}
	Vector operator*(float a) {
		return Vector(x * a, y * a, z * a);
	}
	Vector operator/(float a) {
		return Vector(x / a, y / a, z / a);
	}
	Vector operator+(const Vector& v) {
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector& v) {
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	float operator*(const Vector& v) {         // dot product
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) {         // cross product
		return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	float Length() {
		return sqrt(x * x + y * y + z * z);
	}
	Vector normalized(){
		float length = this->Length();
		return Vector(x /= length, y /= length, z /= length);
	}
};

typedef Vector Point;

void glPoint3f(Point p){
	glVertex3f(p.x, p.y, p.z);
}

void glVector3f(Vector v){
	glNormal3f(v.x, v.y, v.z);
}

void draw2DCircle(Point point){
	float radius = 0.2;
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(point.x, point.y, 0);
	for (int i = 0; i <= 360; ++i){
		glVertex3f(point.x + radius*cos(i / 360.0 * 2 * PI), point.y + radius*sin(i / 360.0 * 2 * PI), 0);
	}
	glEnd();
}

//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
	float r, g, b;

	Color() {
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0) {
		r = r0; g = g0; b = b0;
	}
	Color operator*(float a) {
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c) {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
};

struct ModelTransformation{
	float phi;
	Vector scale, rotate, translate;

	ModelTransformation(){
		phi = 0;
		scale = Vector(1, 1, 1);
		rotate = Vector(0, 1, 0);
		translate = Vector(0, 0, 0);
	}

	void setOpenGL(){
		glTranslatef(translate.x, translate.y, translate.z);
		glRotatef(phi, rotate.x, rotate.y, rotate.z);
		glScalef(scale.x, scale.y, scale.z);
	}
};

class CTRSpline{
	float t[POINT_CNT];
	Point p[POINT_CNT];
	Vector v[POINT_CNT];
	Vector a2[POINT_CNT - 1], a3[POINT_CNT - 1];

	int pointCount;
	int functionCount;

public:
	CTRSpline() :pointCount(0), functionCount(POINT_CNT - 1){}

	void addPoint(Point& p, float weight){
		if (pointCount < POINT_CNT){
			this->p[pointCount] = p;
			this->t[pointCount] = weight;
			pointCount++;
		}
	}
	void setup(){
		v[0] = v[pointCount] = Vector(0, 0, 0);
		for (int i = 1; i < pointCount - 1; ++i){
			v[i] = ((p[i + 1] - p[i]) / (t[i + 1] - t[i]) + (p[i] - p[i - 1]) / (t[i] - t[i - 1]))*0.5;
		}

		for (int i = 0; i < functionCount; ++i){
			a2[i] = (p[i + 1] - p[i]) * 3 / powf(t[i + 1] - t[i], 2.0) - (v[i + 1] + v[i] * 2) / (t[i + 1] - t[i]);
			a3[i] = (p[i] - p[i + 1]) * 2 / powf(t[i + 1] - t[i], 3.0) + (v[i + 1] + v[i]) / powf(t[i + 1] - t[i], 2.0);
		}
	}
	Point curvePoint(float u){
		int i = 0;
		for (int j = 0; j < pointCount; ++j){
			if (u > t[j]){
				i = j;
			}
		}
		return a3[i] * powf(u - t[i], 3) + a2[i] * powf(u - t[i], 2) + v[i] * (u - t[i]) + p[i];
	}
	Point curvePointDerivative(float u){
		int i = 0;
		for (int j = 0; j < pointCount; ++j){
			if (u > t[j]){
				i = j;
			}
		}
		return a3[i] * 3 * powf(u - t[i], 2) + a2[i] * 2 * (u - t[i]) + v[i];
	}
	void draw(){
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < U_MAX; ++i){
			Point temp = curvePoint(i);
			glPoint3f(curvePoint(i));
		}
		glEnd();
		for (int i = 0; i < pointCount; ++i){
			draw2DCircle(p[i]);
		}
	}
};

class Material{
	float kd[4], ks[4], ka[4];
	float shininess[1];

public:
	Material(){
		for (int i = 0; i < 3; ++i){
			kd[i] = ks[i] = ka[i] = 0;
		}
		kd[3] = ks[3] = ka[3] = 1;
		shininess[0] = 0;
	}
	void setKd(float r, float g, float b, float o){
		kd[0] = r;
		kd[1] = g;
		kd[2] = b;
		kd[3] = o;
	}
	void setKs(float r, float g, float b, float o, float shine){
		ks[0] = r;
		ks[1] = g;
		ks[2] = b;
		ks[3] = o;
		shininess[0] = shine;
	}
	void setKa(float r, float g, float b, float o){
		ka[0] = r;
		ka[1] = g;
		ka[2] = b;
		ka[3] = o;
	}
	void setOpenGL(){
		glEnable(GL_LIGHTING);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

		glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, ka);
		glMaterialfv(GL_FRONT, GL_SPECULAR, ka);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	};
};

Material* orangeRed;
Material* frogGreen;

class Texture{
	unsigned int texID;
public:
	Texture(unsigned int ID) :texID(ID){}
	void setOpenGL(){
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, texID);
	};
};

class ParamSurface{
	Texture* texture;
	Material* material;

	bool isTextured;
	bool isMaterialized;
public:
	ParamSurface() :texture(false), isMaterialized(false){}

	virtual Point surfacePoint(float u, float v) = 0;
	virtual Vector surfaceNormal(float u, float v) = 0;
	void setTexture(Texture* texture){
		isTextured = true;
		this->texture = texture;
	}
	void setMaterial(Material* material){
		isMaterialized = true;
		this->material = material;
	}
	void draw(){
		if (isTextured){
			texture->setOpenGL();
		}
		else
			glDisable(GL_TEXTURE_2D);
		if (isMaterialized){
			material->setOpenGL();
		}
		else
			glDisable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);
		for (int u = 0; u < U_MAX; ++u){
			for (int v = 0; v < V_MAX; ++v){
				vertexOpenGL(u, v);
				vertexOpenGL(u, v + 1);
				vertexOpenGL(u + 1, v + 1);

				vertexOpenGL(u + 1, v + 1);
				vertexOpenGL(u + 1, v);
				vertexOpenGL(u, v);
			}
		}
		glEnd();
	}

	void vertexOpenGL(float u, float v){
		if (isTextured){
			glTexCoord2d(u / U_MAX, v / V_MAX);
		}
		if (isMaterialized){
			glVector3f(surfaceNormal(u, v));
		}
		glPoint3f(surfacePoint(u, v));
	}

	void displaySurfaceNormals(){
		glBegin(GL_LINES);
		for (int u = 0; u < U_MAX; ++u){
			for (int v = 0; v < V_MAX; ++v){
				Point p = surfacePoint(u, v);
				Vector n = surfaceNormal(u, v).normalized();
				glPoint3f(p);
				glPoint3f(p + n);
			}
		}
		glEnd();
	}
};

class Plane : public ParamSurface{
	float xMax, yMax;
	Point leftCorner;
	Vector x, y;

public:
	Plane(Point ref, Vector i, Vector j, float xMax, float yMax) :
		leftCorner(ref), xMax(xMax), yMax(yMax){
		x = i.normalized();
		y = j.normalized();
	}
	Point surfacePoint(float u, float v){
		Point temp = leftCorner + x * u / U_MAX * xMax + y * v / V_MAX*yMax;
		return leftCorner + x * u / U_MAX * xMax + y * v / V_MAX*yMax;
	}

	Point surfaceNormal(float u, float v){
		return x%y;
	}
};

class Ellipsoid : public ParamSurface{
	float a, b, c;
	Point center;

public:
	Ellipsoid(Point center, float a, float b, float c) :
		center(center), a(a), b(b), c(c){}

	//u and v are to be interpreted in degrees
	Point surfacePoint(float u, float v){
		//u - angle to z axis
		//v - angle to x axis
		//u [-90, 90] v [-180, 180]
		u = u / U_MAX * PI;
		v = v / V_MAX * 2 * PI;
		float x = a*sin(u)*cos(v);
		float y = b*sin(u)*sin(v);
		float z = c*cos(u);
		return Point(x, y, z) + center;
	}

	Point surfaceNormal(float u, float v){
		//dr/du x dr/dv
		//u [-90, 90] v [-180, 180]
		u = u / U_MAX * PI;
		v = v / V_MAX * 2 * PI;
		Vector drdu = Vector(a*cos(u)*cos(v), b*cos(u)*sin(v), -c*sin(u));
		Vector drdv = Vector(-a*sin(u)*sin(v), b*sin(u)*cos(v), 0);
		return drdu%drdv;
	}
};

class Cone : public ParamSurface{
	float h, r;
	Point center;

public:
	Cone(Point center, float h, float r) : center(center), h(h), r(r){}
	Point surfacePoint(float u, float v){
		//u - h
		//v - angle to x
		v = v / V_MAX * 2 * PI;
		float x = -u / U_MAX*h;
		float y = (1 - u / U_MAX) * r*sin(v);
		float z = (1 - u / U_MAX) * r*cos(v);
		return Point(x, y, z) + center;
	}
	Vector surfaceNormal(float u, float v){
		v = v / V_MAX * 2 * PI;
		Vector drdu = Vector(h / U_MAX, r*sin(v) / U_MAX, r*cos(v) / U_MAX);
		Vector drdv = Vector(0, (1 - u / U_MAX) *r*cos(v), -(1 - u / U_MAX) *r*sin(v));
		return drdu%drdv;
	}
};

class Cylinder : public ParamSurface{
	float h, r;
	Point center;
	Vector a;

public:
	Cylinder(Point center, Vector a, float h, float r) :
		center(center), h(h), r(r){
		this->a = a.normalized();
	}
	Point surfacePoint(float u, float v){
		v = v / V_MAX * 2 * PI;
		Point axisPoint = center + a*u / U_MAX*h;
		Vector B = Vector(0, 0, 1);
		Vector N = a%B;
		return axisPoint + B*r*cos(v) + N*r*sin(v);
	}
	Point surfaceNormal(float u, float v){
		Point axisPoint = center + a*u / U_MAX*h;
		return surfacePoint(u, v) - axisPoint;
	}
};

class StorkBody : public ParamSurface{
	CTRSpline* midline;
	CTRSpline* outline;

public:
	StorkBody(){
		midline = new CTRSpline();
		float weightUnit = U_MAX / (POINT_CNT - 1);
		midline->addPoint(Point(-5., 5.25, 0), 0);
		midline->addPoint(Point(-4.6, 2.6, 0), weightUnit * 1);
		midline->addPoint(Point(-2.75, 0.4, 0), weightUnit * 2);
		midline->addPoint(Point(0.1, -0.75, 0), weightUnit * 3);
		midline->addPoint(Point(2.65, -2.5, 0), weightUnit * 4);
		midline->setup();

		outline = new CTRSpline();
		outline->addPoint(Point(-4.8, 5.25, 0), 0);
		outline->addPoint(Point(-4.1, 2.8, 0), weightUnit * 1);
		outline->addPoint(Point(-1.5, 1.6, 0), weightUnit * 2);
		outline->addPoint(Point(1.1, 0, 0), weightUnit * 3);
		outline->addPoint(Point(3, -2.25, 0), weightUnit * 4);
		outline->setup();
	}
	float radius(float u){
		return (outline->curvePoint(u) - midline->curvePoint(u)).Length();
	}
	Vector normal(float u){
		//N(u) = BxT
		Vector B = Vector(0, 0, 1);
		Vector T = midline->curvePointDerivative(u).normalized();
		return B%T;
	}
	Point surfacePoint(float u, float v){
		//r(u,v) = s(u) + B(u) r(u) cos(v) + N(u) r(u) sin (v)
		//where s(u) - midline
		//                r(u) - radius
		//                N(u) = BxT
		//convert v to radian
		v = v / V_MAX * 2 * PI;
		Vector B = Vector(0, 0, 1); //z increases towards us
		return midline->curvePoint(u) + B*radius(u)*cos(v) + normal(u)*radius(u)*sin(v);
	}
	//returns surfaceNormal by averaging the normal of nearby surfaces
	Point surfaceNormal(float u, float v){
		//v1 r(u-1,v)
		//v3 r(u+1,v)
		//v2 r(u,v-1)
		//v4 r(u,v+1)
		Point r = surfacePoint(u, v);

		Vector v1 = surfacePoint(u - 1, v) - r;
		Vector v2 = surfacePoint(u, v - 1) - r;
		Vector v3 = surfacePoint(u + 1, v) - r;
		Vector v4 = surfacePoint(u, v + 1) - r;
		int valid = 4;
		if (u == 0){
			valid--;
			v1 = Vector(0, 0, 0);
		}
		if (v == 0){
			valid--;
			v2 = Vector(0, 0, 0);
		}
		if (u == U_MAX - 1){
			valid--;
			v3 = Vector(0, 0, 0);
		}
		if (v == V_MAX - 1){
			valid--;
			v4 = Vector(0, 0, 0);
		}

		Vector n1 = v1%v2;
		Vector n2 = v2%v3;
		Vector n3 = v3%v4;
		Vector n4 = v4%v1;

		return (n1 + n2 + n3 + n4) / valid;
	}
};

class Object{
	ParamSurface* surfaces[OBJ_NUM];
	int surfaceCount;
	ModelTransformation transformation;

public:
	Object() :surfaceCount(0){
		transformation = ModelTransformation();
	}
	void translate(Vector& v){
		transformation.translate = v;
	}
	void rotate(float phi, Vector axis){
		transformation.phi = phi;
		transformation.rotate = axis;
	}
	void scale(Vector& v){
		transformation.scale = v;
	}
	void addSurface(ParamSurface* newSurface){
		if (surfaceCount < OBJ_NUM){
			surfaces[surfaceCount] = newSurface;
			surfaceCount++;
		}
	}
	void draw(){
		glPushMatrix();
		transformation.setOpenGL();
		for (int i = 0; i < surfaceCount; ++i){
			surfaces[i]->draw();
		}
		glPopMatrix();
	}
};

class Camera{
	float fovy; //Specifies the field of view angle, in degrees, in the y direction.
	float aspect; //Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
	float zNear; //Specifies the distance from the viewer to the near clipping plane (always positive).
	float zFar; //Specifies the distance from the viewer to the far clipping plane (always positive).

	Point eye, lookAt;
	Vector vup;

public:
	Camera(Point eye, Point lookAt, Vector vup, float fov, float asp, float fp, float bp) :
		eye(eye), lookAt(lookAt), vup(vup), fovy(fov), aspect(asp), zNear(fp), zFar(bp){};

	void setOpenGL(){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovy, aspect, zNear, zFar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eye.x, eye.y, eye.z, lookAt.x, lookAt.y, lookAt.z, vup.x, vup.y, vup.z);
	}
};

class Scene{
	Object* objects[OBJ_NUM];
	Camera* camera;
	int objectCount;

public:
	Scene() :objectCount(0){
		//camera = new Camera(Point(-5, 10, -30), Point(-5, 5, 0), Vector(0, 1, 0), 54, 1, 1, 100);
		camera = new Camera(Point(0, 0, 0), Point(0, 0, -1), Vector(0, 1, 0), 54, 1, 1, 100);
	}

	void render(){
		camera->setOpenGL();

		float pos[] = { 0, 0, 1, 0 };

		if (DEBUGGING){
			glMatrixMode(GL_MODELVIEW);
			glTranslatef(0, 0, -35);
			//glRotatef(45, 1, 1, 0);
			glLightfv(GL_LIGHT0, GL_POSITION, pos);
			glEnable(GL_LIGHT0);

			Object* testObject = new Object();
			Cone* cone = new Cone(Point(0, 0, 0), 5, 2.5);
			Ellipsoid* ellipsoid = new Ellipsoid(Point(0, 0, 0), 10, 10, 10);
			Cylinder* cylinder = new Cylinder(Point(0, 0, 0), Vector(0, 1, 0), 5, 0.5);
			Plane* plane = new Plane(Point(0, 0, 0), Vector(1, 0, 0), Vector(0, 1, 0), 10, 10);
			StorkBody* body = new StorkBody();

			cone->setMaterial(orangeRed);
			ellipsoid->setMaterial(orangeRed);
			cylinder->setMaterial(orangeRed);
			plane->setMaterial(orangeRed);
			body->setMaterial(orangeRed);

			int testMode = 10;
			bool displayNormal = false;

			switch (testMode){
			case 0:
				if (displayNormal)
					ellipsoid->displaySurfaceNormals();
				ellipsoid->draw();
				break;
			case 1:
				if (displayNormal)
					cone->displaySurfaceNormals();
				cone->draw();
				break;
			case 2:
				if (displayNormal)
					body->displaySurfaceNormals();
				body->draw();
				break;
			case 3:
				if (displayNormal)
					plane->displaySurfaceNormals();
				plane->draw();
				break;
			case 4:
				testObject->addSurface(cone);
				testObject->addSurface(ellipsoid);
				testObject->addSurface(cylinder);
				testObject->addSurface(plane);
				testObject->addSurface(body);

				testObject->draw();
			default:
				break;
			}
		}
		else{
			glMatrixMode(GL_MODELVIEW);
			glTranslatef(0, 0, -35);
			glLightfv(GL_LIGHT0, GL_POSITION, pos);
			glEnable(GL_LIGHT0);
			for (int i = 0; i < objectCount; ++i){
				objects[i]->draw();
			}
		}
	}

	void createFrog(Object* frog){
		Ellipsoid* frogBody = new Ellipsoid(Point(-2, 1, 0), 5.5, 2.75, 3);
		Ellipsoid* frogHead = new Ellipsoid(Point(3.75, 1.5, 0), 2, 2, 2.5);
		Vector fl = Point(0, -5, -1) - Point(-2.25, 0.75, -1);
		Cylinder* frontLegR = new Cylinder(Point(0, 0.75, -1), fl, fl.Length() * 3 / 4, 1);
		Cylinder* frontLegL = new Cylinder(Point(0, 0.75, 1), fl, fl.Length() * 3 / 4, 1);
		frogBody->setMaterial(frogGreen);
		frogHead->setMaterial(frogGreen);
		frontLegR->setMaterial(frogGreen);
		frontLegL->setMaterial(frogGreen);

		frog->addSurface(frogBody);
		frog->addSurface(frogHead);
		frog->addSurface(frontLegR);
		frog->addSurface(frontLegL);
	}

	void build(){
		unsigned int texids;
		glGenTextures(1, &texids);
		glBindTexture(GL_TEXTURE_2D, texids);
		int level = 0, border = 0, width = 3, height = 1;
		float terrainPattern[] = { 0, 1, 0, 0.3, 1, 0.3, 0.6, 1, 0.6 };
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_RGB, GL_FLOAT, terrainPattern);
		Texture* terrainTexture = new Texture(texids);

		//terrain
		Object* terrain = new Object();
		Plane* plane = new Plane(Point(-55, -7, -20), Vector(1, 0, 0), Vector(0, 0, 1), 100, 100);
		plane->setTexture(terrainTexture);
		terrain->addSurface(plane);
		this->addObject(terrain);

		//stork
		Object* stork = new Object();

		StorkBody* storkbody = new StorkBody();
		Ellipsoid* head = new Ellipsoid(Point(-5.8, 5.5, 0), 1, 0.6, 0.5);
		Cone* beak = new Cone(Point(-6.5, 5.5, 0), 3, 0.25);

		Material* orangeRed = new Material();
		orangeRed->setKs(1, 1, 1, 1, 4);
		orangeRed->setKd(0.8, 0.27, 0, 1);
		orangeRed->setKa(0.2, 0.1, 0, 1);
		beak->setMaterial(orangeRed);

		Point p1, p2, p3, p4, p5;
		p1 = Point(-1.5, 0.5, 0);
		p2 = Point(0.25, -3, 0);
		p3 = Point(-2, -5.25, 0);
		Vector v1, v2, v3, v4;
		v1 = p2 - p1;
		v2 = p3 - p2;
		v3 = (Point(-1.3, -3.8, 0) - Point(-1.5, 0.5, 0)).normalized();
		v4 = (Point(-1.2, -6.6, 0) - Point(-1.3, -3.8, 0)).normalized();
		p4 = p1 + v3*v1.Length();
		p5 = p4 + v4*v2.Length();
		Cylinder* clu = new Cylinder(p1, v1, v1.Length(), 0.2);
		Cylinder* cld = new Cylinder(p2, v2, v2.Length(), 0.2);
		Cylinder* cru = new Cylinder(p1, v3, v1.Length(), 0.2);
		Cylinder* crd = new Cylinder(p4, v4, v2.Length(), 0.2);
		clu->setMaterial(orangeRed);
		cld->setMaterial(orangeRed);
		cru->setMaterial(orangeRed);
		crd->setMaterial(orangeRed);

		glGenTextures(1, &texids);
		glBindTexture(GL_TEXTURE_2D, texids);
		float storkPattern[] = { 1, 1, 1, 1, 1, 1, 0, 0, 0 };
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_RGB, GL_FLOAT, storkPattern);
		Texture* storkTexture = new Texture(texids);
		storkbody->setTexture(storkTexture);

		stork->addSurface(clu);
		stork->addSurface(cld);
		stork->addSurface(cru);
		stork->addSurface(crd);
		stork->addSurface(storkbody);
		stork->addSurface(head);
		stork->addSurface(beak);

		this->addObject(stork);

		//frogs
		Object* frog = new Object();
		createFrog(frog);
		frog->scale(Vector(0.3, 0.3, 0.3));
		frog->rotate(-45, Vector(0, 1, 0));
		frog->translate(Vector(-8, -6, 3));
		this->addObject(frog);
		
		Object* frog2 = new Object();
		createFrog(frog2);
		frog2->scale(Vector(0.3, 0.3, 0.3));
		frog2->rotate(-135, Vector(0, 1, 0));
		frog2->translate(Vector(4, -6, 10));
		this->addObject(frog2);
	}

	void addObject(Object* newObject){
		if (objectCount < OBJ_NUM){
			objects[objectCount] = newObject;
			objectCount++;
		}
	}
};

const int screenWidth = 600;        // alkalmazA!s ablak felbontA!sa
const int screenHeight = 600;

Color image[screenWidth*screenHeight];        // egy alkalmazA!s ablaknyi kA©p

Scene* scene;

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)
void onInitialization() {
	orangeRed = new Material();
	orangeRed->setKs(0.8, 0.2, 0, 1, 4);
	orangeRed->setKd(0.8, 0.27, 0, 1);
	orangeRed->setKa(0.2, 0.1, 0, 1);
	frogGreen = new Material();
	frogGreen->setKa(0, 0.4, 0, 1);
	frogGreen->setKs(1, 1, 1, 1, 20);
	frogGreen->setKd(0.7, 1, 0.4, 1);

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	//directional light
	float Ia[] = { 0, 0, 0, 1 };
	float Id[] = { 0.3, 0.3, 0.3, 1 };
	float Is[] = { 2, 2, 2, 1 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, Ia);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Id);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Is);

	scene = new Scene();
	scene->build();
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg
void onDisplay() {
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);                // torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

	scene->render();

	glutSwapBuffers();                                     // Buffercsere: rajzolas vege
}

// Billentyuzet esemenyeket lekezelo fuggveny (lenyomas)
void onKeyboard(unsigned char key, int x, int y) {}
// Billentyuzet esemenyeket lekezelo fuggveny (felengedes)
void onKeyboardUp(unsigned char key, int x, int y) {}
// Eger esemenyeket lekezelo fuggveny
void onMouse(int button, int state, int x, int y) {}
// Eger mozgast lekezelo fuggveny
void onMouseMotion(int x, int y){}
// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {}
// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
	glutInit(&argc, argv);                                 // GLUT inicializalasa
	glutInitWindowSize(600, 600);                        // Alkalmazas ablak kezdeti merete 600x600 pixel 
	glutInitWindowPosition(100, 100);                        // Az elozo alkalmazas ablakhoz kepest hol tunik fel
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);        // 8 bites R,G,B,A + dupla buffer + melyseg buffer

	glutCreateWindow("Grafika hazi feladat");                // Alkalmazas ablak megszuletik es megjelenik a kepernyon

	glMatrixMode(GL_MODELVIEW);                                // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);                        // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
	glLoadIdentity();

	onInitialization();                                        // Az altalad irt inicializalast lefuttatjuk

	glutDisplayFunc(onDisplay);                                // Esemenykezelok regisztralasa
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();                                        // Esemenykezelo hurok

	return 0;
}