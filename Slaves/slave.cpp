#include "slave.hpp"

/*Constructor del nodo*/
node::node(string _pk, string _content) {
	pk = _pk;
	content = _content;
}

/*Busqueda de un PK en el vector de links*/
int node::findLink(string pk) {
	bool found = false;
	int i = 0;
	int pos = -1;
	while (found == false) {
		if (links[i] == pk) {
			pos = i;
			found = true;
		}
		i++;
	}
	return pos;
}

slave::slave(string _IP, int _PORT, sockets& _socks) {
	IP = _IP;
	PORT = _PORT;
	socks = _socks;
}

string slave::getIP() {
	return IP;
}

int slave::getPORT() {
	return PORT;
}

void slave::setDevices(vector<pair<string, int>> nDevices) {
	devices = nDevices;
}

void slave::updateDB(string input) {
	int len_pk = stoi(input.substr(0, 2));
	string pk = input.substr(3, len_pk);
	int socket_id = stoi(input.substr(len_pk + 4));
	pk_socket[pk] = socket_id;
}

/*Busqueda de un PK en el vector de nodos*/
int slave::findNode(string pk) {
	bool found = false;
	int i = 0;
	int pos = -1;
	while (found == false) {
		if (nodes[i].pk == pk) {
			pos = i;
			found = true;
		}
		i++;
	}
	return pos;
}

/*Querys*/

string slave::q_create(string input) {
	//Separar el paquete
	int len_pk = stoi(input.substr(0, 2));
	string pk = input.substr(3, len_pk);
	int len_content = stoi(input.substr(len_pk + 4, 3));
	string content = input.substr(len_pk + 8);
	//Crear el nodo
	node n(pk,content);
	nodes.push_back(n);
	return "OK";
}

string slave::q_link(string input) {
	//Separar el paquete
	string output = "";
	int len_pk1 = stoi(input.substr(0, 2));
	string pk1 = input.substr(3, len_pk1);
	int len_pk2 = stoi(input.substr(len_pk1 + 4, 2));
	string pk2 = input.substr(len_pk1 + 7);
	//Buscar el primer PK y añadir el segundo PK al vector de links
	int pos = findNode(pk1);
	if (pos != -1) {
		nodes[pos].links.push_back(pk2);
		output = "OK";
	}
	else {
		output = "ERROR";
	}
	return output;
}

string slave::q_delete(string input) {
	//Separar el paquete
	string output = "";
	int len_pk = stoi(input.substr(0, 2));
	string pk = input.substr(3, len_pk);
	//Eliminar el nodo
	int pos = findNode(pk);
	if (pos != -1) {
		nodes.erase(nodes.begin() + pos);
		output = "OK";
	}
	else {
		output = "ERROR";
	}
	return output;
}

string slave::q_unlink(string input) {
	//Separar el paquete
	string output = "";
	int len_pk1 = stoi(input.substr(0, 2));
	string pk1 = input.substr(3, len_pk1);
	int len_pk2 = stoi(input.substr(len_pk1 + 4, 2));
	string pk2 = input.substr(len_pk1 + 7);
	//Buscar el primer PK, buscar el segundo PK y eliminarlo del vector de links
	int pos = findNode(pk1);
	if (pos != -1) {
		int lpos = nodes[pos].findLink(pk2);
		if (lpos != -1) {
			nodes[pos].links.erase(nodes[pos].links.begin() + lpos);
			output = "OK";
		}
		output = "ERROR";
	}
	else {
		output = "ERROR";
	}
	return output;
}

string slave::q_update(string input) {
	//Separar el paquete
	string output = "";
	int len_oldpk = stoi(input.substr(0, 2));
	string oldpk = input.substr(3, len_oldpk);
	int len_newpk = stoi(input.substr(len_oldpk + 4, 2));
	string newpk = input.substr(len_oldpk + 7);
	//Buscar el PK antiguo y reemplazarlo por el nuevo PK
	int pos = findNode(oldpk);
	if (pos != -1) {
		nodes[pos].pk = newpk;
		output = "OK";
	}
	else {
		output = "ERROR";
	}
	return output;
}

/*Tag Especial (T) (Explore && Select)
Enviar varios separados por |
Posible Bucle Infinito
T | E PKc | Profundidad-1 | 
Link.PK0 | Link.PK1 | ... | Link.PKn*/
string slave::q_explore(string input) {
	string output = "T ";
	int len_pk = stoi(input.substr(0, 2));
	string pk = input.substr(3, len_pk);
	int prof = stoi(input.substr(len_pk + 4, 2));
	int pos = findNode(pk);
	int links_size = nodes[pos].links.size();
	if (prof > 0) {
		output += ("|E " + nodes[pos].pk);
		output += ("|" + to_string(prof-1));
		for (int i = 0; i < links_size; i++) {
			output += ("|" + nodes[pos].links[i]);
		}
	}
	else {
		output = "IGNORE";
	}
	return output;
}

/*Tag Especial (T) (Explore && Select)
Enviar varios separados por |
Posible Bucle Infinito
T | S PKc Content | Profundidad-1 |
Link.PK0 | Link.PK1 | ... | Link.PKn*/
string slave::q_select(string input) {
	string output = "T ";
	int len_pk = stoi(input.substr(0, 2));
	string pk = input.substr(3, len_pk);
	int prof = stoi(input.substr(len_pk + 4, 2));
	int pos = findNode(pk);
	int links_size = nodes[pos].links.size();
	if (prof > 0) {
		output += ("|S " + nodes[pos].pk + "->" + nodes[pos].content);
		output += ("|" + to_string(prof - 1));
		for (int i = 0; i < links_size; i++) {
			output += ("|" + nodes[pos].links[i]);
		}
	}
	else {
		output = "IGNORE";
	}
	return output;
}

string slave::parser(string packet) {
	string msg = "";
	if (packet[0] == 'P') {
		msg = "KEEPALIVE";
	}
	else if (packet[0] == '1') {
		msg = q_create(packet.substr(2));
	}
	else if (packet[0] == '2') {
		msg = q_link(packet.substr(2));
	}
	else if (packet[0] == '3') {
		msg = q_delete(packet.substr(2));
	}
	else if (packet[0] == '4') {
		msg = q_unlink(packet.substr(2));
	}
	else if (packet[0] == '5') {
		msg = q_update(packet.substr(2));
	}
	else if (packet[0] == '6') {
		msg = q_explore(packet.substr(2));
	}
	else if (packet[0] == '7') {
		msg = q_select(packet.substr(2));
	}
	else if (packet[0] == 'U') {
		//U 02 PK ID
		updateDB(packet.substr(2));
		msg = "ADD_PK.ID";
	}
	else if (packet[0] == 'E') {
		v_explore.push_back(packet.substr(2));
		msg = "EXPLORE";
	}
	else if (packet[0] == 'S') {
		v_select.push_back(packet.substr(2));
		msg = "SELECT";
	}
	else {
		msg = "IGNORE";
	}
	return msg;
}

void slave::recv_send(int sockfd, int fd, sockets& _socks) {
	socks = _socks;
	//RECV
	int n = 0;
	string r_packet = "";
	n = read(socks, sockfd, r_packet, r_packet.size());
	cout << "From Master: " << r_packet << endl;
	//SEND
	string s_packet = parser(r_packet);
	string msg = "";
	if (s_packet[0] == 'T') {
		vector<string> values = separateValues(s_packet, '|');
		//Enviar E a quien envio Explore ||
		//Enviar S a quien envio Select
		msg = values[1];
		vector<string> subvalues = separateValues(values[1]);
		n = write(socks,fd, msg.c_str(), msg.size());
		//Enviar Explore a todos los links, es decir, el nivel siguiente
		if (subvalues[0] == "E") {
			for (int i = 3; i < values.size(); i++) {
				msg = "6 " + zeroPadding(to_string(values[i].size()), 2) + " " + values[i] + " " + zeroPadding(values[2], 2);
				n = write(socks, pk_socket[values[i]], msg.c_str(), msg.size());
			}
		}
		//Enviar Select a todos los links, es decir, el nivel siguiente
		if (subvalues[0] == "S") {
			for (int i = 3; i < values.size(); i++) {
				msg = "7 " + zeroPadding(to_string(values[i].size()), 2) + " " + values[i] + " " + zeroPadding(values[2], 2);
				n = write(socks, pk_socket[values[i]], msg.c_str(), msg.size());
			}
		}
	}
	//
	if (s_packet == "EXPLORE") {
		//Si ya llegaron los explore de todos lo hijos enviar salida completa del explore
		vector<string> values = separateValues(s_packet, ' ');
		int pos = findNode(values[1]);
		if (v_explore.size() == nodes[pos].links.size()) {
			for (int i = 0; i < v_explore.size(); i++) {
				msg += (v_explore[i] + "\n");
			}
			n = write(socks, fd, msg.c_str(), msg.size());
		}
	}
	if (s_packet == "SELECT") {
		//Si ya llegaron los select de todos lo hijos enviar salida completa del select
		vector<string> values = separateValues(s_packet, ' ');
		int pos = findNode(values[1]);
		if (v_select.size() == nodes[pos].links.size()) {
			for (int i = 0; i < v_select.size(); i++) {
				msg += (v_select[i] + "\n");
			}
			n = write(socks, fd, msg.c_str(), msg.size());
		}
	}
}
