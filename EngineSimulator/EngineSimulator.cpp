// EngineSimulator.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "iostream"
#include "string"
#include "iostream"
#include "sstream"
#include "fstream"
#include "direct.h"
#include "windows.h"
#include "comdef.h"
#include "vector"
#include "iterator"
#include "cmath"

using namespace std;

//int ConsoleIO()
//{
//	cout << "Engine Simulator" << endl << endl;
//	int menuPointer = 0;
//	char s[2];
//	while (menuPointer != 3)
//	{
//		cout << "    Menu" << endl;
//		cout << "1) Test Engine" << endl;
//		cout << "2) Add Engine" << endl;
//		cout << "3) Exit" << endl;
//		cout << "Type in menu item: ";
//
//		cin.getline(s, sizeof(s));
//		stringstream str;
//		str << s;
//		str >> menuPointer;
//
//		cout << endl << endl;
//
//		switch (menuPointer)
//		{
//		case 1: return 1;
//		case 2: return 2;
//		}
//
//	}
//	return -1;
//}


class Engine
{
public:
	string name; //Название двигателя 
	double I; // Момент инерции двигателя I (кг∙м^2)
	double M[6]; // Крутящий момент, вырабатываемый двигателем в Н∙м в 6 позициях графика
	double V[6]; // Скорость вращения в радиан/сек в 6 позициях графика
	double T; // Температура перегрева в C (градусах цельсия)
	double HM; // Коэффициент зависимости скорости нагрева от крутящего момента
	double HV; // Коэффициент зависимости скорости нагрева от скорости вращения коленвала
	double C; // Коэффициент зависимости скорости охлаждения от температуры двигателя и окружающей среды
	double currentM; // Текущий момент двигателя
	double currentV; // Текущая скорость вращения коленвала
	double currentTemperature; // Текущая температура двигателя


	// Так как двигатель работает без нагрузки, то весь вырабатываемый момент идет на раскрутку коленвала, его ускорение вычисляется просто : A = M / I
	// Скорость нагрева двигателя рассчитывать как Vнагрева = M * HM + V^2 * HV (градусов С в секунду)
	// Скорость охлаждения двигателя рассчитывать как Vохлаждения = C * (Tсреды - Tдвигателя) (градусов С в секунду)
};



int testEngine(double _ambientTemperature, Engine &_engine)
{
	_engine.currentTemperature = _ambientTemperature; //Текущая температура двигателя
	int time = 0;
	int maxTime = 3600;
	double heating;
	double cooling;
	int currentMVRatio = 0;
	_engine.currentM = _engine.M[currentMVRatio];
	_engine.currentV = _engine.V[currentMVRatio];
	double a = _engine.currentM / _engine.I;


	while (_engine.currentTemperature < _engine.T && time < maxTime)
	{
		time++;
		_engine.currentV += a;

		if (currentMVRatio < sizeof(_engine.M - 2))
		{
			currentMVRatio += _engine.currentV < _engine.V[currentMVRatio + 1] ? 0 : 1;
		}

		double spdUp = _engine.currentV - _engine.V[currentMVRatio];
		double spdDown = _engine.V[currentMVRatio + 1] - _engine.V[currentMVRatio];
		double spdFactor = _engine.M[currentMVRatio + 1] - _engine.M[currentMVRatio];

		_engine.currentM = spdUp / spdDown * spdFactor + _engine.M[currentMVRatio];

		heating = _engine.currentM * _engine.HM + _engine.currentV * _engine.currentV * _engine.HV;
		cooling = _engine.C * (_ambientTemperature - _engine.currentTemperature);

		_engine.currentTemperature += heating + cooling;

		a = _engine.currentM / _engine.I;

	}

	return time;
}



int getEnginesList(string (&_engines)[128])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile(L".\\Engines\\*txt", &FindFileData);
	CHAR fileNames[200][MAX_PATH];
	INT i = 0;
	if (hf == INVALID_HANDLE_VALUE) { return(-1);}
	do {
		strcpy_s(fileNames[i], _bstr_t(FindFileData.cFileName));
		_engines[i] = fileNames[i];
		//wcout << to_wstring(i) << ") " << fileNames[i] << endl;
		i++;
	} while (FindNextFile(hf, &FindFileData));
	FindClose(hf);

	return 0;
}



int getEngine(string name, Engine &_engine)
{
	string lines[128] = {};
	//memset(lines, 0, sizeof(lines));
	int i = 0;

	ifstream inputStream(".\\Engines\\" + name);
	if (inputStream.is_open())
	{
		while (getline(inputStream, lines[i]))
		{
			//Get engine
			//cout << lines[i] << endl;
			i++;
		}
	}
	else return -1;
	inputStream.close();
	
	//Some hardcode (can be replaced with is_digit())
	lines[0].erase(0, 5);
	lines[1].erase(0, 2);
	lines[2].erase(0, 2);
	lines[3].erase(0, 2);
	lines[4].erase(0, 2);
	lines[5].erase(0, 3);
	lines[6].erase(0, 3);
	lines[7].erase(0, 2);

	_engine.name = lines[0];
	_engine.I = stod(lines[1]);

	stringstream ss2(lines[2]);
	vector<double>  vd2;
	copy(istream_iterator<double>(ss2),
		istream_iterator<double>(), back_inserter(vd2));
	//copy(vd2.begin(), vd2.end(), ostream_iterator<double>(cout, " "));
	for (i = 0; i < 6; i++) _engine.M[i] = vd2[i];

	stringstream ss3(lines[3]);
	vector<double>  vd3;
	copy(istream_iterator<double>(ss3),
		istream_iterator<double>(), back_inserter(vd3));
	//copy(vd3.begin(), vd3.end(), ostream_iterator<double>(cout, " "));
	for (i = 0; i < 6; i++) _engine.V[i] = vd3[i];

	_engine.T = stod(lines[4]);
	_engine.HM = stod(lines[5]);
	_engine.HV = stod(lines[6]);
	_engine.C = stod(lines[7]);


	return 0;
}



int chooseEngine(Engine &_choosenEngine)
{
	string engines[128] = {};
	memset(engines, 0, sizeof(engines));

	getEnginesList(engines);
	
	cout << "List of Engines: " << endl;

	int i = 0;
	while (engines[i] != "")
	{
		engines[i].erase(engines[i].find_last_of('.'), engines[i].length());
		cout << to_string(i) + ")" << engines[i] << endl;
		i++;
	}

	
	char menuPointer = { 0 };
	while (!isdigit(menuPointer))
	{
		do
		{
			cout << endl << "Choose Engine from list: ";
			if (!(cin >> menuPointer))
			{
				cout << endl << "Please enter number of Engine." << endl;
				cin.clear();
				cin.ignore(10000, '\n');
			}
			if (!(((int)menuPointer - 48 < i) && ((int)menuPointer - 48 >=0))) // (int)menuPointer - 48 - is casting a (char) to int
			{
				cout << "Please choose existing number." << endl;
				cin.clear();
				cin.ignore(10000, '\n');
			}
			else continue;
		} while (!(((int)menuPointer - 48 < i) && ((int)menuPointer - 48 >= 0)));
	}
	cout << endl << "Getting Engine..." << endl << endl;

	string name(engines[(int)menuPointer - 48]); // (int)menuPointer - 48 - same casting

	if (getEngine(name + ".txt", _choosenEngine) != 0) return -1;

	cout << "Engine loaded." << endl << endl << "-----------------------" << endl << "Engine specifications:" << endl;

	cout << endl << "Название двигателя: " << _choosenEngine.name << endl << "Момент инерции двигателя I (кг/м2): " << _choosenEngine.I;
	cout << endl << "Крутящий момент M в Н∙м в 6 позициях графика: ";
	for (int i = 0; i < 6; i++) cout << to_string(_choosenEngine.M[i]) + " ";
	cout << endl << "Скорость вращения V в радиан/сек в 6 позициях графика: ";
	for (int i = 0; i < 6; i++) cout << to_string(_choosenEngine.V[i]) + " ";
	cout << endl << "Температура перегрева T в C (градусах цельсия): " << _choosenEngine.T;
	cout << endl << "Коэффициент зависимости HM скорости нагрева от крутящего момента: " << _choosenEngine.HM;
	cout << endl << "Коэффициент зависимости HV скорости нагрева от скорости вращения коленвала: " << _choosenEngine.HV;
	cout << endl << "Коэффициент зависимости C скорости охлаждения от температуры двигателя и окружающей среды: " << _choosenEngine.C << endl << endl;

	return 0;
}



int addEngine()
{
	//Тут можно попробовать взять свойства из класса Engine

	string name; //Название двигателя
	double I = 0; // Момент инерции двигателя I (кг∙м^2)
	double M[6]; // Крутящий момент, вырабатываемый двигателем в Н∙м в 6 позициях графика
	double V[6]; // Скорость вращения в радиан/сек в 6 позициях графика
	double T = 0; // Температура перегрева в C (градусах цельсия)
	double HM = 0; // Коэффициент зависимости скорости нагрева от крутящего момента
	double HV = 0; // Коэффициент зависимости скорости нагрева от скорости вращения коленвала
	double C = 0; // Коэффициент зависимости скорости охлаждения от температуры двигателя и окружающей среды

	char saving = { 0 };
	

		cout << "Engine will be saved to /Engines catalog." << endl;


		cout << "Введите название нового двигателя: ";
		cin >> name;

		cout << endl << "Момент инерции двигателя I (кг/м^2): ";
		cin >> I;

		cout << endl << "Крутящий момент M, вырабатываемый двигателем в Н∙м в 6 позициях графика через пробелы: ";
		
		for (int i = 0; i < 6; i++)
		{
			cin >> M[i];
		}
		/*string tempstring = "";
		string tempchar = "";
		int m = 0;
		for (int i = 0; i < sizeof(tempstring); i++)
		{
			if (isdigit(tempstring[i])) tempchar += tempstring[i];
			else
			{
				if (tempchar != "")
				{
					if (m <= 6) M[m] = stoi(tempchar); else continue;
					m++;
					tempchar = { 0 };
				}
			}
		}*/

		cout << endl << "Скорость вращения V в радиан/сек в 6 позициях графика через пробелы: ";
		for (int i = 0; i < 6; i++)
		{
			cin >> V[i];
		}
		/*tempstring = "";
		tempchar = "";
		m = 0;
		for (int i = 0; i < sizeof(tempstring); i++)
		{
			if (isdigit(tempstring[i])) tempchar += tempstring[i];
			else
			{
				if (tempchar != "")
				{
					if (m <= 6) M[m] = stoi(tempchar); else continue;
					m++;
					tempchar = { 0 };
				}
			}
		}*/

		cout << endl << "Температура перегрева T в C (градусах цельсия): ";
		cin >> T;

		cout << endl << "Коэффициент зависимости HM скорости нагрева от крутящего момента: ";
		cin >> HM;

		cout << endl << "Коэффициент зависимости HV скорости нагрева от скорости вращения коленвала: ";
		cin >> HV;

		cout << endl << "Коэффициент зависимости C скорости охлаждения от температуры двигателя и окружающей среды: ";
		cin >> C;

		cout << endl << endl << "Характеристики вашего двигателя" << endl << "Название двигателя: " << name << endl << "Момент инерции двигателя I (кг/м^2): " << I;
		cout << endl << "Крутящий момент M, вырабатываемый двигателем в Н∙м в 6 позициях графика: ";
			for (int i = 0; i < 6; i++) cout << to_string(M[i]) + " "; 
		cout << endl << "Скорость вращения V в радиан/сек в 6 позициях графика: ";
			for (int i = 0; i < 6; i++) cout << to_string(V[i]) + " ";
		cout << endl << "Температура перегрева T в C (градусах цельсия): " << T << endl << "Коэффициент зависимости HM скорости нагрева от крутящего момента: " << HM;
		cout << endl << "Коэффициент зависимости HV скорости нагрева от скорости вращения коленвала: " << HV;
		cout << endl << "Коэффициент зависимости C скорости охлаждения от температуры двигателя и окружающей среды: " << C << endl << endl;


		cout << "Save Engine? y/n " << endl;
		
		if (!(cin >> saving))
		{
			cout << endl << "Please enter y or n." << endl;
			cin.clear();
			cin.ignore(10000, '\n');
		}

		if (saving == 'n')
		{
			cin.clear();
			cin.ignore(10000, '\n');
			return 0;
		}

		if (saving == 'y')
		{
			cin.clear();
			cin.ignore(10000, '\n');
			cout << endl << "Saving engine file..." << endl << endl;
			_mkdir(".\\Engines");

			ofstream outStream;
			outStream.open(".\\Engines\\" + name + ".txt", ios::out);

			if (!outStream.is_open())
			{
				cout << "Error saving engine file." << endl << endl;
				return -1;
			}

			outStream << "Name:" + name << endl;

			outStream << "I:" + to_string(I) << endl;

			outStream << "M:";
			for (int i = 0; i < 6; i++) outStream << to_string(M[i]) + " ";

			outStream << endl << "V:";
			for (int i = 0; i < 6; i++) outStream << to_string(V[i]) + " ";

			outStream << endl << "T:" + to_string(T) << endl;

			outStream << "HM:" + to_string(HM) << endl;

			outStream << "HV:" + to_string(HV) << endl;

			outStream << "C:" + to_string(C);

			outStream.close();

			cout << "Engine successfully saved." << endl << endl;


	}

	return 0;
}




int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	cout << "Engine Simulator" << endl << endl;
	int menuPointer = 0;
	while (menuPointer != 3)
	{
		menuPointer = 0;
		cout << "-------Menu-------" << endl;
		cout << "1) Test Engine" << endl;
		cout << "2) Add Engine" << endl;
		cout << "3) Exit" << endl;
		cout << "Type in menu item: ";

		if (!(cin >> menuPointer)) 
		{
			cout << endl <<  "Please enter numbers only." << endl;
			cin.clear();
			cin.ignore(10000, '\n');
		}

		cout << endl << endl;

		switch (menuPointer)
		{
		case 1: //Test Engine 
		{
			Engine _engine;
			double _ambientTemperature = 0;
			double time = 0;

			if (chooseEngine(_engine) != 0)
			{
				cout << "Error getting Engine." << endl << endl;
				break;
			}

			cout << endl << endl << "Enter ambient temperatue: ";

			while (!(cin >> _ambientTemperature))
			{
				cout << "Please enter correct temperaure: ";
				cin.clear();
				cin.ignore(10000, '\n');
			}
			
			cout << endl << endl << "Starting to testing Engine." << endl;

			time = testEngine(_ambientTemperature, _engine);

			if (time >= 3600)
			{
				cout << endl << endl << "Engine temperature is stable." << endl << endl;
			}
			else cout << endl << endl << "Engine took " << time << "s to overheat." << endl << endl;

			break;
		}
		case 2: //Add Engine
		{
			addEngine();
			break;
		}
		case 3:	return 0; //Exit program
		default: cout << "Please choose menu item from 1 to 3." << endl << endl;
		}

	}
	return 0;
}

