#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <stdexcept>
#include <memory>
#include <cstdio>
#include <stdio.h>
using namespace std;

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
string getIpFromDDNS(){
	string salidaProceso =  exec(" dig +short [TU SITIO WEB]");
	//replace(salidaProceso, "bpbrainiak.ddns.net has address ","");
	cout << "\nIP determinada por el proceso: " << salidaProceso << "\n"; 
	return salidaProceso;
}
string get_brainiakSite()
{
	string salidaProceso = exec("dig @[TU DNS ANTIGUO] [TU DOMINIO] +short");
	cout  << "\nEl proceso ha determinado que el sitio tiene la siguiente direccion IP " << salidaProceso << "\n";
	return salidaProceso;
}
void copiar(){
	exec("cp /etc/bind/[TU ARCHIVO DE CONFIGURACION DE ZONA] /etc/[TU ARCHIVO DE CONFIGURACION DE ZONA]bak");
	exec("cp /tmp/[TEMPLATE DEL ARCHIVO DE CONFIGURACION DE ZONA] /etc/bind/[TU ARCHIVO DE CONFIGURACION DE ZONA]");
        cout << "Reiniciando servicio..."  << "\n";
	cout << exec("service bind9 restart") << "\n";
}
int main(int argc, char* argv[]){
	string ipAddress = "";
	string ipAddressSitio = "";
	vector<string> lineas;
	int largoVector = 0;
	int indiceVector = 0;
	string line;
	if(argc==1) {
		cout<< "No se especifico direccion IP\n";
		ipAddress = getIpFromDDNS();		
	}
	else {	
		cout << "direccion IP : "  << argv[1] <<"\n";
		ipAddress = argv[1];
	}
	ipAddressSitio = get_brainiakSite();
	ifstream myfile("/var/dbfileutility/[TEMPLATE DE ARCHIVO DE CONFIGURACION DE ZONA]");
	if(myfile.is_open()){
		while(getline(myfile, line)){
			lineas.push_back(line);
			largoVector++;
		}
		myfile.close();
	}	
	else {
		cout << "Unable to open file\n";
		return 0;
	}
	ofstream fileout("/tmp/[TEMPLATE DE ARCHIVO DE CONFIGURACION DE ZONA]", ios::trunc);
	if(fileout.is_open()){
		for(indiceVector=0; indiceVector<largoVector; indiceVector++){
			replace(lineas[indiceVector], "{IP_HOME}", ipAddress);
			replace(lineas[indiceVector], "{IP_SITE}",ipAddressSitio);
			fileout << lineas[indiceVector] << "\n";
		}
		fileout.close();
		copiar();
	}
	else cout << "Unable to open file\n";
	return 0;
}
