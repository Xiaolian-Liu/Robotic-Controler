#pragma once
#include <string>
#include <vector>
#include <Eigen\Dense>
#include <float.h>

#include "Link.h"


using std::string;
using std::vector;
using RKD::Link;
using namespace Eigen;

namespace RKD {


class SerialLink
{
public:
	vector<Link> links;
	Vector3d gravity;
	Vector4d base;
	Vector4d tool;

	string name;
	string manuf;
	//bool fast;

	int n;
	//string config;
	bool mdh;



	SerialLink();
	SerialLink(const string &name, const string &manuf, int n, bool mdh);
	SerialLink(const vector<Link> &links, const string &name, const string &manuf, bool mdh = false);
	virtual ~SerialLink();
	Matrix4d fkine(const vector<double> &q);
	virtual Matrix4d fkinef(const vector<double> &q);
	virtual int ikine(vector<double> &q, const Matrix4d &T0, const Matrix4d &T);
	void setLinks(const vector<Link> &links);
};


}
