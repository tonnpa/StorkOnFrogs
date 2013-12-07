//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beinleftUpperLegdolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization fA1ggvA©nyt kivA©ve, a lefoglalt adat korrekt felszabadA­tA!sa nA©lkA1l 
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Nguyen Phan Anh
// Neptun : BQZUZ3
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

#define OBJ_NUM 10
#define PI 3.14
#define POINT_CNT 5
#define U_MAX 20
#define V_MAX 20
#define EPS 0.0001
#define DELTA_TIME 20.0

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
	float operator*(const Vector& v) {
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector operator%(const Vector& v) {
		return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	void operator+=(const Vector& v) {
		x += v.x; y += v.y; z += v.z;
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

	void addPoint(Point p, float weight){
		if (pointCount < POINT_CNT){
			this->p[pointCount] = p;
			this->t[pointCount] = weight;
			pointCount++;
		}
	}
	void setup(){
		v[0] = v[pointCount - 1] = Vector(0.01, 0.01, 0.01);
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
		kd[0] = r; kd[1] = g; kd[2] = b; kd[3] = o;
	}
	void setKs(float r, float g, float b, float o, float shine){
		ks[0] = r; ks[1] = g; ks[2] = b; ks[3] = o;
		shininess[0] = shine;
	}
	void setKa(float r, float g, float b, float o){
		ka[0] = r; ka[1] = g; ka[2] = b; ka[3] = o;
	}
	void setOpenGL(){
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	};
};

Material* orangeRed;
Material* frogGreen;
Material* storkWhite;
Material* fireflyShine;
Material* eyeBlack;

class Texture{
	unsigned int texID;
public:
	Texture(unsigned int ID) :texID(ID){}
	void setOpenGL(){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	};
};

Texture* terrainTexture;
Texture* storkTexture;

class Bone {
	float length, rot_angle;
	Vector rot_axis, joint_pos, dir;

public:
	Bone(Vector joint_pos, float rot_angle, Vector rot_axis, float length, Vector direction): 
		joint_pos(joint_pos), rot_angle(rot_angle), length(length) {
		this->rot_axis = rot_axis.normalized();
		this->dir = direction.normalized();
	}
	friend class Stork;
};

class ParamSurface{
	Texture* texture;
	Material* material;

	bool isTextured, isMaterialized;
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
		if (isTextured)
			texture->setOpenGL();
		else
			glDisable(GL_TEXTURE_2D);
		if (isMaterialized)
			material->setOpenGL();
		else
			glDisable(GL_LIGHTING);

		glBegin(GL_TRIANGLES);
		for (int u = 0; u < U_MAX; ++u){
			for (int v = 0; v < V_MAX; ++v){
				vertexOpenGL(u, v);	vertexOpenGL(u, v + 1);vertexOpenGL(u + 1, v + 1);

				vertexOpenGL(u + 1, v + 1);	vertexOpenGL(u + 1, v);vertexOpenGL(u, v);
			}
		}
		glEnd();
	}
	void vertexOpenGL(float u, float v){
		if (isTextured)
			glTexCoord2d(u / U_MAX, v / V_MAX);
		if (isMaterialized)
			glVector3f(surfaceNormal(u, v));
		glPoint3f(surfacePoint(u, v));
	}
};

class Plane : public ParamSurface{
	float xMax, yMax;
	Point leftCorner;
	Vector x, y;

public:
	Plane(Point ref, Vector i, Vector j, float xMax, float yMax) :
		leftCorner(ref), xMax(xMax), yMax(yMax){
		x = i.normalized();	y = j.normalized();
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

public:
	Ellipsoid(float a, float b, float c) :
		a(a), b(b), c(c){}
	Point surfacePoint(float u, float v){
		u = u / U_MAX * PI;
		v = v / V_MAX * 2 * PI;
		float x = a*sin(u)*cos(v);
		float y = b*sin(u)*sin(v);
		float z = c*cos(u);
		return Point(x, y, z);
	}
	Point surfaceNormal(float u, float v){
		u = u / U_MAX * PI;
		v = v / V_MAX * 2 * PI;
		Vector drdu = Vector(a*cos(u)*cos(v), b*cos(u)*sin(v), -c*sin(u));
		Vector drdv = Vector(-a*sin(u)*sin(v), b*sin(u)*cos(v), 0);
		return drdu%drdv;
	}
};

class Cone : public ParamSurface{
	float h, r;

public:
	Cone(float h, float r) : h(h), r(r){}
	Point surfacePoint(float u, float v){
		v = v / V_MAX * 2 * PI;
		float x = -u / U_MAX*h;
		float y = (1 - u / U_MAX) * r*sin(v);
		float z = (1 - u / U_MAX) * r*cos(v);
		return Point(x, y, z);
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
	Vector a;

public:
	Cylinder(Vector a, float h, float r) :
		h(h), r(r){
		this->a = a.normalized();
	}
	Point surfacePoint(float u, float v){
		v = v / V_MAX * 2 * PI;
		Point axisPoint = a*u / U_MAX*h;
		Vector B = Vector(0, 0, 1);
		Vector N = a%B;
		return axisPoint + B*r*cos(v) + N*r*sin(v);
	}
	Point surfaceNormal(float u, float v){
		Point axisPoint = a*u / U_MAX*h;
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
		midline->addPoint(Point(-5.1, 6, 0), 0);
		midline->addPoint(Point(-4.7, 3.35, 0), weightUnit * 1);
		midline->addPoint(Point(-2.85, 1.15, 0), weightUnit * 2);
		midline->addPoint(Point(0, 0, 0), weightUnit * 3);
		midline->addPoint(Point(2.55, -1.75, 0), weightUnit * 4);
		midline->setup();

		outline = new CTRSpline();
		outline->addPoint(Point(-4.9, 6, 0), 0);
		outline->addPoint(Point(-4.2, 3.55, 0), weightUnit * 1);
		outline->addPoint(Point(-1.6, 2.35, 0), weightUnit * 2);
		outline->addPoint(Point(1, 0.75, 0), weightUnit * 3);
		outline->addPoint(Point(2.9, -1.5, 0), weightUnit * 4);
		outline->setup();
	}
	float radius(float u){
		return (outline->curvePoint(u) - midline->curvePoint(u)).Length();
	}
	Vector normal(float u){
		Vector B = Vector(0, 0, 1);
		Vector T = midline->curvePointDerivative(u).normalized();
		return B%T;
	}
	Point surfacePoint(float u, float v){
		v = v / V_MAX * 2 * PI;
		Vector B = Vector(0, 0, 1);
		return midline->curvePoint(u) + B*radius(u)*cos(v) + normal(u)*radius(u)*sin(v);
	}
	Point surfaceNormal(float u, float v){
		Point r = surfacePoint(u, v);

		Vector v1 = surfacePoint(u - 1, v) - r;
		Vector v2 = surfacePoint(u, v - 1) - r;
		Vector v3 = surfacePoint(u + 1, v) - r;
		Vector v4 = surfacePoint(u, v + 1) - r;
		int valid = 4;
		if (u == 0){
			valid--; v1 = Vector(0, 0, 0);
		}
		if (v == 0){
			valid--; v2 = Vector(0, 0, 0);
		}
		if (u == U_MAX - 1){
			valid--; v3 = Vector(0, 0, 0);
		}
		if (v == V_MAX - 1){
			valid--; v4 = Vector(0, 0, 0);
		}
		Vector n1 = v1%v2; Vector n2 = v2%v3; Vector n3 = v3%v4; Vector n4 = v4%v1;
		return (n1 + n2 + n3 + n4) / valid;
	}
};

class Object{
protected:
	ParamSurface* surfaces[OBJ_NUM];
	int surfaceCount;
	ModelTransformation transformation;

public:
	Object() :surfaceCount(0){
		transformation = ModelTransformation();
	}
	void translate(Vector v){
		transformation.translate = v;
	}
	void rotate(float phi, Vector axis){
		transformation.phi = phi;
		transformation.rotate = axis;
	}
	void scale(Vector v){
		transformation.scale = v;
	}
	void addSurface(ParamSurface* newSurface){
		if (surfaceCount < OBJ_NUM){
			surfaces[surfaceCount] = newSurface;
			surfaceCount++;
		}
	}
	virtual void draw(){
		glPushMatrix();
		transformation.setOpenGL();
		for (int i = 0; i < surfaceCount; ++i){
			surfaces[i]->draw();
		}
		glPopMatrix();
	}
	virtual void animate(float deltaTime){}
};

class Stork : public Object{
	//body parts
	StorkBody* storkbody;
	Ellipsoid* head;
	Cone* beak;
	Ellipsoid* leftEye;
	Ellipsoid* rightEye;
	Cylinder* leftUpperLeg;
	Cylinder* leftLowerLeg;
	Cylinder* rightUpperLeg;
	Cylinder* rightLowerLeg;
	//skeleton
	Bone* leftFemur;
	Bone* rightFemur;
	Bone* leftTibia;
	Bone* rightTibia;
	Bone* spine;
	Bone* headBone;
	Point leftEyePos;
	Point rightEyePos;
	Point beakPos;

	//animation variables
	float deltaAngle, forward, up, turnAngle, turnState;
	int leftLegState, rightLegState;
	Point start;

public:
	Stork() : deltaAngle(4), leftLegState(1), rightLegState(2), forward(0), up(0), turnAngle(0), turnState(0), start(Point(0, 0, 0)){
		storkbody = new StorkBody();
		storkbody->setMaterial(storkWhite);
		Point sPL = Point(-2.85, 1.15, 0); Point sPU = Point(-5.9, 6.25, 0); Point beakTip = Point(-7.6, 3.75, 0);
		spine = new Bone(sPL, 0, Vector(0, 0, 1), (sPU-sPL).Length(), sPU-sPL);
		head = new Ellipsoid(1, 0.6, 0.5);
		head->setMaterial(storkWhite);
		headBone = new Bone(spine->dir*spine->length, 0, Vector(0, 0, 1), 3.5, beakTip - sPU);
		beak = new Cone(3, 0.25);
		leftEyePos = head->surfacePoint(-U_MAX / 4, -V_MAX / 8); rightEyePos = head->surfacePoint(-U_MAX / 4 * 3, -V_MAX / 8);
		leftEye = new Ellipsoid(0.1, 0.1, 0.1); rightEye = new Ellipsoid(0.1, 0.1, 0.1);
		leftEye->setMaterial(eyeBlack); rightEye->setMaterial(eyeBlack);
		beakPos = head->surfacePoint(-U_MAX / 2, 0) + Point(0.25, 0, 0);
		beak->setMaterial(orangeRed);
		Point leftTop = Point(-1.6, -0.25, 0.75); Point rightTop = Point(-1.6, -0.25, -0.75);
		Vector legDirection = Vector(0, -1, 0);
		float upperLegLength = 3; float lowerLegLength = 3;
		leftUpperLeg = new Cylinder(legDirection, upperLegLength, 0.2);
		leftLowerLeg = new Cylinder(legDirection, lowerLegLength, 0.2);
		rightUpperLeg = new Cylinder(legDirection, upperLegLength, 0.2);
		rightLowerLeg = new Cylinder(legDirection, lowerLegLength, 0.2);
		leftUpperLeg->setMaterial(orangeRed); leftLowerLeg->setMaterial(orangeRed);
		rightUpperLeg->setMaterial(orangeRed); rightLowerLeg->setMaterial(orangeRed);
		storkbody->setTexture(storkTexture);
		leftFemur = new Bone(leftTop, -30, Vector(0, 0, 1), upperLegLength, legDirection);
		rightFemur = new Bone(rightTop, 30, Vector(0, 0, 1), upperLegLength, legDirection);
		leftTibia = new Bone(legDirection*upperLegLength, 0, Vector(0, 0, 1), lowerLegLength, legDirection);
		rightTibia = new Bone(legDirection*upperLegLength, 0, Vector(0, 0, 1), lowerLegLength, legDirection);
		this->addSurface(leftUpperLeg); this->addSurface(leftLowerLeg);
		this->addSurface(rightUpperLeg); this->addSurface(rightLowerLeg);
		this->addSurface(storkbody);
		this->addSurface(head);
		this->addSurface(leftEye); this->addSurface(rightEye);
		this->addSurface(beak);
	}
	void drawHead(){
		head->draw();
		glPushMatrix();
			glTranslatef(leftEyePos.x, leftEyePos.y, leftEyePos.z);
			leftEye->draw();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(rightEyePos.x, rightEyePos.y, rightEyePos.z);
			rightEye->draw();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(beakPos.x, beakPos.y, beakPos.z);
			beak->draw();
		glPopMatrix();
	}
	void turn(float phi){
		turnAngle = phi;
		transformation.phi += phi;
	}
	void draw(){
		glPushMatrix();
			transformation.setOpenGL();
			if (turnAngle != 0){
				turnState += turnAngle;
				turnAngle = 0;
				start = Point(forward, 0, 0);
				forward = 0;
			}
			//glTranslatef(forward, up, 0);
			glTranslatef(start.x, start.y, start.z);
			glTranslatef(forward, up, 0);
			glRotatef(turnState, 0, 1, 0);
			glTranslatef(-start.x, -start.y, -start.z);
			storkbody->draw();

			glPushMatrix();
				glTranslatef(spine->joint_pos.x, spine->joint_pos.y, spine->joint_pos.z);
				glTranslatef(headBone->joint_pos.x, headBone->joint_pos.y, headBone->joint_pos.z);
				glRotatef(10, 0, 0, 1);
				drawHead();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(leftFemur->joint_pos.x, leftFemur->joint_pos.y, leftFemur->joint_pos.z);
				glRotatef(leftFemur->rot_angle, leftFemur->rot_axis.x, leftFemur->rot_axis.y, leftFemur->rot_axis.z);
				leftUpperLeg->draw();
				glTranslatef(leftTibia->joint_pos.x, leftTibia->joint_pos.y, leftTibia->joint_pos.z);
				glRotatef(leftTibia->rot_angle, leftTibia->rot_axis.x, leftTibia->rot_axis.y, leftTibia->rot_axis.z);
				leftLowerLeg->draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(rightFemur->joint_pos.x, rightFemur->joint_pos.y, rightFemur->joint_pos.z);
				glRotatef(rightFemur->rot_angle, rightFemur->rot_axis.x, rightFemur->rot_axis.y, rightFemur->rot_axis.z);
				rightUpperLeg->draw();
				glTranslatef(rightTibia->joint_pos.x, rightTibia->joint_pos.y, rightTibia->joint_pos.z);
				glRotatef(rightTibia->rot_angle, rightTibia->rot_axis.x, rightTibia->rot_axis.y, rightTibia->rot_axis.z);
				rightLowerLeg->draw();
			glPopMatrix();

		glPopMatrix();
	}
	void nextLegState(float deltaTime, int* currentLegState, Bone* femur, Bone* tibia){
		float dtu, dtl;

		switch (*currentLegState)
		{
		case 1:
			//change state
			if (femur->rot_angle > 30){
				*currentLegState = 2;
				femur->rot_angle = 30;
				tibia->rot_angle = 0;
			}
			//proceed without state change
			dtu = dtl = deltaTime / (500.0 / 15.0);
			femur->rot_angle += deltaAngle * dtu;
			break;
		case 2:
			if (femur->rot_angle > 50){
				*currentLegState = 3;
				femur->rot_angle = 50;
				tibia->rot_angle = -95;
			}
			dtu = deltaTime / (200.0 / 5.0); dtl = deltaTime / (200.0 / 24.00);
			femur->rot_angle += deltaAngle * dtu;
			tibia->rot_angle -= deltaAngle * dtl;
			break;
		case 3:
			if (femur->rot_angle < -30){
				*currentLegState = 1;
				femur->rot_angle = -30;
				tibia->rot_angle = 0;
			}
			dtu = deltaTime / (300.0 / 20.0); dtl = deltaTime / (300.0 / 24.00);
			femur->rot_angle -= deltaAngle * dtu;
			tibia->rot_angle += deltaAngle * dtl;
			break;
		default:
			break;
		}
	}
	void animate(float deltaTime){
		deltaTime /= 2;
		float oldFemurAngle = leftFemur->rot_angle; float oldTibiaAngle = leftTibia->rot_angle;
		float oldFemurAngle2 = rightFemur->rot_angle; float oldTibiaAngle2 = rightTibia->rot_angle;
		nextLegState(deltaTime, &leftLegState, leftFemur, leftTibia);
		nextLegState(deltaTime, &rightLegState, rightFemur, rightTibia);
		//stepping with left leg
		if (leftLegState == 3 || leftLegState ==2){
			forward -= fabs(leftFemur->length * sin(leftFemur->rot_angle / 180.0*PI) + leftTibia->length * sin((leftTibia->rot_angle + leftFemur->rot_angle) / 180.0*PI) -
				leftFemur->length * sin(oldFemurAngle / 180.0*PI) - leftTibia->length * sin((oldTibiaAngle + oldFemurAngle) / 180.0*PI));
			up = rightFemur->length * cos(rightFemur->rot_angle / 180.0*PI) + rightTibia->length * cos((rightTibia->rot_angle + rightFemur->rot_angle) / 180.0*PI);
		}
		//stepping with right leg
		else{
			forward -= fabs(rightFemur->length * sin(rightFemur->rot_angle / 180.0*PI) + rightTibia->length * sin((rightTibia->rot_angle + rightFemur->rot_angle) / 180.0*PI) -	rightFemur->length * sin(oldFemurAngle2 / 180.0*PI) - rightTibia->length * sin((oldTibiaAngle2 + oldFemurAngle2) / 180.0*PI));
			up = leftFemur->length * cos(leftFemur->rot_angle / 180.0*PI) + leftTibia->length * cos((leftTibia->rot_angle + leftFemur->rot_angle) / 180.0*PI);
		}
	}
};

class Camera{
	float fovy, aspect, zNear, zFar;
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
	Stork* stork;

public:
	Scene() :objectCount(0){
		camera = new Camera(Point(0, 20, 0), Point(0, 0, -50), Vector(0, 1, 0), 54, 1, 1, 100);
	}
	void addObject(Object* newObject){
		if (objectCount < OBJ_NUM){
			objects[objectCount] = newObject;
			objectCount++;
		}
	}
	void render(){
		camera->setOpenGL();
		float pos[] = { 0, -1, 1, 0 };
		float positionalPos[] = { 7.8, 7.8, -25, 1 };
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		glLightfv(GL_LIGHT1, GL_POSITION, positionalPos);
		for (int i = 0; i < objectCount; ++i){
			objects[i]->draw();
		}
	}
	void build(){
		Object* terrain = new Object();
		createTerrain(terrain);
		terrain->translate(Vector(-20, -0.5, -65));
		this->addObject(terrain);

		//Object* firefly = new Object();
		//createFirefly(firefly);
		//firefly->translate(Vector(0, 0, -50));
		//this->addObject(firefly);

		stork = new Stork();
		stork->rotate(180, Vector(0, 1, 0));
		stork->translate(Vector(0, 0, -50));
		this->addObject(stork);

		//Object* frog = new Object();
		//createFrog(frog);
		//frog->scale(Vector(0.3, 0.3, 0.3));
		//frog->rotate(-45, Vector(0, 1, 0));
		//frog->translate(Vector(-8, -6, -57));
		//this->addObject(frog);

		//Object* frog2 = new Object();
		//createFrog(frog2);
		//frog2->scale(Vector(0.3, 0.3, 0.3));
		//frog2->rotate(-135, Vector(0, 1, 0));
		//frog2->translate(Vector(4, -6, -50));
		//this->addObject(frog2);
	}
	//void createFirefly(Object* firefly){
	//	Ellipsoid* fireflyBody = new Ellipsoid(Point(8, 8, 0), 0.2, 0.2, 0.2);
	//	fireflyBody->setMaterial(fireflyShine);
	//	firefly->addSurface(fireflyBody);
	//}
	void createTerrain(Object* terrain){
		Plane* plane = new Plane(Point(0, 0, 0), Vector(1, 0, 0), Vector(0, 0, 1), 40, 40);
		plane->setTexture(terrainTexture);
		terrain->addSurface(plane);
	}
	//void createFrog(Object* frog){
	//	Ellipsoid* frogBody = new Ellipsoid(Point(-2, 1, 0), 5.5, 2.75, 3);
	//	Ellipsoid* frogHead = new Ellipsoid(Point(3.75, 1.5, 0), 2, 2, 2.5);
	//	Vector fl = Point(0, -5, -1) - Point(-2.25, 0.75, -1);
	//	Cylinder* frontLegR = new Cylinder(Point(0, 0.75, -1), fl, fl.Length() * 3 / 4, 1);
	//	Cylinder* frontLegL = new Cylinder(Point(0, 0.75, 1), fl, fl.Length() * 3 / 4, 1);
	//	frogBody->setMaterial(frogGreen);
	//	frogHead->setMaterial(frogGreen);
	//	frontLegR->setMaterial(frogGreen);
	//	frontLegL->setMaterial(frogGreen);
	//	float angleToZ = U_MAX / 4;
	//	float angleToX = V_MAX / 8;
	//	Point eye = frogHead->surfacePoint(angleToZ, angleToX);
	//	Point eye2 = frogHead->surfacePoint(angleToZ * 3, angleToX);
	//	Ellipsoid* leftEye = new Ellipsoid(eye, 0.7, 0.7, 0.7);
	//	Ellipsoid* rightEye = new Ellipsoid(eye2, 0.7, 0.7, 0.7);
	//	leftEye->setMaterial(eyeBlack);
	//	rightEye->setMaterial(eyeBlack);
	//	frog->addSurface(frogBody);
	//	frog->addSurface(frogHead);
	//	frog->addSurface(frontLegR);
	//	frog->addSurface(frontLegL);
	//	frog->addSurface(leftEye);
	//	frog->addSurface(rightEye);
	//}

	void simulateWorld(float old_time, float current_time){
		for (float timeSlotBeginning = old_time; timeSlotBeginning < current_time; timeSlotBeginning += DELTA_TIME){
			float timeSlotEnd = fminf(timeSlotBeginning + DELTA_TIME, current_time);
			float dt = timeSlotEnd - timeSlotBeginning;
			for (int i = 0; i < objectCount; ++i){
				objects[i]->animate(dt);
			}
		}
	}
	void rotateStork(float phi){
		stork->turn(phi);
	}
};

Scene* scene;

const int screenWidth = 600;
const int screenHeight = 600;
Color image[screenWidth*screenHeight];

float old_time;
float current_time;

void onInitialization() {
	orangeRed = new Material();
	orangeRed->setKs(0.8, 0.2, 0, 1, 4);
	orangeRed->setKd(0.8, 0.27, 0, 1);
	orangeRed->setKa(0.2, 0.1, 0, 1);
	frogGreen = new Material();
	frogGreen->setKa(0, 0.4, 0, 1);
	frogGreen->setKs(1, 1, 1, 1, 20);
	frogGreen->setKd(0.7, 1, 0.4, 1);
	storkWhite = new Material();
	storkWhite->setKa(0.75, 0.75, 0.75, 1);
	storkWhite->setKd(1, 1, 1, 1);
	storkWhite->setKs(1, 1, 1, 1, 7);
	fireflyShine = new Material();
	fireflyShine->setKa(1, 1, 1, 1);
	fireflyShine->setKd(1, 1, 1, 1);
	fireflyShine->setKs(1, 1, 1, 1, 20);
	eyeBlack = new Material();
	eyeBlack->setKa(0, 0, 0, 1);
	eyeBlack->setKd(0.1, 0.1, 0.1, 1);
	eyeBlack->setKs(0.7, 0.7, 0.7, 1, 5);

	unsigned int texids;
	glGenTextures(1, &texids);
	glBindTexture(GL_TEXTURE_2D, texids);
	int level = 0, border = 0, width = 3, height = 1;
	float terrainPattern[] = { 0.54, 0.27, 0.07, 0, 0, 0, 0.82, 0.41, 0.16 };
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_RGB, GL_FLOAT, terrainPattern);
	terrainTexture = new Texture(texids);

	glGenTextures(1, &texids);
	glBindTexture(GL_TEXTURE_2D, texids);
	float storkPattern[] = { 1, 1, 1, 1, 1, 1, 0, 0, 0 };
	glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, border, GL_RGB, GL_FLOAT, storkPattern);
	storkTexture = new Texture(texids);

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	float dIa[] = { 0, 0, 0, 1 };
	float dId[] = { 0.3, 0.3, 0.3, 1 };
	float dIs[] = { 0.4, 0.4, 0.4, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, dIa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dId);
	glLightfv(GL_LIGHT0, GL_SPECULAR, dIs);

	float pIa[] = { 0.1, 0.1, 0.1, 1 };
	float pId[] = { 0.5, 0.5, 0.5, 1 };
	float pIs[] = { 0.7, 0.7, 0.7, 1 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, pIa);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, pId);
	glLightfv(GL_LIGHT1, GL_SPECULAR, pIs);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	scene = new Scene();
	scene->build();

	old_time = current_time = 0;
}

void onDisplay() {
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene->render();

	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {}
void onKeyboardUp(unsigned char key, int x, int y) {
	if (key == 'b') {
		scene->rotateStork(10);
	}
	else if (key == 'j'){
		scene->rotateStork(-10);
	}
}
void onMouse(int button, int state, int x, int y) {}
void onMouseMotion(int x, int y){}
void onIdle() {
	old_time = current_time;
	current_time = glutGet(GLUT_ELAPSED_TIME);

	scene->simulateWorld(old_time, current_time);

	glutPostRedisplay();
}
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