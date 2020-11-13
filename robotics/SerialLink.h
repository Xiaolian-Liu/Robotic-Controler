#pragma once
#include <string>
#include <vector>
#include <Eigen\Dense>
#include <float.h>
#include "JointVec.h"
#include "Link.h"
#include "transform.h"


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
	Matrix4d base;
	Matrix4d tool;

	string name;
	string manuf;
	//bool fast;

	const int n = 6;
	//string config;
	bool mdh;



	SerialLink();
	SerialLink(const string &name, const string &manuf, bool mdh);
	SerialLink(const vector<Link> &links, const string &name, const string &manuf, bool mdh = false);
	virtual ~SerialLink();
	Matrix4d fkine(const JointVec &q);  //units rad
	virtual Matrix4d fkinef(const JointVec &q);  // units rad
	int ikine(JointVec & q, JointVec q0, Matrix4d T);
	void setLinks(const vector<Link> &links);
	void setBase(Matrix4d m);
	void setTool(Matrix4d m);
};


}
