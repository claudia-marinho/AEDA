#include "Campeonato.h"
#include <iostream>
#include "Lists.h"
#include "Modalidade.h"
#include "insertionSort.h"
#include <algorithm>

using namespace std;

Campeonato::Campeonato(string n, Data i, Data f, Hora a, Hora fe){
	nome = n;
	inicio = i;
	fim = f;
	abertura = a;
	fecho = fe;
	criado = false;
}

string Campeonato::getNome() const
{
	return nome;
}

Data Campeonato::getInicio() const
{
	return inicio;
}
Data Campeonato::getFim() const
{
	return fim;
}

Hora Campeonato::getAbertura() const
{
	return abertura;
}

Hora Campeonato::getFecho() const
{
	return fecho;
}

vector<Desporto *> Campeonato::getDesportos() const
{
	return desportos;
}

vector<Prova*> Campeonato::getProvas() const
{
	return provas;
}

vector<Equipa*> Campeonato::getEquipas() const
{
	return equipas;
}

bool Campeonato::adicionaDesporto(Desporto &d){
	int indice = -1;
	for(unsigned int i = 0; i < desportos.size(); i++)
		if (desportos[i]->getNome() == d.getNome())
			{
			indice = i;
			return false;
			}

	if (indice == -1)
		desportos.push_back(&d);
	return true;
}

bool Campeonato::adicionaEquipa(Equipa * eq)
{
	int indice = -1;
	for(unsigned int i = 0; i < equipas.size(); i++)
	{
		if (comparar_strings(eq->getNome(), equipas[i]->getNome()))
			{
			indice = i;
			return false;
			}
	}

	if(indice == -1)
		equipas.push_back(eq);
	return true;

}

void Campeonato::adicionaProva(Prova &p)
{

	bool tamanho_zero = false;
	if (p.getData() < inicio || fim < p.getData())
	{
		throw Data::DataInvalida(p.getData().getAno(), p.getData().getMes(), p.getData().getDia());
	}
	else
	{
		Hora fimProva = p.getInicio() + p.getModalidade()->getDuracao();

		if (p.getInicio() < abertura || fecho < fimProva)
		{
			throw Hora::HoraInvalida(p.getInicio().getHoras(), p.getInicio().getMinutos());
		}
		else
		{
			if (provas.size() == 0)
			{
				tamanho_zero = true;
				provas.push_back(&p);
			}
			else
			{
				for (unsigned int i = 0; i < provas.size(); i++)
				{
					bool simultaneo = provas[i]->Simultaneo(p);

					if (simultaneo && comparar_strings(p.getModalidade()->getNome(),provas[i]->getModalidade()->getNome()))
						throw Prova::ProvasSimultaneas(p.getModalidade()->getNome(), provas[i]->getModalidade()->getNome());
				}
			}
		}
	}

	if (!tamanho_zero)
		provas.push_back(&p);

}

bool Campeonato::loadDesportos(string nome_ficheiro)
{
	//int elementos_equipa;
	string desporto;
	string tipo_de_pontuacao;
	char barra;
	char crescente;
	bool c;
	ifstream in;

	in.open(nome_ficheiro.c_str());

	while (!in.eof())
	{
		//in >> elementos_equipa;
		//in >> barra;

		getline(in, desporto);

		in >> tipo_de_pontuacao;
		in >> barra;
		in >> crescente;

		if (barra != '/' && !in.eof())
			return false;

		in.ignore(1000, '\n');
		in.ignore(1000, '\n');

		if (crescente == 'C')
			c = true;
		else if (crescente == 'D')
			c = false;
		else return false;;

		Desporto * ds = new Desporto(desporto, tipo_de_pontuacao, c);

		if (desporto != "")
			desportos.push_back(ds);
	}

	in.close();
	return true;
}


bool Campeonato::loadEquipas(string nome_ficheiro)
{
	ifstream in;
	unsigned int k = 0;
	string extraido = " ";

	in.open(nome_ficheiro.c_str());

	while (!in.eof())
	{
		string eq, str, at;
		char genero;
		bool g;

		if (k == 0)
		{
			int c = in.peek();

			if (c == '\n')
				in.ignore(1000,'\n');

			getline(in, eq);
		}
		else
		{
			eq = extraido;

			int c = in.peek();

			if (c == '\n')
				in.ignore(1000,'\n');
			else
			{
				getline(in,str);
				eq = eq + str;
			}
		}
		k++;

		Equipa * equipa = new Equipa(eq);

		if (eq != "")
			adicionaEquipa(equipa);

		extraido = "";
		int c = in.peek();

		if (!in.eof() && c != '\n')
			do{
				in >> extraido;
				if (extraido == "-")
				{
					int c = in.peek();
					if (c == '\n')
						in.ignore(1000,'\n');

					getline(in, str);
					genero = str[str.size()-1];
					at = str.substr(1,str.size()-3);


					if (genero == 'M')
						g= true;
					else if (genero == 'F')
						g = false;
					else return false;

					Atleta * atleta = new Atleta(at, equipa, g);

					int indiceEquipa = -1;

					for (unsigned int i = 0; i < equipas.size(); i++)
					{
						if (comparar_strings(eq, equipas[i]->getNome()))
							indiceEquipa = i;
					}
					if (indiceEquipa == -1 && eq != "")
						throw Equipa::EquipaInexistente(eq);

					if (eq != "")
						equipas[indiceEquipa]->adicionaAtleta(atleta);
				}
					} while(extraido == "-" && !in.eof());
	}
	in.close();
	return true;
}

bool Campeonato::loadModalidades(string nome_ficheiro)
{
	ifstream in;

	in.open(nome_ficheiro.c_str());

	while (!in.eof())
	{
		string desporto, d;
		string modalidade;
		int horas;
		int minutos;
		int indice = -1;

		getline (in, desporto);

		desporto = tirar_espacos_fim(desporto);

		for (unsigned int i = 0; i < desportos.size(); i++)
			if (comparar_strings(desportos[i]->getNome(), desporto))
				indice = i;

		if (indice == -1 && desporto != "")
			{
			throw Desporto::DesportoInexistente(desporto);
			}

		getline(in, modalidade);
		modalidade = tirar_espacos_fim(modalidade);

		char c = in.peek();

		if (c == '\n')
			in.ignore(1000,'\n');

		in >> horas;
		in >> minutos;

		in.ignore(1000,'\n');
		in.ignore(1000,'\n');

		Modalidade * m = new Modalidade(modalidade, horas, minutos, desportos[indice]);
		if (desporto != "")
			desportos[indice]->adicionaModalidade(m);

	}
	in.close();
}

bool Campeonato::loadProvas(string nome_ficheiro)
{
	ifstream in;
	unsigned int k = 0;
	string extraido = " ";

	in.open(nome_ficheiro.c_str());

	while(!in.eof())
	{
		string mod, at, str;
		int dia, mes, ano, horas, minutos;
		char genero, barra;

		int i_desporto = -1;
		int i_modalidade = -1;

		if (k == 0)
			{
			int c = in.peek();

			if (c == '\n')
				in.ignore(1000,'\n');

			getline(in, mod);
			}
		else
			{
			mod = extraido;

			int c = in.peek();

			if (c == '\n')
				in.ignore(1000,'\n');
			else
				{
				getline(in,str);
				mod = mod + str;
				}
			}

		mod = tirar_espacos_fim(mod);

		k++;

		for (unsigned int i = 0; i < desportos.size(); i++)
		{
			for(unsigned int j = 0; j < desportos[i]->getModalidades().size(); j++)
			{
				if (comparar_strings(mod,desportos[i]->getModalidades()[j]->getNome()))
				{
					i_desporto = i;
					i_modalidade = j;
				}
			}
		}

		if (i_modalidade == -1 && mod != "")
		{
			throw Modalidade::ModalidadeInexistente(mod);
		}

		in >> dia >> mes >> ano;
		in >> barra;
		in >> horas >> minutos;
		in >> barra;
		in >> genero;

		Data d(ano,mes,dia);
		Hora h(horas,minutos);

		Prova * p = new Prova(desportos[i_desporto]->getModalidades()[i_modalidade], d, h, genero);

		extraido = " ";

		if (!in.eof())
			do{
				in >> extraido;
				if (extraido == "-")
				{
					int c = in.peek();

					if (c == '\n')
						in.ignore(1000,'\n');

					getline(in, at);
					at = at.substr(1,at.size()-1);

					for(unsigned int i = 0; i < equipas.size();i++)
					{
						for(unsigned int j = 0; j < equipas[i]->getAtletas().size();j++)
						{
							if (comparar_strings(at,equipas[i]->getAtletas()[j]->getNome()) && at != "")
							{
								equipas[i]->getAtletas()[j]->adicionaProva(p);
								p->adicionaAtleta(equipas[i]->getAtletas()[j]);
								equipas[i]->adicionaDesporto(desportos[i_desporto]);
								equipas[i]->getAtletas()[j]->adicionaModalidade(desportos[i_desporto]->getModalidades()[i_modalidade]);
							}

						}
					}
				}
			} while(extraido == "-" && !in.eof());


		adicionaProva(*p);
	}
}

void Campeonato::updateDesportos(string nome_ficheiro)
{
	ofstream out;

	out.open(nome_ficheiro.c_str());

	if(!ficheiroExiste(nome_ficheiro))
		throw FicheiroInexistente(nome_ficheiro);

	for(unsigned int i = 0; i < desportos.size(); i++)
	{
		out << desportos[i]->getNome() << endl;
		out << desportos[i]->getPontuacao() << " / ";
		if (desportos[i]->isCrescente())
			out << "C" << endl;
		else out << "D" << endl;
		out << endl;
	}
}

void Campeonato::updateEquipas(string nome_ficheiro)
{
	ofstream out;
	out.open(nome_ficheiro.c_str());

	if(!ficheiroExiste(nome_ficheiro))
		throw FicheiroInexistente(nome_ficheiro);

	for(unsigned int i = 0; i < equipas.size();i++)
	{
		out << equipas[i]->getNome() << endl;
		for(unsigned int j = 0; j < equipas[i]->getAtletas().size(); j++)
		{
			out << "- " << equipas[i]->getAtletas()[j]->getNome() << " ";
			if (equipas[i]->getAtletas()[j]->getGenero())
				out << "M" << endl;
			else out << "F" << endl;
		}
		out << endl;
	}
}

void Campeonato::updateModalidades(string nome_ficheiro)
{
	ofstream out;
	out.open(nome_ficheiro.c_str());

	if(!ficheiroExiste(nome_ficheiro))
		throw FicheiroInexistente(nome_ficheiro);

	for(unsigned int i = 0; i < desportos.size();i++)
	{
		for(unsigned int j = 0; j < desportos[i]->getModalidades().size(); j++)
		{
			out << desportos[i]->getNome() << endl;
			out << desportos[i]->getModalidades()[j]->getNome() << endl;
			out << desportos[i]->getModalidades()[j]->getDuracao().getHoras() << " ";
			out << desportos[i]->getModalidades()[j]->getDuracao().getMinutos() << endl;
			out << endl;
		}
	}
}

void Campeonato::updateProvas(string nome_ficheiro)
{
	ofstream out;
	out.open(nome_ficheiro.c_str());

	if(!ficheiroExiste(nome_ficheiro))
		throw FicheiroInexistente(nome_ficheiro);

	for(unsigned int i = 0; i < provas.size(); i++)
	{
		out << provas[i]->getModalidade()->getNome() << endl;
		out << provas[i]->getData().getDia() << " ";
		out << provas[i]->getData().getMes() << " ";
		out << provas[i]->getData().getAno() << " / ";
		out << provas[i]->getInicio().getHoras() << " ";
		out << provas[i]->getInicio().getMinutos() << " / ";
		if (provas[i]->getGenero())
			out << "M" << endl;
		else out << "F" << endl;

		for(unsigned int j = 0; j < provas[i]->getAtletas().size(); j++)
		{
			out << "- " << provas[i]->getAtletas()[j]->getNome() << endl;
		}
		out << endl;
	}
}
/*
void Campeonato::apagaDesporto(string n)
{
	bool encontrado = false;
	for (unsigned int i = 0; i < desportos.size(); i++)
	{
		if ( desportos[i]->getNome() == n)
		{
			desportos.erase(desportos.begin()+i);
			encontrado = true;
		}
	}

	for(unsigned int i = 0; i < equipas.size(); i++)
	{
		for(unsigned int j= 0; j < equipas[i]->getDesportos().size(); j++)
		{
			if (n == equipas[i]->getDesportos()[j]->getNome())
				{
				equipas[i]->apagaDesporto(j);
				}
		}

	}
}
*/

void Campeonato::apagaModalidade(string n)
{
	for (unsigned int i = 0; i < desportos.size(); i++)
	{
		for(unsigned int j = 0; j < desportos[i]->getModalidades().size();j++)
		{
			if (desportos[i]->getModalidades()[j]->getNome() == n)
			{
				desportos[i]->apagaModalidade(j);
				j--;
			}
		}
	}

	for(unsigned int i = 0; i < equipas.size(); i++)
	{
		for(unsigned int j= 0; j < equipas[i]->getDesportos().size(); j++)
		{
			for(unsigned int k = 0; k < equipas[i]->getDesportos()[j]->getModalidades().size(); k++)
			if (n == equipas[i]->getDesportos()[j]->getModalidades()[k]->getNome())
			{
				equipas[i]->apagaModalidade(j,k);
				k--;
			}
		}

		for(unsigned int j = 0; j < equipas[i]->getAtletas().size(); j++)
		{
			for(unsigned int k = 0; k < equipas[i]->getAtletas()[j]->getModalidades().size(); k++)
			{
				if (equipas[i]->getAtletas()[j]->getModalidades()[k]->getNome() == n)
				{
					equipas[i]->apagaModalidade(j,k);
					k--;
				}
			}
		}
	}
}

void Campeonato::menuApagar(){
	bool exit = false;
	int ch, ch2, ch3;

	system("cls");
	vector<string> choices;
	choices.push_back("Desportos");
	choices.push_back("Modalidades");
	choices.push_back("Equipas");
	choices.push_back("Atletas");
	choices.push_back("Provas");
	while (!exit){
		system("cls");
		ch = fazMenu("Campeonato Polidesportivo - Planeamento", choices);
		if (ch == -1)
			exit = true;
		else if(ch == 0){
			bool exit2 = false;
			while (!exit2){
				system("cls");
				ch2 = fazMenu("Desportos:", desportos);
				if (ch2 == -1){
					exit2 = true;
					continue;
				}

				vector<Equipa*> equipas_desp;

				for (unsigned int i = 0; i < equipas.size(); i++)
					for (unsigned int j = 0; j < equipas[i]->getDesportos().size(); j++)
						if (equipas[i]->getDesportos()[j] == desportos[ch2]){
							equipas_desp.push_back(equipas[i]);
						}

				if (equipas_desp.size() != 0)
				{
					system("cls");
					cout << "Ainda existem equipas subscritas a este desporto.\n";
					cout << endl;
					cout << "Por favor elimine as seguintes equipas:\n";

					for(unsigned int i = 0; i < equipas_desp.size(); i++)
					{
						cout << equipas_desp[i]->getNome() << endl;
					}

					_getch();
					return;

				}

				if (desportos[ch2]->getModalidades().size() != 0)
				{
					bool exit3 = false;
					while (!exit3){
						system("cls");

						cout << desportos[ch2]->getNome();
						cout << " ainda tem modalidades.\n";
						cout << endl;

						ch3 = fazMenu("Modalidades a Retirar:", desportos[ch2]->getModalidades());
						if (ch3 == -1){
							exit3 = true;
							continue;
						}
						else{
							apagaModalidade(desportos[ch2]->getModalidades()[ch3]->getNome());
							if (desportos[ch2]->getModalidades().size() == 0)
								desportos.erase(desportos.begin()+ch2);
							return;
						}
					}
				}

				desportos.erase(desportos.begin()+ch2);
				return;
			}
		}

		else if(ch == 1){
			bool exit2 = false;
			while (!exit2){
				system("cls");
				ch2 = fazMenu("Desportos:", desportos);
				if (ch2 == -1){
					exit2 = true;
					continue;
				}
				bool exit3 = false;
				while (!exit3){
					system("cls");

					if (desportos[ch2]->getModalidades().size() == 0)
					{
						system("cls");
						cout << "Nao existem mais modalidades neste desporto.";
						_getch();
						return;

					}else {
						ch3 = fazMenu("Modalidades:", desportos[ch2]->getModalidades());
						if (ch3 == -1){
							exit3 = true;
							continue;
					}
					}

					vector<Prova*> provas_mod;

					for (int i = 0; i < provas.size(); i++)
						if (provas[i]->getModalidade() == desportos[ch2]->getModalidades()[ch3]){
							provas_mod.push_back(provas[i]);
						}

					if (provas_mod.size() != 0)
					{
						system("cls");
						cout << "Ainda existem provas subscritas a esta modalidade.\n";
						cout << endl;
						cout << "Por favor elimine as seguintes equipas:\n";

						for(unsigned int i = 0; i < provas_mod.size(); i++)
						{
							cout << *provas_mod[i] << endl;
						}

						_getch();
						return;

					}
					apagaModalidade(desportos[ch2]->getModalidades()[ch3]->getNome());
					return;
				}
			}
		}

		else if(ch == 2){
			bool exit2 = false;
			while (!exit2){
				system("cls");
				ch2 = fazMenu("Equipas:", equipas);
				if (ch2 == -1){
					exit2 = true;
					continue;
				}
				if (equipas[ch2]->getAtletas().size() != 0)
				{
					bool exit3 = false;
					while (!exit3){
						system("cls");

						cout << equipas[ch2]->getNome();
						cout << " ainda tem atletas.\n";
						cout << endl;

						ch3 = fazMenu("Atletas a Retirar:", equipas[ch2]->getAtletas());
						if (ch3 == -1){
							exit3 = true;
							continue;
						}
						else{
							equipas[ch2]->apagaAtleta(equipas[ch2]->getAtletas()[ch3]->getNome());
							if (equipas[ch2]->getAtletas().size() == 0)
								equipas.erase(equipas.begin()+ch2);
							return;
						}
					}
				}
				return;
			}
		}

		else if(ch == 3){
			bool exit2 = false;
			while (!exit2){
				system("cls");
				ch2 = fazMenu("Equipas:", equipas);
				if (ch2 == -1){
					exit2 = true;
					continue;
				}
				bool exit3 = false;
				while (!exit3){
					system("cls");
					ch3 = fazMenu("Atletas:", equipas[ch2]->getAtletas());
					if (ch3 == -1){
						exit3 = true;
						continue;
					}
					if (provas[ch2]->getAtletas()[ch3]->getProvas().size() != 0){
						{
							bool exit4 = false;
							while (!exit4 && provas[ch2]->getAtletas()[ch3]->getProvas().size() != 0){
								system("cls");
								cout << provas[ch2]->getAtletas()[ch3]->getNome();
								cout << " ainda esta inscrito em provas.\n";
								cout << endl;

								int ch4 = fazMenu("Provas a Retirar:", provas[ch2]->getAtletas()[ch3]->getProvas());
								if (ch4 == -1)
								{
									exit4 = true;
									continue;
								}
								else{
									provas[ch2]->getAtletas()[ch3]->apagaProva(ch4);
									if (provas[ch2]->getAtletas()[ch3]->getProvas().size() == 0)
										equipas[ch2]->apagaAtleta(equipas[ch2]->getAtletas()[ch3]->getNome());
									return;
								}
							}
						}
						return;
					}
				}
			}
		}

		else{
			bool exit2 = false;
			while (!exit2){
				system("cls");
				ch2 = fazMenu("Provas:", provas);
				if (ch2 == -1){
					exit2 = true;
					continue;
				}

				if (provas[ch2]->getAtletas().size() != 0){
					{
						bool exit3 = false;
						while (!exit3 && provas[ch2]->getAtletas().size() != 0){
							system("cls");
							cout << *provas[ch2];
							cout << endl;
							cout << "Esta prova ainda tem atletas.\n";
							cout << endl;

							int ch3 = fazMenu("Selecionar Atletas:", provas[ch2]->getAtletas());
							if (ch3 == -1)
								{
								exit3 = true;
								continue;
								}
							else{
								provas[ch2]->apagaAtleta(provas[ch2]->getAtletas()[ch3]->getNome());
								if (provas[ch2]->getAtletas().size() == 0)
									provas.erase(provas.begin()+ch2);
								return;
							}
						}
					}
					return;
				}
			}
			//return;
		}
	}
}

void Campeonato::menuCriacao(){
	bool exit = false;
	while (!exit){
		system("cls");
		vector<string> choices;
		choices.push_back("Desportos");
		if(desportos.size() > 0){
			choices.push_back("Equipas");
			choices.push_back("Provas");
			choices.push_back("Apagar");
			choices.push_back("Salvar");
			choices.push_back("Terminar Planeamento");
		}

		int ch = fazMenu("Campeonato Polidesportivo - Planeamento", choices);
		if (ch == -1)
			exit = true;
		else if (ch == 0)
			menuDesportos();
		else if (ch == 1)
			menuEquipas();
		else if (ch == 2)
			menuProvas();
		else if (ch == 3)
			menuApagar();
		//		else if (ch == 4)
		//			Salvar();
		//		else
		//			TerminarPlaneamento();
	}
}

void Campeonato::menuDesportos(){
	bool exit = false;
	while (!exit){
		system("cls");
		int ch = fazMenu("Desportos:", desportos, "Novo Desporto");
		if (ch == -1)
			exit = true;
		else if (ch < desportos.size())
			desportos[ch]->menu();
		else{
			try{
				adicionaDesporto();
			}
			catch (DesportoExiste d){
				cout << "Desporto \"" << d.getNome() << "\" ja existe.";
				_getch();
			}

		}

	}
}

void Campeonato::menuEquipas(){
	bool exit = false;
	while (!exit){
		system("cls");
		int ch = fazMenu("Equipas:", equipas, "Nova Equipa");
		if (ch == -1)
			exit = true;
		else if (ch < equipas.size())
			equipas[ch]->menu(desportos);
		else{
			try{
				adicionaEquipa();
			}
			catch (EquipaExiste eq){
				cout << "Equipa \"" << eq.getNome() << "\" ja existe.";
				_getch();
			}
		}
	}
}

void Campeonato::menuProvas(){
	bool exit = false;
	while (!exit){
		system("cls");
		int ch = fazMenu("Provas:", provas, "Nova Prova");
		if (ch == -1)
			exit = true;
		else if (ch < provas.size())
			provas[ch]->menu(equipas, desportos);
		else
			adicionaProva();
	}
}

void Campeonato::adicionaDesporto(){
	system("cls");

	cout << "Novo Desporto\n";

	string n;
	cout << "Nome: ";
	getline(cin, n);
	//	cin.ignore(10000,'\n');

	string p;
	cout << "Tipo de pontuacao: ";
	getline(cin, p);
	//	cin.ignore(10000,'\n');

	int ch;
	vector<string> choices;
	choices.push_back("Sim");
	choices.push_back("Nao");
	ch = fazMenu("A pontuacao e crescente? (Valores maiores sao melhores?)", choices);
	if (ch == -1)
		return;

	Desporto *d = new Desporto(n, p, !ch);
	if (search(desportos , *d) != -1)
		throw DesportoExiste(n);
	while(true){
		try{
			d->adicionaModalidade();
			break;
		}
		catch (Desporto::ModalidadeExiste m){
			cout << "A modalidade " << m.getNome() << " ja existe.\n";
		}
	}
	desportos.push_back(d);
}

void Campeonato::adicionaEquipa(){
	system("cls");

	cout << "Nova Equipa\n";

	string n;
	cout << "Nome: ";
	getline(cin, n);

	Equipa *eq = new Equipa(n);
	if (search(equipas , *eq) != -1)
		throw EquipaExiste(n);
	equipas.push_back(eq);
}

void Campeonato::adicionaProva(){
	system("cls");
	int ch, ch2;
	while(true){
		system("cls");
		ch = fazMenu("Desportos:", desportos);
		if (ch == -1)
			return;
		else{
			system("cls");
			ch2 = fazMenu("Modalidades:", desportos[ch]->getModalidades());
			if (ch2 != -1)
				break;
		}
	}

	Modalidade* mod = desportos[ch]->getModalidades()[ch2];

	int d, m, a, h, min;
	Data data;
	Hora hora;
	//Data---------------------
	while (true){
		system("cls");

		cout << "Ano: ";
		while (!(cin >> a)){
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Input invalido!\n";
			cout << "Ano: ";
		}
		cin.ignore(1000, '\n');

		cout << "Mes: ";
		while (!(cin >> m)){
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Input invalido!\n";
			cout << "Mes: ";
		}
		cin.ignore(1000, '\n');

		cout << "Dia: ";
		while (!(cin >> d)){
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Input invalido!\n";
			cout << "Dia: ";
		}
		cin.ignore(1000, '\n');

		try{
			data = Data(a,m,d);
			if (data < inicio || fim < data)
				throw Data::DataInvalida(a,m,d);
			break;
		}
		catch (Data::DataInvalida D){
			cout << D.getMessage();
			cout << "Datas validas sao desde " << inicio << " ate " << fim << ".\n";
			_getch();
		}
	}



	while (true){
		system("cls");
		cout << "Horas de Inicio: ";
		while (!(cin >> h))
		{
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Input invalido!\n";
			cout << "Horas de Inicio: ";
		}
		cin.ignore(1000, '\n');

		cout << "Minutos: ";
		while (!(cin >> min))
		{
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Input invalido!\n";
			cout << "Minutos: ";
		}
		cin.ignore(1000, '\n');


		try{
			hora = Hora(h,min);
			if (hora < abertura || fecho < hora+mod->getDuracao())
				throw Hora::HoraInvalida(h,min);
			break;
		}
		catch (Hora::HoraInvalida H){
			cout << H.getMessage();
			cout << "Datas validas sao desde " << abertura << " ate " << fecho << ".\n";
			_getch();
		}
	}

	int g;
	vector<string> choices;
	choices.push_back("Masculino");
	choices.push_back("Feminino");
	g = fazMenu("Genero?", choices);
	if (g == -1)
		return;


	Prova p(mod, data, hora,g);
	for (unsigned int i = 0; i < provas.size(); i++){
		if (provas[i]->Simultaneo(p)){
			cout << "Ja existem provas do mesmo tipo de desporto para o horario marcado.\n";
			_getch();
		}

	}


	provas.push_back(&p);

}

void atribuiPontuacao(ProvaTerminada &pro, vector<float> pontos) {
//ordena o vetor atletas por pontuacao e faz pushback das pontuacoes ordenadas no pontuacoes da provaTerminada
	//e faz push:_back dos atletas ordenados tb

	for (unsigned int i = 0; i < pontos.size() - 1; i++)
		for (unsigned int j = i; j < pontos.size(); j++) {
			if (pontos[i] < pontos[j]) {
				swap(pontos[i], pontos[j]); // ou assim paracido, seja como for trocarlos
				swap(pro.getAtletas()[i], pro.getAtletas()[j]); // assim os atletas tb sao ordenados
			}
		}

	if (pro.getAtletas().size() < 2) {
		cout << pro.getAtletas().size();
		pro.getAtletas()[0]->adicionaPontuacao(3);
	} else {
		pro.getAtletas()[0]->adicionaPontuacao(3);
		pro.getAtletas()[1]->adicionaPontuacao(2);
		pro.getAtletas()[2]->adicionaPontuacao(1);

	}

	for (unsigned int k = 0; k < pontos.size(); k++)
		pro.getPontuacoes().push_back(pontos[k]);
}


void Campeonato::adicionaEquipa(Equipa &eq){
	int indice = -1;
	for(unsigned int i = 0; i < equipas.size(); i++)
	{
		if (equipas[i]->getNome() == eq.getNome())
			indice = i;
	}

	if (indice == -1)
		equipas.push_back(&eq);
}

void Campeonato::listaDesportos() const{
	vector<string> ndes;

	cout << "Desportos no campeonato: " << endl;

	for (unsigned int i = 0; i < desportos.size(); i++)
		ndes.push_back(desportos[i]->getNome());

	insertionSort<string>(ndes);

	for (unsigned int j = 0; j < desportos.size(); j++)
		cout << desportos[j]->getNome() << endl;
}

void Campeonato::listaProvasNaoRealizadas() const{
	vector<Prova> vprova;

	cout << "Provas por realizar no campeonato: " << endl;

	for (unsigned int j = 0; j < provas.size();j++)
		if(provas[j]->getRealizada() == 0)

		vprova.push_back(*provas[j]);


	if (vprova.size() == 0)
		cout <<endl << "Ja foram realizadas todas as provas" <<endl <<endl;
	else{
	insertionSort<Prova>(vprova);

	for (unsigned int i = 0; i < vprova.size(); i++)
			cout << vprova[i].getData() << ": Prova de " << (*vprova[i].getModalidade()->getDesporto())
			<< "("<<(*vprova[i].getModalidade())<<")"
			<< " as " << vprova[i].getInicio() << endl;}
}

void Campeonato::listaProvasRealizadas() const{
	vector<Prova> vprova;

		cout << "Provas ja realizadas no campeonato: " << endl;

		for (unsigned int j = 0; j < provas.size();j++)
			if(provas[j]->getRealizada() == 1)
				vprova.push_back(*provas[j]);

		if (vprova.size() == 0)

		cout <<endl << "Ainda nao foram realizadas provas" <<endl <<endl;
		else{
		insertionSort<Prova>(vprova);

		for (unsigned int i = 0; i < vprova.size(); i++)
			cout << vprova[i].getData() << ": Prova de " << (*vprova[i].getModalidade()->getDesporto())
			<< "("<<(*vprova[i].getModalidade())<<")"
			<< " as " << vprova[i].getInicio() << endl;}
}

void Campeonato::listaAtletas() const{
	vector<string> vat;

	cout << "Atletas no campeonato: " << endl;

	for (unsigned int j = 0; j < equipas.size();j++)
		for (unsigned int u = 0; u < equipas[j]->getAtletas().size(); u++)
			vat.push_back(equipas[j]->getAtletas()[u]->getNome());

	insertionSort<string>(vat);

	for (unsigned int i = 0; i < vat.size(); i++)
		cout << vat[i] << endl;
}

void Campeonato::listaAtletasEquipa() const{
	//vector<string> vat;

	cout << "Atletas no campeonato: " << endl;

	for (unsigned int j = 0; j < equipas.size(); j++) {
		vector<string> vat;
		cout << "Equipa " << equipas[j]->getNome() << ":" << endl;
		for (unsigned int u = 0; u < equipas[j]->getAtletas().size(); u++)
			vat.push_back(equipas[j]->getAtletas()[u]->getNome());

		insertionSort<string>(vat);

		for (unsigned int i = 0; i < vat.size(); i++)
			cout << vat[i] << endl;
	}

}

void Campeonato::listaAtletasColocacao() const {
	vector<Atleta> vat;

	cout << "Atletas no campeonato por colocacao: " << endl;

	for (unsigned int j = 0; j < equipas.size(); j++)
		for (unsigned int u = 0; u  < equipas[j]->getAtletas().size(); u++)
			vat.push_back((*equipas[j]->getAtletas()[u]));

	insertionSort<Atleta>(vat);

	for (unsigned int i = 0; i < vat.size(); i++)
		cout << vat[i] << "(" << vat[i].getPontos() << ")"<< endl;
}

void Campeonato::listaAtletasDesporto() const {
	vector<Atleta> vat;

	for (unsigned int j = 0; j < equipas.size(); j++)
		for (unsigned int u = 0; u < equipas[j]->getAtletas().size(); u++)
			vat.push_back((*equipas[j]->getAtletas()[u]));

	cout << "Atletas no campeonato: " << endl << endl;

	for (unsigned int j = 0; j < desportos.size(); j++) {
		vector<Atleta> vatd;

		for (unsigned int u = 0; u < vat.size(); u++) //percorre os atletas
			for (unsigned int a = 0;
					a < vat[u].getEquipa()->getDesportos().size(); a++) //percorre os seus desportos
				if (desportos[j]->getNome()
						== vat[u].getEquipa()->getDesportos()[a]->getNome()) //desporto = desporto do a atleta
					vatd.push_back(vat[u]);

		if (vatd.size() == 0)
			cout <<endl << "Nao ha atletas  em " << desportos[j]->getNome() <<endl;
		else {
			insertionSort<Atleta>(vatd);
			cout <<endl<< "Atletas em " << desportos[j]->getNome() << ":" << endl << endl;
			for (unsigned int i = 0; i < vatd.size(); i++)
				cout << vatd[i] << endl;
		}
	}
}

void Campeonato::listaEquipasColocacao() const {
	vector<pair<int, string> > peq (equipas.size(), make_pair(0, ""));
	vector<int> peqb (equipas.size());

	for (unsigned int i = 0; i < equipas.size(); i++) {

		peq[i].second = equipas[i]->getNome();
		for (unsigned int j = 0; j < equipas[i]->getAtletas().size(); j++) {

			peq[i].first += equipas[i]->getAtletas()[j]->getPontos();
			peqb[i] += equipas[i]->getAtletas()[j]->getPontos();
		}
	}
	insertionSort<int>(peqb);

	cout << "Ranking de Equipas :" << endl << endl;

	for (unsigned int a = 0; a < peq.size(); a++)

		for (unsigned int b = 0; b < peqb.size(); b++)
			if (peqb[b] == peq[a].first)
				cout<< peq[b].second << endl;

}

void Campeonato::listaAtletasModalidade() const{
//	vector< pair <string, string> > vam; // first = nome atleta sec= nome modalidade
//
//	for (unsigned int j = 0; j < equipas.size(); j++)
//		for (unsigned int u = 0; u < equipas[j]->getAtletas().size(); u++)
//			vat.push_back(make_pair(*equipas[j]->getAtletas()[u])),equipas[j].getAtletas()[u].getModalidade); //vetor com todos atletas
//
//	cout << "Atletas no campeonato: " << endl << vat.size();
//
//
//	for (unsigned int a = 0; a < desportos.size(); a++) {
//		for (unsigned int b = 0; b < desportos[a]->getModalidades().size();b++)
//			{vector <Atleta> vat;
//			for (unsigned int c=0;c< desportos[a]->getModalidades()[b].getAtletas().size();c++)
//				vat.push_back( desportos[a]->getModalidades()[b]->getAtletas()[c])
//
//			}

	vector<Atleta> vat;

	for (unsigned int j = 0; j < equipas.size(); j++)
		for (unsigned int u = 0; u < equipas[j]->getAtletas().size(); u++)
			vat.push_back((*equipas[j]->getAtletas()[u]));


	for (unsigned int j = 0; j < desportos.size(); j++) {
		vector<Atleta> vatm;
		for (unsigned int w = 0; w < desportos[j]->getModalidades().size(); w++) //percorre as modalidades todas
				{
			for (unsigned int u = 0; u < vat.size(); u++) //percorre os atletas
				for (unsigned int b = 0; b < vat[u].getModalidades().size();
						b++) // percorre modalidades dos atletas
						{
					if (desportos[j]->getModalidades()[w]->getNome()
							== vat[u].getModalidades()[b]->getNome())
						vatm.push_back(vat[u]);
				}

			if (vatm.size() == 0)
				cout << endl << "Nao ha atletas  em "
						<< desportos[j]->getModalidades()[w]->getNome() << endl;
			else {
				insertionSort<Atleta>(vatm);
				cout << endl << "Atletas em "
						<< desportos[j]->getModalidades()[w]->getNome() << " ("
						<< desportos[j]->getNome() << ") " << ":" << endl
						<< endl;
				for (unsigned int i = 0; i < vatm.size(); i++)
					cout << vatm[i] << endl;
			}
		}
	}
}

bool Campeonato::realizaProva(Prova &p , vector <float> pontuacoes){
	Hora hi (p.getInicio().getHoras(),p.getInicio().getMinutos());

	for (vector < Prova*>::iterator it = provas.begin(); it < provas.end();it++)
		if ((*it) == &p)
			{
			ProvaTerminada * nova = new ProvaTerminada(p.getModalidade(), p.getData(), hi, p.getGenero());
			nova->setAtletas(p.getAtletas());
			atribuiPontuacao(*nova, pontuacoes);
			provas.push_back(nova);
			it = provas.erase(it);

			return true;
			}
		else if ((!((*it) == &p))&& (it++)==provas.end())
			return false;
return false;

}
