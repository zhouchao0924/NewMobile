
#ifndef _AITLine2D_H__
#define _AITLine2D_H__

#include "MathBase.h"
#include "kVector2D.h"

template <class T>
class TLine2DTemp
{
public:

	TLine2DTemp() : start(0, 0), end(1, 1) {};
	TLine2DTemp(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {};
	TLine2DTemp(const TVector2DTemp<T>& start, const TVector2DTemp<T>& end) : start(start), end(end) {};
	TLine2DTemp(const TLine2DTemp<T>& other) :start(other.start), end(other.end) {};

	// operators

	TLine2DTemp<T> operator+(const TVector2DTemp<T>& point) const { return TLine2DTemp<T>(start + point, end + point); };
	TLine2DTemp<T>& operator+=(const TVector2DTemp<T>& point) { start += point; end += point; return *this; };

	TLine2DTemp<T> operator-(const TVector2DTemp<T>& point) const { return TLine2DTemp<T>(start - point, end - point); };
	TLine2DTemp<T>& operator-=(const TVector2DTemp<T>& point) { start -= point; end -= point; return *this; };

	bool operator==(const TLine2DTemp<T>& other) const { return (start == other.start && end == other.end) || (end == other.start && start == other.end); };
	bool operator!=(const TLine2DTemp<T>& other) const { return !(start == other.start && end == other.end) || (end == other.start && start == other.end); };

	// functions

	void SetLine(const T& xa, const T& ya, const T& xb, const T& yb) { start.set(xa, ya); end.set(xb, yb); }
	void SetLine(const TVector2DTemp<T>& nstart, const TVector2DTemp<T>& nend) { start.set(nstart); end.set(nend); }
	void SetLine(const TLine2DTemp<T>& line) { start.set(line.start); end.set(line.end); }

	//! Returns length of line
	//! \return Returns length of line.
	double GetLength() const { return (start -end).Size(); };

	//! Returns the vector of the line.
	//! \return Returns the vector of the line.
	TVector2DTemp<T> GetVector() const { return TVector2DTemp<T>(start.X - end.X, start.Y - end.Y); };

	TVector2DTemp<T> GetMiddle() const { return TVector2DTemp<T>((start.X + end.X) / 2.0f, (start.Y + end.Y) / 2.0f); };

	//判断other(这条线段)与自己所在的直线是否相交,如果相交,rate里返回一个start到end的比率值
	bool GetIntersection(const TLine2DTemp<T>& other, T &rate)
	{
		T denom = ((other.end.y - other.start.y)*(end.X - start.X)) -
			((other.end.X - other.start.X)*(end.y - start.y));

		T nume_a = ((other.end.X - other.start.X)*(start.y - other.start.y)) -
			((other.end.y - other.start.y)*(start.X - other.start.X));

		T nume_b = ((end.X - start.X)*(start.y - other.start.y)) -
			((end.y - start.y)*(start.X - other.start.X));

		if (denom == 0.0f)
		{
			return false;
			//				if(nume_a == 0.0f && nume_b == 0.0f)
			//					return COINCIDENT;
			//				return PARALLEL;
		}

		T ua = nume_a / denom;
		T ub = nume_b / denom;

		if ((ub >= 0.0f) && (ub <= 1.0f))
		{
			rate = ua;
			return true;
		}
		return false;
	}

	//判断两条线段是否相交,如果相交,out里返回交点
	//目前共线当作不相交
	bool GetIntersectionPoint(const TLine2DTemp<T>& other, TVector2DTemp<T>& out)
	{
		T rate;
		if (!GetIntersection(other, rate))
			return false;

		if ((rate >= 0.0f) && (rate <= 1.0f))
		{
			out = start + (end - start)*rate;
			return true;
		}
		return false;
	}


	//! Returns unit vector of the line.
	//! \return Returns unit vector of this line.
	TVector2DTemp<T> GetUnitVector()
	{
		T len = (T)1.0 / (T)GetLength();
		return TVector2DTemp<T>((end.X - start.X) * len, (end.Y - start.Y) * len);
	}

	void GetProjection(const TVector2DTemp<T>& point, T &rate) const
	{
		TVector2DTemp<T> c = point - start;
		TVector2DTemp<T> v = end - start;
		T d = (T)v.Size();
		v /= d;
		rate = v.Dot(c);
		rate /= d;
	}

	//get the projection on this line for the point
	void GetProjectionPoint(const TVector2DTemp<T>& point, TVector2DTemp<T>&pointProj) const
	{
		T rate;
		GetProjection(point, rate);
		pointProj = start + (end - start)*rate;
	}

	//! Returns the closest point on this line to a point
	TVector2DTemp<T> GetClosestPoint(const TVector2DTemp<T>& point) const
	{
		TVector2DTemp<T> c = point - start;
		TVector2DTemp<T> v = end - start;
		T d = (T)v.Size();
		v /= d;
		T t = v.Dot(c);

		if (t < (T)0.0) return start;
		if (t > d) return end;

		v *= t;
		return start + v;
	}

	double GetDistTo(const TVector2DTemp<T>& point) const
	{
		TVector2DTemp<T> v = GetProjectionPoint(point);
		return point.GetDistanceFrom(v);
	}

	//返回:1:在正面,0:在线上,-1:在反面,
	//所谓正面是指当你从start向end移动时,正面在你的右侧
	int ClassifyPoint(const TVector2DTemp<T> &point) const
	{
		float d = (point.X - start.X)*(end.y - start.y) - (point.y - start.y)*(end.X - start.X);
		if (d > 0.0f)
			return 1;
		if (d < 0.0f)
			return -1;
		return 0;
	}
	// member variables
	TVector2DTemp<T> start;
	TVector2DTemp<T> end;
};

typedef TLine2DTemp<float> kLine2D;

#endif