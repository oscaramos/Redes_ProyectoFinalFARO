#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <vector>
using namespace std;

class Database
{
public:
	map<string, string> nodes; // map<pk, content>
	map<string, vector<string> > relationships;// map<pk1, vector<pk2>>

public:
	bool existNode(string pk)
	{
		return nodes.count(pk) > 0;
	}

	void createNode(string pk, string content)
	{
		nodes[pk] = {content};
	}

	void deleteNode(string pk)
	{
		auto it = nodes.find(pk);
		nodes.erase(it);
	}

	void linkRelationship(string pk1, string pk2)
	{
		relationships[pk1] = pk2;
	}

	void unlinkRelationship(string pk1, string pk2)
	{
		
	}

	vector<string> getAllRelationships(string pk)
	{
		auto it = nodes.find(pk);
		return it->;
	}




};

#endif