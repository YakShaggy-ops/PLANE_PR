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
#include <windows.h>

using namespace std;

// --- ГЛОБАЛЬНЫЕ КОНСТАНТЫ ---
const int MAX_RECORDS = 100;
const int MAX_LINE_LEN = 256;
const char* TEST_FILENAME = "data1.txt"; // Имя файла вынесено в константу

// --- СТРУКТУРЫ ---
struct Flight {
    int timeInMinutes;   // Время в минутах от начала суток
    char timeStr[6];     // Исходная строка времени (ЧЧ:ММ)
    char model[20];      // Марка ЛА
    char tailNum[10];    // Бортовой номер
    int airportNum;      // Номер аэродрома (1, 2 или 3)
};

// --- ПРОТОТИПЫ ФУНКЦИЙ ---

// Назначение: Вычисляет длину C-строки.
// Параметры: str (входной) - указатель на строку.
int customStrLen(const char* str);

// Назначение: Подсчет видимой длины строки в UTF-8 (русская буква = 1 символ)
int getVisibleLen(const char* str);

// Назначение: Сравнивает две C-строки на идентичность.
// Параметры: s1, s2 (входные) - указатели на строки. Возвращает true, если равны.
bool customStrCmp(const char* s1, const char* s2);

// Назначение: Копирует содержимое из одной C-строки в другую.
// Параметры: dest (выходной) - куда копировать, src (входной) - откуда копировать.
void customStrCpy(char* dest, const char* src);

// Назначение: Строгая проверка формата времени (XX:YY) и перевод в минуты.
// Параметры: timeStr (входной) - строка времени, timeInMinutes (выходной) - время в минутах.
bool validateAndParseTime(const char* timeStr, int& timeInMinutes);

// Назначение: Проверка бортового номера (строго Б-XXXX).
// Параметры: tailStr (входной) - строка номера.
bool validateTailNumber(const char* tailStr);

// Назначение: Идентификация номера аэродрома.
// Параметры: aptStr (входной) - строка аэродрома, aptNum (выходной) - числовой код (1-3).
bool validateAndParseAirport(const char* aptStr, int& aptNum);

// Назначение: Проверка на смысловые дубликаты (один борт - разные марки, или посадка в одно время).
// Параметры: records (входной) - массив данных, count (входной) - текущее число записей, newFlight (входной) - новая запись.
bool checkLogicalErrors(const Flight* records, int count, const Flight& newFlight);

// Назначение: Индексная сортировка массива структур методом пузырька (по убыванию времени).
// Параметры: arr (входной) - массив структур, indices (вх/вых) - массив индексов, n (входной) - размер.
void indexBubbleSort(const Flight arr[], int indices[], int n);


// --- ГЛАВНАЯ ФУНКЦИЯ ---
int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    // Динамическое выделение памяти
    Flight* records = new Flight[MAX_RECORDS];
    int* indices = new int[MAX_RECORDS];
    int count = 0;
    int lineNum = 0; // Счетчик строк для вывода ошибок

    ifstream file(TEST_FILENAME);
    if (!file.is_open()) {
        cout << "Ошибка: не удалось открыть файл " << TEST_FILENAME << endl;
        delete[] records;
        delete[] indices;
        return 1;
    }

    char line[MAX_LINE_LEN];

    // Читаем файл построчно для полного контроля над мусором в строках
    while (file.getline(line, MAX_LINE_LEN)) {
        lineNum++;
        if (count >= MAX_RECORDS) {
            cout << "Предупреждение: достигнут лимит записей (" << MAX_RECORDS << ")." << endl;
            break;
        }

        // Удаляем возможный символ \r на конце (для файлов, созданных в Windows/Linux)
        int len = customStrLen(line);
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
        }

        // Пропускаем пустые строки
        if (customStrLen(line) == 0) continue;

        char tStr[20] = {0}, mod[20] = {0}, tail[20] = {0}, aptStr[20] = {0}, extra[20] = {0};

        // Читаем токены из строки (ожидаем ровно 4)
        // Используем sscanf для безопасного разбиения без strtok
        int tokensRead = sscanf(line, "%19s %19s %19s %19s %19s", tStr, mod, tail, aptStr, extra);

        if (tokensRead < 4) {
            cout << "[Строка " << lineNum << " пропущена] Недостаточно данных." << endl;
            continue;
        }
        if (tokensRead > 4) {
            cout << "[Строка " << lineNum << " пропущена] Лишние данные в конце строки." << endl;
            continue;
        }

        int timeMins = 0;
        int aptNum = 0;

        // Входной контроль
        if (!validateAndParseTime(tStr, timeMins)) {
            cout << "[Строка " << lineNum << " пропущена] Ошибка формата времени: " << tStr << endl;
            continue;
        }
        if (!validateTailNumber(tail)) {
            cout << "[Строка " << lineNum << " пропущена] Некорректный бортовой номер: " << tail << " (Ожидается Б-XXXX)" << endl;
            continue;
        }
        if (!validateAndParseAirport(aptStr, aptNum)) {
            cout << "[Строка " << lineNum << " пропущена] Неверный аэродром: " << aptStr << endl;
            continue;
        }

        // Формируем структуру для проверки логики
        Flight tempFlight;
        tempFlight.timeInMinutes = timeMins;
        customStrCpy(tempFlight.timeStr, tStr);
        customStrCpy(tempFlight.model, mod);
        customStrCpy(tempFlight.tailNum, tail);
        tempFlight.airportNum = aptNum;

        // Логический контроль
        if (!checkLogicalErrors(records, count, tempFlight)) {
            cout << "[Строка " << lineNum << " пропущена] Логическая ошибка (дубликат или конфликт)." << endl;
            continue;
        }

        // Сохраняем в массив
        records[count] = tempFlight;
        indices[count] = count;
        count++;
    }
    file.close();

    cout << "\n=== РЕЗУЛЬТАТЫ ОБРАБОТКИ (Файл: " << TEST_FILENAME << ") ===\n\n";

    indexBubbleSort(records, indices, count);

    // Вывод красивой таблицы по каждому аэродрому
    for (int currentAirport = 1; currentAirport <= 3; currentAirport++) {
        cout << " АЭРОДРОМ № " << currentAirport << " " << endl;
        cout << "┌───────┬────────────────────┬────────────┐" << endl;
        cout << "│ Время │ Марка ЛА           │ Борт.номер │" << endl;
        cout << "├───────┼────────────────────┼────────────┤" << endl;

        bool found = false;
        for (int i = 0; i < count; i++) {
            int idx = indices[i];
            if (records[idx].airportNum == currentAirport) {
                found = true;

                // Печатаем начало: время и марку ЛА
                cout << "│ " << records[idx].timeStr << " │ " << records[idx].model;

                // Добиваем пробелами до ширины колонки (18)
                int modelSpaces = 18 - getVisibleLen(records[idx].model);
                for (int s = 0; s < modelSpaces; s++) cout << " ";

                // Печатаем бортовой номер
                cout << " │ " << records[idx].tailNum;

                // Добиваем пробелами до ширины колонки (10)
                int tailSpaces = 10 - getVisibleLen(records[idx].tailNum);
                for (int s = 0; s < tailSpaces; s++) cout << " ";

                // Закрываем строку таблицы
                cout << " │" << endl;
            }
        }

        if (!found) {
            cout << "│ Нет посадок на данном аэродроме         │" << endl;
        }
        cout << "└───────┴────────────────────┴────────────┘\n" << endl;
    }

    // Очистка динамической памяти (устранение утечек)
    delete[] records;
    delete[] indices;

    return 0;
}

// --- РЕАЛИЗАЦИЯ ФУНКЦИЙ ---

int customStrLen(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

bool customStrCmp(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return false;
        i++;
    }
    return s1[i] == s2[i];
}

void customStrCpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

bool validateAndParseTime(const char* timeStr, int& timeInMinutes) {
    if (customStrLen(timeStr) != 5) return false;
    if (timeStr[2] != ':') return false;

    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (timeStr[i] < '0' || timeStr[i] > '9') return false;
    }

    int h = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    int m = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');

    if (h < 0 || h > 23 || m < 0 || m > 59) return false;

    timeInMinutes = h * 60 + m;
    return true;
}

bool validateTailNumber(const char* tailStr) {
    int len = customStrLen(tailStr);

    // В UTF-8 русская 'Б' занимает 2 байта (итого 7). В старых кодировках - 1 байт (итого 6).
    // Если длина не 6 и не 7, то это точно мусор.
    if (len != 6 && len != 7) return false;

    // Проверяем, что 5-й символ с конца — это тире '-'
    if (tailStr[len - 5] != '-') return false;

    // Проверяем, что последние 4 символа — это строго цифры
    for (int i = len - 4; i < len; i++) {
        if (tailStr[i] < '0' || tailStr[i] > '9') return false;
    }

    return true;
}

bool validateAndParseAirport(const char* aptStr, int& aptNum) {
    if (customStrCmp(aptStr, "1") || customStrCmp(aptStr, "АП1") || customStrCmp(aptStr, "AP1")) { aptNum = 1; return true; }
    if (customStrCmp(aptStr, "2") || customStrCmp(aptStr, "АП2") || customStrCmp(aptStr, "AP2")) { aptNum = 2; return true; }
    if (customStrCmp(aptStr, "3") || customStrCmp(aptStr, "АП3") || customStrCmp(aptStr, "AP3")) { aptNum = 3; return true; }
    return false;
}

bool checkLogicalErrors(const Flight* records, int count, const Flight& newFlight) {
    for (int i = 0; i < count; i++) {
        if (customStrCmp(records[i].tailNum, newFlight.tailNum)) {
            // Ошибка: один борт, но разные марки самолета
            if (!customStrCmp(records[i].model, newFlight.model)) return false;
            // Ошибка: один борт садится дважды в одно и то же время
            if (records[i].timeInMinutes == newFlight.timeInMinutes) return false;
        }
    }
    return true;
}

void indexBubbleSort(const Flight arr[], int indices[], int n) {
    for (int i = 0; i < n - 1; i++) {
        bool hasSwap = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[indices[j]].timeInMinutes < arr[indices[j + 1]].timeInMinutes) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
                hasSwap = true;
            }
        }
        if (!hasSwap) break; // Оптимизация пузырька
    }
}

int getVisibleLen(const char* str) {
    int len = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        // В UTF-8 каждый дополнительный байт русской буквы начинается с битов 10xxxxxx (0x80)
        // Мы считаем только стартовые байты, пропуская дополнительные.
        if ((str[i] & 0xC0) != 0x80) {
            len++;
        }
    }
    return len;
}
