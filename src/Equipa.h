/*
 * Equipa.h
 *
 *  Created on: 14/10/2015
 *      Author: Filipe
 */

#ifndef SRC_EQUIPA_H_
#define SRC_EQUIPA_H_

#include <string>
#include <vector>
#include "Prova.h"
#include "Desporto.h"

using namespace std;

class Atleta;
class Prova;

class Equipa{
	string nome;
	vector<Atleta *> atletas;
	vector<Desporto *> desportos;
public:
	Equipa(string n);
	string getNome() const;
	vector<Atleta *> getAtletas() const;
	vector<Desporto *> getDesportos() const;
	bool adicionaAtleta(Atleta * a);

	bool operator== (const Equipa & c) const;
	friend ostream & operator<<(ostream & o, const Equipa & d);

	void adicionarDesporto(vector<Desporto*> DespList);
	void retirarDesporto();
	void menu(vector<Desporto*> DespList);
	void menuAtletas();

	void adicionaAtleta();
	class AtletaExiste{
		string nome;
		public:
		AtletaExiste(){};
		AtletaExiste(string n) {nome = n;}
		string getNome() const {return nome;}
	};
};


class Atleta{
	string nome;
	Equipa* equipa;
	vector<Prova *> provas;
	char genero;
	int pontos;
public:
	Atleta(string n, Equipa* e, char g);
	string getNome() const;
	Equipa* getEquipa() const;
	int getPontos() const;
	vector<Prova *> getProvas() const;
	void adicionaProva(Prova * p);
	void adicionaPontuacao(int p);

	void menu();

	bool operator== (const Atleta & c) const;
	friend ostream & operator<<(ostream & o, const Atleta & d);
};


#endif /* SRC_EQUIPA_H_ */

