#include "stdafx.h"
#include "Coordinate.h"


namespace Graph
{
	///<summary>
	/// コンストラクタ
	///</summary>
	Coordinate::Coordinate(double x, double y, unsigned int id) : Identifiable(id), _x(x), _y(y)
	{
	}


	///<summary>
	/// デストラクタ
	///</summary>
	Coordinate::~Coordinate()
	{
	}

	///<summary>
	/// x座標の取得
	///</summary>
	double Coordinate::x() const
	{
		return _x;
	}


	///<summary>
	/// y座標の取得
	///</summary>
	double Coordinate::y() const
	{
		return _y;
	}


	Coordinate operator+(const Coordinate& c1, const Coordinate& c2)
	{
		return Coordinate(c1._x + c2._x, c1._y + c2._y);
	}

	Coordinate operator-(const Coordinate& c1, const Coordinate& c2)
	{
		return Coordinate(c1._x - c2._x, c1._y - c2._y);
	}

	///<summary>
	/// 2点間の距離を計算する
	///</summary>
	double dist(const Coordinate& c1, const Coordinate& c2)
	{
		double x_diff = c2._x - c1._x;
		double y_diff = c2._y - c1._y;
		return std::sqrt(x_diff * x_diff + y_diff * y_diff);
	}
}
