#ifndef _H_VEC3_
#define _H_VEC3_
class Vec3
{
public:
	Vec3() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	};
	Vec3(double _x, double _y, double _z) {
		x = _x;
		y = _y;
		z = _z;
	};
	~Vec3();
public:
	bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z;}
	Vec3 operator-() const { 
		return Vec3(-x, -y, -z);
	};
	Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
	Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
	Vec3 operator*(const double d) const { return Vec3(x*d, y*d, z*d); }
	Vec3 operator/(const double d) const { return Vec3(x/d, y/d, z/d); }

public:
	double x, y, z;
};

#endif
