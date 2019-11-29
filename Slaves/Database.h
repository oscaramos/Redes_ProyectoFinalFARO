#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
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

	bool existLink(string pk1, string pk2)
	{
		if(!existNode(pk1))
			return false;
		vector<string>& vec = relationships[pk1];
		auto it = find(vec.begin(), vec.end(), pk2);
		cout << "DATABASE: existe? link pk1=["<<pk1<<"] y" << " pk2=["<<pk2<<"] ";
		if(it != vec.end())
		{
			cout << "SI" << endl;
			return true;
		}
		cout << "NO" << endl;
		return false;
	}

	void createNode(string pk, string content)
	{
		nodes[pk] = {content};
		cout << "DATABASE: creado nodo pk=["<<pk<<"] con contenido=["<<content<<"]" << endl;
	}

	void deleteNode(string pk)
	{
		if(existNode(pk))
		{
			nodes.erase(nodes.find(pk));
			cout << "DATABASE: eliminado nodo pk=["<<pk<<"]" << endl;
		}
	}

	void linkRelationship(string pk1, string pk2)
	{
		relationships[pk1].push_back(pk2);
		cout << "DATABASE: linkeado nodos pk1=["<<pk1<<"]" << " pk2=["<<pk2<<"]" << endl;
	}

	void unlinkRelationship(string pk1, string pk2)
	{
		vector<string>& vec = relationships[pk1];
		auto it = find(vec.begin(), vec.end(), pk2);
		if(it != vec.end()){
			vec.erase(it);
			cout << "DATABASE: deslinkeados nodos pk1=["<<pk1<<"]" << " pk2=["<<pk2<<"]" << endl;
		}
	}

	vector<string> getAllRelationships(string pk)
	{
		return relationships[pk];
	}
};

#endif