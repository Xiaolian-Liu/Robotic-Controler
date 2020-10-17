#include "SerialLink.h"
#include <vector>
using std::vector;

namespace RKD {


SerialLink::SerialLink()
{

}




SerialLink::SerialLink(const vector<Link> &links, const string &name, const string &manuf, bool mdh)
{
	this->n = links.size();
	this->mdh = mdh;
	this->name = name;
	this->manuf = manuf;
	this->links = links;

	this->gravity << 0, 0, -9.80665;
	this->base.setIdentity();
	this->tool.setIdentity();
}


SerialLink::~SerialLink()
{

}

SerialLink::SerialLink(const string &name, const string &manuf, int nDof, bool mdh)
{
	// TODO: Add your implementation code here.
	this->name = name;
	this->manuf = manuf;
	this->n = nDof;
	this->mdh = mdh;

	this->gravity << 0, 0, -9.80665;
	this->base.setIdentity();
	this->tool.setIdentity();
}

Matrix4d SerialLink::fkine(const vector<double> &q)
{
	Matrix4d res = this->base;
	for (int i = 0; i < n; i++)
	{
		res = res * links[i].A(q[i]);
	}
	res = res * this->tool;
}

Matrix4d SerialLink::fkinef(const vector<double> &q)
{
	return fkine(q);
}

int SerialLink::ikine(vector<double> &q, const Matrix4d &T0, const Matrix4d &T)
{
	return 0;
}

}





void RKD::SerialLink::setLinks(const vector<Link> &links)
{
	// TODO: Add your implementation code here.
	this->links.erase(links.begin(), links.end());
	for (int i = 0; i < links.size(); i++)
	{
		this->links.push_back(links[i]);
	}
}
