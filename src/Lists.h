/*
 * Lists.h
 *
 *  Created on: 19/10/2015
 *      Author: Filipe
 */

#ifndef SRC_LISTS_H_
#define SRC_LISTS_H_


#include "Lists.h"
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
using namespace std;

void gotoxy(int x, int y);

int wherey();

template <class T>
int fazMenu(string titulo, vector<T*> options);

template <class T>
int fazMenu(string titulo, vector<T*> options, string Op1);

template <class T>
int search(const vector<T*> &v, T x);




//Menu implementation
template <class T>
int fazMenu(string titulo, vector<T*> options){
	cout << titulo << endl << endl;
	int inicio = wherey();
	for (int i = 0; i < options.size(); i++)
		cout << "   " << (*options[i]) << endl;
	gotoxy(0, inicio);
	int op(0);

	while (true)
	{
		gotoxy(0, inicio + op);
		cout << "->";
		gotoxy(0, inicio + op);

		switch (_getch())
		{
		case KEY_ESC:
			return -1;
			break;
		case KEY_UP:
			if (op > 0)
				op--;
			break;
		case KEY_DOWN:
			if (op < options.size()-1)
				op++;
			break;
		case KEY_ENTER:
			return op;
			break;
		default:
			break;
		}

		cout << "  ";

	}
}

template <class T>
int fazMenu(string titulo, vector<T*> options, string Op1){
	cout << titulo << endl << endl;
	int inicio = wherey();
	for (int i = 0; i < options.size(); i++)
		cout << "   " << (*options[i]) << endl;
	cout << "   " << Op1 << endl;
	gotoxy(0, inicio);
	int op(0);

	while (true)
	{
		gotoxy(0, inicio + op);
		cout << "->";
		gotoxy(0, inicio + op);

		switch (_getch())
		{
		case KEY_ESC:
			return -1;
			break;
		case KEY_UP:
			if (op > 0)
				op--;
			break;
		case KEY_DOWN:
			if (op < options.size())
				op++;
			break;
		case KEY_ENTER:
			return op;
			break;
		default:
			break;
		}

		cout << "  ";

	}
}

template <class T>
int search(const vector<T*> &v, T x)
{
    for (unsigned int i = 0; i < v.size(); i++)
        if ((*v[i]) == x)
           return i;
    return -1;
}
#endif /* SRC_LISTS_H_ */