#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <iomanip>
#include <cmath>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

/** -------------------------------------------------------------------------------------
* Функции и структуры для отрисовки таблицы.
*
* Автор Блока: Тихонов Павел Сергеевич.
* Описание: Отрисовка таблицы с использованием юникодных
* символов заимствованных из Unicode box drawing characters.
* Дата создания: 2024-12-23.
-------------------------------------------------------------------------------------- **/
struct TableChars
{
    static const wchar_t VERTICAL = L'│';
    static const wchar_t HORIZONTAL = L'─';
    static const wchar_t TOP_LEFT = L'┌';
    static const wchar_t TOP_RIGHT = L'┐';
    static const wchar_t BOTTOM_LEFT = L'└';
    static const wchar_t BOTTOM_RIGHT = L'┘';
    static const wchar_t T_DOWN = L'┬';
    static const wchar_t T_UP = L'┴';
    static const wchar_t T_RIGHT = L'├';
    static const wchar_t T_LEFT = L'┤';
    static const wchar_t CROSS = L'┼';
};

/**
 * Вычисляет максимальную ширину для каждого столбца таблицы
 * @param table - входная таблица в виде векторов строк
 * @return вектор с максимальными ширинами столбцов
 */
std::vector<size_t> calculateColumnWidths(const std::vector<std::vector<std::string>> &table)
{
    // Инициализируем вектор шириной столбцов нулями
    std::vector<size_t> colWidths(table[0].size(), 0);

    // Проходим по каждой строке таблицы
    for (const auto &row : table)
    {
        // Для каждого элемента в строке
        for (size_t i = 0; i < row.size(); ++i)
        {
            // Обновляем максимальную ширину если текущий элемент длиннее
            colWidths[i] = std::max(colWidths[i], row[i].length());
        }
    }
    return colWidths;
}

/**
 * Печатает горизонтальную линию с заданными граничными символами
 * @param colWidths - ширины столбцов
 * @param left - левый символ
 * @param mid - средний символ
 * @param right - правый символ
 */
void printHorizontalLine(
    const std::vector<size_t> &colWidths,
    wchar_t left,
    wchar_t mid,
    wchar_t right)
{
    // Печатаем левую границу
    std::wcout << left;

    // Для каждого столбца
    for (size_t i = 0; i < colWidths.size(); ++i)
    {
        // Печатаем горизонтальную линию нужной длины (+2 для отступов)
        std::wcout << std::wstring(colWidths[i] + 2, TableChars::HORIZONTAL);

        // Печатаем разделитель между столбцами, кроме последнего
        if (i < colWidths.size() - 1)
            std::wcout << mid;
    }
    // Печатаем правую границу и переход на новую строку
    std::wcout << right << '\n';
}

/**
 * Печатает одну строку данных с форматированием
 * @param row - строка данных
 * @param colWidths - ширины столбцов
 */
void printTableRow(
    const std::vector<std::string> &row,
    const std::vector<size_t> &colWidths)
{
    // Печатаем левую границу
    std::wcout << TableChars::VERTICAL;

    // Для каждого элемента в строке
    for (size_t i = 0; i < row.size(); ++i)
    {
        // Печатаем пробел, выравненный по левому краю текст и пробел
        std::wcout << L' ' << std::left << std::setw(colWidths[i])
                   << std::wstring(row[i].begin(), row[i].end()) << L' ';
        // Печатаем вертикальную границу
        std::wcout << TableChars::VERTICAL;
    }
    // Переход на новую строку
    std::wcout << '\n';
}

/**
 * Печатает всю таблицу целиком
 * @param table - таблица данных
 */
void printTable(const std::vector<std::vector<std::string>> &table)
{
    // Устанавливаем локаль для корректного отображения Unicode
    std::wcout.imbue(std::locale(""));

    // Получаем ширины столбцов
    auto colWidths = calculateColumnWidths(table);

    // Печатаем верхнюю границу
    printHorizontalLine(colWidths, TableChars::TOP_LEFT, TableChars::T_DOWN, TableChars::TOP_RIGHT);

    // Печатаем все строки таблицы
    for (size_t i = 0; i < table.size(); ++i)
    {
        printTableRow(table[i], colWidths);
        // Печатаем разделители между строками, кроме последней
        if (i < table.size() - 1)
        {
            printHorizontalLine(colWidths, TableChars::T_RIGHT, TableChars::CROSS, TableChars::T_LEFT);
        }
    }

    // Печатаем нижнюю границу
    printHorizontalLine(colWidths, TableChars::BOTTOM_LEFT, TableChars::T_UP, TableChars::BOTTOM_RIGHT);
}

/** -------------------------------------------------------------------------------------
* Условие задачи |  Task condition
-------------------------------------------------------------------------------------- **/

double f(double x)
{
    return sqrt(x) - cos(0.387 * x);
}

double df(double x)
{
    return 0.5 / sqrt(x) + 0.387 * sin(0.387 * x);
}

double df2(double x)
{
    return -0.25 / (x * sqrt(x)) + 0.149769 * cos(0.387 * x);
}

/** -------------------------------------------------------------------------------------
* Методы решения.
*
* Автор: Нарзиев Артемий Тимурович, Шалаев Александр Максимович,
* Журавлев Дмитрий Сергеевич, Тихонов Павел Сергеевич.
* Описание: Реализация методов решения уравнения.
* Дата создания: 2024-12-23.
-------------------------------------------------------------------------------------- **/

/** ---------------------------
 * Базовая релизация решения методом хорд.
 *
 * @param a - левая граница отрезка
 * @param b - правая граница отрезка
 * @param EPS - точность вычислений
 * @return корень уравнения и история итераций
 *
 * Автор: Шалаев Александр Максимович.
--------------------------------*/
std::pair<double, std::vector<std::vector<double>>> chord_method_while(double a, double b, const double EPS)
{
    double x0 = a;
    double x1 = b;
    double x = 0;

    std::vector<std::vector<double>> snapshot;

    while (fabs(x1 - x) >= EPS)
    {
        snapshot.push_back({x, x1});

        x = x1;
        x1 = x0 - (f(x0) / (f(x1) - f(x0))) * (x1 - x0);
    }
    return std::make_pair(x, snapshot);
}

/** ---------------------------
 * Базовая релизация решения методом Ньютона.
 *
 * @param x0 - начальное приближение
 * @param EPS - точность вычислений
 * @return корень уравнения и история итераций
 *
 * Автор: Журавлев Дмитрий Сергеевич.
--------------------------------*/
std::pair<double, std::vector<std::vector<double>>> newton_method_while(double x0, const double EPS)
{
    if (x0 == 0)
        x0 = 0.01;
    double x = 0;
    double x1 = x0;

    std::vector<std::vector<double>> snapshot;

    while (fabs(x1 - x) >= EPS)
    {
        snapshot.push_back({x, x1});

        x = x1;
        x1 = x - f(x) / df(x);
    }
    return std::make_pair(x1, snapshot);
}

/** ---------------------------
 * Реализация метода хорд с использованием цикла for.
 *
 * @param a - левая граница отрезка
 * @param b - правая граница отрезка
 * @param EPS - точность вычислений
 * @return корень уравнения и история итераций
 *
 * Автор: Нарзиев Артемий Тимурович.
--------------------------------*/
std::pair<double, std::vector<std::vector<double>>> chord_method_for(double a, double b, const double EPS)
{
    double x0 = a;
    double x1 = b;
    double x = 0;

    std::vector<std::vector<double>> snapshot;

    for (; fabs(x1 - x) >= EPS;)
    {
        snapshot.push_back({x, x1});
        x = x1;
        x1 = x0 - (f(x0) / (f(x1) - f(x0))) * (x1 - x0);
    }
    return std::make_pair(x, snapshot);
}

/** ---------------------------
 * Реализация метода хорд с использованием цикла for.
 *
 * @param x0 - начальное приближение
 * @param EPS - точность вычислений
 * @return корень уравнения и история итераций
 *
 * Автор: Нарзиев Артемий Тимурович.
--------------------------------*/
std::pair<double, std::vector<std::vector<double>>> newton_method_for(double x0, const double EPS)
{
    if (x0 == 0)
        x0 = 0.01;
    double x = 0;
    double x1 = x0;

    std::vector<std::vector<double>> snapshot;

    for (; fabs(x1 - x) >= EPS;)
    {
        snapshot.push_back({x, x1});
        x = x1;
        x1 = x - f(x) / df(x);
    }
    return std::make_pair(x1, snapshot);
}

/** ---------------------------
 * Применение функций к итерациям методов
 *
 * @param snapshot - история итераций метода
 * @param useNewton - флаг использования метода Ньютона
 * @return таблица строк с результатами вычислений:
 *         - x: текущее приближение
 *         - f(x): значение функции
 *         - df(x) или 1/(1+f(x)): производная или коэффициент
 *         - |x1-x|: погрешность
 *
 * Автор: Тихонов Павел Сергеевич.
--------------------------------*/
std::vector<std::vector<std::string>> applyFunctions(const std::vector<std::vector<double>> &snapshot, bool useNewton = false)
{
    std::vector<std::vector<std::string>> result;
    for (const auto &screen : snapshot)
    {
        double x = screen[1];
        double fx = f(x);
        std::vector<std::string> row = {
            std::to_string(x),
            std::to_string(fx),
            std::to_string(useNewton ? df(x) : 1 / (1 + fx)),
            std::to_string(fabs(x - screen[0]))};
        result.push_back(row);
    }
    return result;
}

/** ---------------------------
 * Точка входа в программу.
 *
 * Автор: Журавлев Дмитрий Сергеевич, Нарзиев Артемий Тимурович,
 * Шалаев Александр Максимович, Тихонов Павел Сергеевич.
--------------------------------*/
int main()
{

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Обьявление заголовков таблиц.
    std::vector<std::vector<std::string>> table = {
        {"x", "f(x)", "1/((1+fx1))", "|x1-x|"}};
    std::vector<std::vector<std::string>> table1 = {
        {"x", "f(x)", "1/((1+fx1))", "|x1-x|"}};

    std::vector<std::vector<std::string>> table2 = {
        {"x", "f(x)", "f'(x)", "|x1-x|"}};
    std::vector<std::vector<std::string>> table3 = {
        {"x", "f(x)", "f'(x)", "|x1-x|"}};

    double a = 0;
    double b = 1.5;
    double x0;
    const double EPS = 0.001;

    // Вызов метода хорд.
    auto [chord_root, chord_steps] = chord_method_while(a, b, EPS);
    auto [chord_root1, chord_steps1] = chord_method_for(a, b, EPS);
    auto chord_values = applyFunctions(chord_steps);
    auto chord_values1 = applyFunctions(chord_steps1);

    if (f(a) * df2(a) >= 0)
        x0 = a;
    else if (f(b) * df2(b) >= 0)
        x0 = b;
    else
    {
        std::cout << "Ошибка. Хотя бы один из концов должен быть точкой перегиба!";
        return 0;
    }

    // Вызов метода Ньютона.
    auto [newton_root, newton_steps] = newton_method_while(x0, EPS);
    auto [newton_root1, newton_steps1] = newton_method_for(x0, EPS);
    auto newton_values = applyFunctions(newton_steps, true);
    auto newton_values1 = applyFunctions(newton_steps1, true);

    // Добавление результатов в таблицы.
    table.insert(
        table.end(),
        std::make_move_iterator(chord_values.begin()),
        std::make_move_iterator(chord_values.end()));
    table1.insert(
        table1.end(),
        std::make_move_iterator(chord_values1.begin()),
        std::make_move_iterator(chord_values1.end()));

    table2.insert(
        table2.end(),
        std::make_move_iterator(newton_values.begin()),
        std::make_move_iterator(newton_values.end()));

    table3.insert(
        table3.end(),
        std::make_move_iterator(newton_values1.begin()),
        std::make_move_iterator(newton_values1.end()));

    // Вывод таблиц в консоль.
    printTable(table);
    printTable(table1);

    printTable(table2);
    printTable(table3);

    return 0;
}
