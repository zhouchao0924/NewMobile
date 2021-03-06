
#pragma once

#include "MathBase.h"
#include "kVector2D.h"
#include "kLine.h"

template <class T>
class TSphereTemp
{
public:

	// Constructors
	TSphereTemp()
	{
		center.Set(0, 0, 0);
		radius = 0;
	}

	TSphereTemp(const TVector3DTemp<T> &c, T r)
	{
		Set(c, r);
	}

	bool operator==(const TSphereTemp<T>& other) const
	{
		return (center == other.center&& radius == other.radius);
	}
	// functions
	void Set(const TVector3DTemp<T> &c, T r)
	{
		center = c;
		radius = r;
	}
	void Zero()
	{
		center.set(0, 0, 0);
		radius = 0;
	}
	void SetRadius(T r)
	{
		radius = r;
	}
	void SetCenter(T x, T y, T z)
	{
		center.set(x, y, z);
	}
	void SetCenter(const TVector3DTemp<T> &c)
	{
		center = c;
	}
	void BuildFromPoints(TVector3DTemp<T> *pts, int nPts)
	{
		Zero();
		if (nPts <= 0)
			return;
		SetCenter(pts[0]);
		for (int i = 1; i < nPts; i++)
		{
			TVector3DTemp<T> off = (pts[i] - center);
			T d = T(off.SizeSquared());
			if (d > radius*radius)
			{
				d = sqrt(d);
				T r = ((T)0.5) * (d + radius);
				T scale = (r - radius) / d;
				center = center + scale*off;
				radius = r;
			}
		}
	}

	//update the TSphere's radius with the point(while keeping the center fixed)
	//FC for :Fixed Center
	void AddPoint(const TVector3DTemp<T> &v)
	{
		T length = (T)((v - center).Size());
		if (length > radius)
			radius = length;
	}

	//! Returns if the line intersects with a shpere
	//! \param sorigin: Origin of the shpere.
	//! \param sradius: Radius if the TSphere.
	//! \return Returns true if there is an intersection.
	//! If there is one, the distance to the first intersection point
	//! is stored in outdistance.
	//注意:如果线段两个端点都在球体内部,返回false
	bool GetIntersectionWithLine(TLineTemp<T> &line, float & outdistance)
	{
		TVector3DTemp<T> q = center - line.start;
		float c = q.Size();

		if (((center - line.end).SizeSquared() < radius*radius) && (c <= radius))
		{
			//起始点和结束点都在球内部,没有交点
			return false;
		}

		TVector3DTemp<T> dirLine = line.Vector();
		float distLine = dirLine.Size();
		dirLine /= (T)distLine;

		float v = q.Dot(dirLine);
		if ((v < 0.0f) && (c > radius))
			return false;//起始点在球体外部时,并且向着远离圆心的方向射去时,没有交点
		float d = radius *radius - (c*c - v*v);

		if (d < 0.0)
			return false;

		if (c > radius)
			outdistance = v - sqrt(d);
		else
			outdistance = v + sqrt(d);

		if (outdistance > distLine)
			return false;
		return true;
	}

	bool GetIntersectionWithLine(TLineTemp<T> &line, TVector3DTemp<T>&outIntersection)
	{
		float dist;
		if (!GetIntersectionWithLine(line, dist))
			return false;

		TVector3DTemp<T> linevect = line.Vector().Normalize();
		outIntersection = line.start + linevect*(T)dist;
		return true;
	}

	bool Contains(TVector3DTemp<T> &v)
	{
		return (v - center).SizeSquared() < radius*radius;
	}

	TVector3DTemp<T> center;
	T radius;
};

// template <class T>
// class TCircle
// {
// public:
// 	TCircle() { Radius = 0; }
// 	TCircle(const TVector2DTemp<T> &InCenter, T InRadius) { Center = InCenter; Radius = InRadius; }
// 	void SetRadius(float InRadius) { Radius = InRadius; }
// 	void SetCenter(const TVector2DTemp<T> &InCenter) { Center = InCenter; }
// 	bool IsPointInSide(const TVector2DTemp<T> &Position) { return Center.GetDistanceFrom(Position) <= (double)Radius; }
// 	TVector2DTemp<T> Center;
// 	T			 Radius;
// };

typedef TSphereTemp<float> kSphere;
// typedef TCircle<float> kCircle;



