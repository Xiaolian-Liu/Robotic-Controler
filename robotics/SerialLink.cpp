#include "SerialLink.h"
#include <vector>
using std::vector;

namespace RKD {


SerialLink::SerialLink()
{

}




SerialLink::SerialLink(const vector<Link> &links, const string &name, const string &manuf, bool mdh)
	: links(links), name(name), manuf(manuf), mdh(mdh)
{
	this->gravity << 0, 0, -9.80665;
	this->base.setIdentity();
	this->tool.setIdentity();
}


SerialLink::~SerialLink()
{

}

SerialLink::SerialLink(const string &name, const string &manuf, bool mdh) 
	: name(name), manuf(manuf), mdh(mdh)
{
	// TODO: Add your implementation code here.
	this->gravity << 0, 0, -9.80665;
	this->base.setIdentity();
	this->tool.setIdentity();
}

Matrix4d SerialLink::fkine(const JointVec &q)
{
	Matrix4d res = this->base;
	for (int i = 0; i < n; i++)
	{
		res = res * links[i].A(q[i]);
	}
	res = res * this->tool;
	return res;
}

Matrix4d SerialLink::fkinef(const JointVec &q)
{
	return fkine(q);
}

int SerialLink::ikine(JointVec & q, JointVec q0, Matrix4d T)
{
	return 0;
}

}





void RKD::SerialLink::setLinks(const vector<Link> &links)
{
	// TODO: Add your implementation code here.

	/*this->links.erase(links.begin(), links.end());
	for (int i = 0; i < links.size(); i++)
	{
		this->links.push_back(links[i]);
	}*/
	this->links = links;
}

void RKD::SerialLink::setBase(Matrix4d m)
{
	this->base = m;
}

void RKD::SerialLink::setTool(Matrix4d m)
{
	this->tool = m;
}
