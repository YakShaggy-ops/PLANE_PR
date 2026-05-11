#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;



// Максимальное количество записей
const int MAX_RECORDS = 100;

// Структура для хранения данных о самолете
struct Flight {
    int timeInMinutes; // Время в минутах от начала суток (для удобной сортировки)
    string timeStr;    // Исходная строка времени (ЧЧ:ММ)
    string model;      // Марка ЛА
    string tailNum;    // Бортовой номер
    int airportNum;    // Номер аэродрома (1, 2 или 3)
};

// Функция входного контроля и парсинга времени
bool validateAndParseTime(const string& timeStr, int& timeInMinutes) {
    if (timeStr.length() != 5 || timeStr[2] != ':') return false;

    int h = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    int m = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');

    if (h < 0 || h > 23 || m < 0 || m > 59) return false;

    timeInMinutes = h * 60 + m;
    return true;
}

// Функция входного контроля и парсинга аэродрома
bool validateAndParseAirport(const string& aptStr, int& aptNum) {
    if (aptStr == "1" || aptStr == "АП1" || aptStr == "AP1") { aptNum = 1; return true; }
    if (aptStr == "2" || aptStr == "АП2" || aptStr == "AP2") { aptNum = 2; return true; }
    if (aptStr == "3" || aptStr == "АП3" || aptStr == "AP3") { aptNum = 3; return true; }
    return false;
}

// Функция индексной сортировки методом пузырька (по убыванию времени)
// Обмен данными происходит только через параметры
void indexBubbleSort(const Flight arr[], int indices[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // Если время у текущего элемента меньше, чем у следующего - меняем ИНДЕКСЫ местами (сортировка по убыванию)
            if (arr[indices[j]].timeInMinutes < arr[indices[j + 1]].timeInMinutes) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    //setlocale(LC_ALL, "Russian"); // Для корректного вывода кириллицы в консоль

    Flight records[MAX_RECORDS];
    int indices[MAX_RECORDS];
    int count = 0;

    // Открываем файл с исходными данными
    ifstream file("data.txt");
    if (!file.is_open()) {
        cout << "Ошибка: не удалось открыть файл data.txt" << endl;
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
        if (tail.length() < 3) { // Базовая проверка бортового номера на длину
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

    cout << "\n--- Результаты сортировки ---\n\n";

    // Вызов функции сортировки (сортируется массив индексов, а не самих данных)
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
