/*********************************************************************************************
* Project Type: Console_Application                                                          *
* Project Name: C:\Users\24svi\CLionProjects\PLANE_PR                                        *
* File Name: main.cpp                                                                        *
* Language: C++, CLion                                                                       *
* Programmer: Ковалев Святослав Александрович                                                *
* Modified by:                                                                               *
* Created: 04.06.2026                                                                        *
* Last Revision: 04.06.2026                                                                  *
* Comment:                                                                                   *
* Тема: «Структуры и индексная сортировка»                                                   *
* 1) подготовить программу, осуществляющую печать таблицы о самолетах, совершающих посадку   *
* на каждом аэродроме в порядке убывания времени посадки (использовать индексную             *
* сортировку методом «пузырька»);                                                            *
* 2) обеспечить входной контроль времени посадки, бортового номера и аэродрома посадки,      *
* выполнить отладку и тестирование.                                                          *
* Чтение данных из файла производить с использованием функций ввода/вывода языка С++.        *
* Алгоритм должен быть параметризован; обмен данными с подпрограммой должен осуществляться   *
* только через параметры; исходные данные хранятся в отдельном файле.                        *
*********************************************************************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

// Максимальное количество записей
const int MAX_RECORDS = 100;

// Структура для хранения данных о самолете
struct Flight {
    int timeInMinutes; // Время в минутах от начала суток
    string timeStr;    // Исходная строка времени (ЧЧ:ММ)
    string model;      // Марка ЛА
    string tailNum;    // Бортовой номер
    int airportNum;    // Номер аэродрома (1, 2 или 3)
};

// --- ПРОТОТИПЫ ФУНКЦИЙ ---
bool validateAndParseTime(const string& timeStr, int& timeInMinutes);
bool validateAndParseAirport(const string& aptStr, int& aptNum);
void indexBubbleSort(const Flight arr[], int indices[], int n);

// --- ГЛАВНАЯ ФУНКЦИЯ ---
int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // ==========================================
    // ИМЯ ФАЙЛА ДЛЯ ТЕСТИРОВАНИЯ (меняй здесь)
    string filename = "data8.txt";
    // ==========================================

    Flight records[MAX_RECORDS];
    int indices[MAX_RECORDS];
    int count = 0;

    // Открываем файл с исходными данными
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка: не удалось открыть файл " << filename << endl;
        return 1;
    }

    string tStr, mod, tail, aptStr;
    // Считываем данные из файла до конца
    while (file >> tStr >> mod >> tail >> aptStr) {
        if (count >= MAX_RECORDS) {
            cout << "Предупреждение: превышено максимальное количество записей (" << MAX_RECORDS << ")." << endl;
            break;
        }

        int timeMins = 0;
        int aptNum = 0;

        // --- Входной контроль ---
        if (!validateAndParseTime(tStr, timeMins)) {
            cout << "[Пропущено] Ошибка формата времени: " << tStr << " (" << mod << ")" << endl;
            continue;
        }
        if (tail.length() < 3) {
            cout << "[Пропущено] Некорректный бортовой номер: " << tail << endl;
            continue;
        }
        if (!validateAndParseAirport(aptStr, aptNum)) {
            cout << "[Пропущено] Неверный аэродром: " << aptStr << " (допустимо 1, 2, 3 или АП1, АП2, АП3)" << endl;
            continue;
        }

        // Если данные корректны, сохраняем их в массив
        records[count].timeStr = tStr;
        records[count].timeInMinutes = timeMins;
        records[count].model = mod;
        records[count].tailNum = tail;
        records[count].airportNum = aptNum;

        // Инициализируем массив индексов
        indices[count] = count;
        count++;
    }
    file.close();

    cout << "\n--- Результаты сортировки (Файл: " << filename << ") ---\n\n";

    // Вызов функции сортировки
    indexBubbleSort(records, indices, count);

    // Печать таблицы по каждому аэродрому
    for (int currentAirport = 1; currentAirport <= 3; currentAirport++) {
        cout << " АЭРОДРОМ № " << currentAirport << " " << endl;
        cout << "Время\tМарка\t\tБорт.номер" << endl;
        cout << "-----------------------------------" << endl;

        bool found = false;
        // Проходим по отсортированным индексам
        for (int i = 0; i < count; i++) {
            int idx = indices[i];
            if (records[idx].airportNum == currentAirport) {
                found = true;
                cout << records[idx].timeStr << "\t"
                     << records[idx].model << "\t\t"
                     << records[idx].tailNum << endl;
            }
        }

        if (!found) {
            cout << "Нет посадок на данном аэродроме." << endl;
        }
        cout << endl;
    }

    return 0;
}

// --- РЕАЛИЗАЦИЯ ФУНКЦИЙ ---

bool validateAndParseTime(const string& timeStr, int& timeInMinutes) {
    if (timeStr.length() != 5 || timeStr[2] != ':') return false;

    int h = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    int m = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');

    if (h < 0 || h > 23 || m < 0 || m > 59) return false;

    timeInMinutes = h * 60 + m;
    return true;
}

bool validateAndParseAirport(const string& aptStr, int& aptNum) {
    if (aptStr == "1" || aptStr == "АП1" || aptStr == "AP1") { aptNum = 1; return true; }
    if (aptStr == "2" || aptStr == "АП2" || aptStr == "AP2") { aptNum = 2; return true; }
    if (aptStr == "3" || aptStr == "АП3" || aptStr == "AP3") { aptNum = 3; return true; }
    return false;
}

void indexBubbleSort(const Flight arr[], int indices[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[indices[j]].timeInMinutes < arr[indices[j + 1]].timeInMinutes) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
}
