
#ifndef __AI_QUAT_H__
#define __AI_QUAT_H__

#include "MathBase.h"
#include "kVector3D.h"

template <class T>
class TQuat
{
public:

	//! Default Constructor
	TQuat() : X(0), Y(0), Z(0), W(1)
	{
	}

	//! Constructor
	TQuat(T X, T Y, T Z, T W)
		: X(X), Y(Y), Z(Z), W(W)
	{
	}

	//! Constructor which converts euler angles to a TQuat
	TQuat(T x, T y, T z)
	{
		Set(x, y, z);
	}

	//! equal operator
	bool operator==(const TQuat<T>& other) const
	{
		if (X != other.X)
			return false;
		if (Y != other.Y)
			return false;
		if (Z != other.Z)
			return false;
		if (W != other.W)
			return false;

		return true;
	}

	//! assignment operator
	TQuat<T>& operator=(const TQuat<T>& other)
	{
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		W = other.W;
		return *this;
	}

	//! add operator
	TQuat<T> operator+(const TQuat<T>& other) const
	{
		TQuat tmp;
		tmp.X = X + other.X;
		tmp.Y = Y + other.Y;
		tmp.Z = Z + other.Z;
		tmp.W = W + other.W;
		return tmp;
	}

	//! multiplication operator
	TQuat<T> operator*(const TQuat<T>& other) const
	{
		TQuat tmp;
		tmp.W = (other.W * W) - (other.X * X) - (other.Y * Y) - (other.Z * Z);
		tmp.X = (other.W * X) + (other.X * W) + (other.Y * Z) - (other.Z * Y);
		tmp.Y = (other.W * Y) + (other.Y * W) + (other.Z * X) - (other.X * Z);
		tmp.Z = (other.W * Z) + (other.Z * W) + (other.X * Y) - (other.Y * X);
		return tmp;
	}

	//! multiplication operator
	TQuat<T> operator*(T s) const
	{
		return TQuat(s*X, s*Y, s*Z, s*W);
	}

	//! multiplication operator
	TQuat<T>& operator*=(T s)
	{
		X *= s; Y *= s; Z *= s; W *= s;
		return *this;
	}

	//! multiplication operator
	TVector3DTemp<T> operator* (const TVector3DTemp<T>& v) const
	{
		// nVidia SDK implementation 

		TVector3DTemp<T> uv, uuv;
		TVector3DTemp<T> qvec(X, Y, Z);
		uv = qvec.CrossProduct(v);
		uuv = qvec.CrossProduct(uv);
		uv *= (((T)2.0f) * W);
		uuv *= (T)2.0f;

		return v + uv + uuv;
	}

	//! multiplication operator
	TQuat<T>& operator*=(const TQuat<T>& other)
	{
		*this = other * (*this);
		return *this;
	}

	bool Equals(const TQuat<T>& other)const
	{
		return Equals_Float(X, other.X) &&
			Equals_Float(Y, other.Y) &&
			Equals_Float(Z, other.Z) &&
			Equals_Float(W, other.W);
	}


	//! calculates the dot product
	T Dot(const TQuat<T>& q2) const
	{
		return (X * q2.X) + (Y * q2.Y) + (Z * q2.Z) + (W * q2.W);
	}

	//! sets new TQuat 
	void Set(T x, T y, T z, T w)
	{
		X = x;
		Y = y;
		Z = z;
		W = w;
	}

	//! sets new TQuat based on euler angles
	void Set(T x, T y, T z)
	{
		T angle;

		angle = x * 0.5f;
		T sr = (T)sin(angle);
		T cr = (T)cos(angle);

		angle = y * 0.5f;
		T sp = (T)sin(angle);
		T cp = (T)cos(angle);

		angle = z * 0.5f;
		T sy = (T)sin(angle);
		T cy = (T)cos(angle);

		T cpcy = cp * cy;
		T spcy = sp * cy;
		T cpsy = cp * sy;
		T spsy = sp * sy;

		X = sr * cpcy - cr * spsy;
		Y = cr * spcy + sr * cpsy;
		Z = cr * cpsy - sr * spcy;
		W = cr * cpcy + sr * spsy;

		Normalize();
	}

	//! normalizes the TQuat
	TQuat<T>& Normalize()
	{
		T n = X*X + Y*Y + Z*Z + W*W;

		if (n == 1)
			return *this;

		n = 1.0f / sqrt(n);
		X *= n;
		Y *= n;
		Z *= n;
		W *= n;

		return *this;

	}

	//! Inverts this TQuat
	void MakeInverse()
	{
		X = -X; Y = -Y; Z = -Z;
	}

	//! axis must be unit length 
	//angle是一个顺时针的角度(当axis指向观察者的时候)
	//! The TQuat representing the rotation is 
	//!  q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k) 
	void FromAngleAxis(T angle, const TVector3DTemp<T>& axis)
	{
		T fHalfAngle = 0.5f*angle;
		T fSin = (T)sin(fHalfAngle);
		W = (T)cos(fHalfAngle);
		X = fSin*axis.X;
		Y = fSin*axis.Y;
		Z = fSin*axis.Z;
	}

	//quaternion必须是Normalize的
	void ToAngleAxis(T &angle, TVector3DTemp<T>&axis)
	{
		angle = 2 * acos(W);
		T s = sqrt(1 - W*W); // assuming quaternion normalised then w is less than 1, so term always positive.
		if (s < 0.001)
		{
			axis.X = X;
			axis.y = Y;
			axis.z = Z;
		}
		else
		{
			axis.X = X / s; // normalise axis
			axis.y = Y / s;
			axis.z = Z / s;
		}
	}

	//欧拉角的一些说明:
	//head:正值表示沿着y轴的顺时针方向旋转,当y轴指向观察者的时候
	//pitch:正值表示向上抬
	//roll:正值表示沿着z轴的顺时针方向旋转,当z轴指向观察者的时候
	TVector3DTemp<T> GetEuler()
	{
		TVector3DTemp<T> euler;
		DOUBLE sqw0 = W*W;
		DOUBLE sqz0 = Z*Z;
		DOUBLE sqy0 = Y*Y;
		DOUBLE sqx0 = (-X)*(-X);

		//head
		euler.X = (float)atan2((DOUBLE)(2 * Y*W - 2 * Z*(-X)), (DOUBLE)(sqw0 + sqz0 - sqy0 - sqx0));

		//pitch
		euler.Y = -(float)asin(rosy::In_range<DOUBLE>(-(DOUBLE)(2 * Z*Y + 2 * (-X)*W), -1, 1));

		//roll
		euler.Z = (float)atan2((DOUBLE)(2 * Z*W - 2 * Y*(-X)), (DOUBLE)(sqw0 - sqz0 - sqx0 + sqy0));

		return euler;
	}

	void FromEuler(TVector3DTemp<T> &a)
	{
		float c1 = cos(a.X / 2.0f);
		float c2 = cos(-a.Y / 2.0f);
		float c3 = cos(a.Z / 2.0f);
		float s1 = sin(a.X / 2.0f);
		float s2 = sin(-a.Y / 2.0f);
		float s3 = sin(a.Z / 2.0f);

		W = c3*c2*c1 + s3*s2*s1;
		Z = s3*c2*c1 - c3*s2*s1;
		X = c3*s2*c1 + s3*c2*s1;
		Y = c3*c2*s1 - s3*s2*c1;
	}


	//build a rotate TQuat which could rotate start to end
	//both the 2 vector should be normalized
	//from Game Programming Gems I ,chapter 2,section 10
	void From2Vector(TVector3DTemp<T>&start, TVector3DTemp<T>&end)
	{
		TVector3DTemp<T> t;
		t = start.CrossProduct(end);
		T d;
		d = start.Dot(end);
		d = (1 + d) * 2;
		if (d < 0)
			d = 0;
		T s;
		s = (T)sqrt(d);
		X = t.X / s;
		Y = t.Y / s;
		Z = t.Z / s;
		W = s / (T)2.0;
	}
	//! Interpolates the TQuat between to quats based on time
	//time is 0,full q1; time is 1,full q2
	static TQuat<T> Lerp(const TQuat<T> &q1, const TQuat<T> &q2, T time)
	{
		TQuat<T> q11 = q1;
		TQuat<T> q22 = q2;
		T angle = q11.Dot(q22);

		if (angle < 0.0f)
		{
			q11 *= -1.0f;
			angle *= -1.0f;
		}

		T scale;
		T invscale;

		if ((angle + 1.0f) > 0.05f)
		{
			if ((1.0f - angle) >= 0.05f)  // spherical interpolation
			{
				T theta = (T)acos(angle);
				T invsintheta = 1.0f / (T)sin(theta);
				scale = (T)sin(theta * (((T)1.0f) - ((T)time))) * invsintheta;
				invscale = (T)sin(theta * ((T)time)) * invsintheta;
			}
			else // linear interploation
			{
				scale = 1.0f - time;
				invscale = time;
			}
		}
		else
		{
			q22 = TQuat(-q11.Y, q11.X, -q11.W, q11.Z);
			scale = (T)sin(KM_PI * (((T)0.5f) - ((T)time)));
			invscale = (T)sin(((T)KM_PI) * ((T)time));
		}
		return (q11*scale) + (q22*invscale);;
	}

	static TQuat<T> Exp(TQuat<T> & q) //[0,a*V]-->[cosa,sina*V]
	{
		TVector3DTemp<T> v(q.X, q.Y, q.Z);
		float a = float(v.Size());
		float cosa = cos(a);
		float sina = sin(a);
		TQuat<T> ret;
		ret.W = cosa;
		if (a > 0) {
			ret.X = sina*q.X / a;
			ret.Y = sina*q.Y / a;
			ret.Z = sina*q.Z / a;
		}
		else {
			ret.X = ret.Y = ret.Z = 0;
		}

		return ret;
	}
	static TQuat<T> Squad(TQuat<T> &q0, TQuat<T> &q1, TQuat<T> &a, TQuat<T> &b, float t)// a,b:ctrl point
	{
		TQuat<T> c, d, ret;
		c.Slerp(q0, q1, t);
		d.Slerp(a, b, t);
		ret.Slerp(c, d, 2 * t*(1 - t));
		return ret;
	}
	static TQuat<T> Spline(TQuat<T> &qn1, TQuat<T>&q, TQuat<T>&qp1)//ctrl point
	{
		TQuat<T> ret;
		TQuat<T> q1 = q;
		q1.MakeInverse();
		TQuat<T> a = Log(q1*qn1) + Log(q1*qp1);
		a *= -0.25;
		ret = q*exp(a);
		return ret;
	}

	static TQuat<T> Log(TQuat<T> &q) // [cosa,sina*V] --> [0,a*V]
	{
		TQuat<T> ret;
		float a = acos(q.W);
		float sina = sin(a);
		ret.W = 0;
		if (sina > 0) {
			ret.X = T(float(q.X) / sina);
			ret.Y = T(float(q.Y) / sina);
			ret.Z = T(float(q.Z) / sina);
		}
		else {
			ret.X = ret.Y = ret.Z = 0;
		}
		return ret;
	}


	union
	{
		struct { T X, Y, Z, W; };
		T array[4];
	};
};

typedef TQuat<float> kQuaternion;

//v1,v2 should be normalized
//if r is 0,return v1,if r is 1,return v2
template<class T>
TVector3DTemp<T> Slerp(TVector3DTemp<T>&v1, TVector3DTemp<T>&v2, T r)
{
	TQuat<T> qt, qtZero;
	qt.From2Vector(v1, v2);
	qt.Lerp(qtZero, qt, r);
	TVector3DTemp<T> ret = qt*v1;//could make it no longer normalized
	ret.Normalize();
	return ret;
}

#endif