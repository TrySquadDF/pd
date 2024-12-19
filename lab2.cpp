#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <iomanip>
#include <cmath>

#include <io.h>
#include <fcntl.h>
#include <windows.h>

// Unicode box drawing characters
const wchar_t VERTICAL = L'│';
const wchar_t HORIZONTAL = L'─';
const wchar_t TOP_LEFT = L'┌';
const wchar_t TOP_RIGHT = L'┐';
const wchar_t BOTTOM_LEFT = L'└';
const wchar_t BOTTOM_RIGHT = L'┘';
const wchar_t T_DOWN = L'┬';
const wchar_t T_UP = L'┴';
const wchar_t T_RIGHT = L'├';
const wchar_t T_LEFT = L'┤';
const wchar_t CROSS = L'┼';

void printHorizontalLine(const std::vector<size_t> &colWidths, wchar_t left, wchar_t mid, wchar_t right)
{
    std::wcout << left;
    for (size_t i = 0; i < colWidths.size(); ++i)
    {
        std::wcout << std::wstring(colWidths[i] + 2, HORIZONTAL);
        if (i < colWidths.size() - 1)
            std::wcout << mid;
    }
    std::wcout << right << '\n';
}

void printTable(const std::vector<std::vector<std::string>> &table)
{
    // Calculate column widths
    std::vector<size_t> colWidths(table[0].size(), 0);
    for (const auto &row : table)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            colWidths[i] = std::max(colWidths[i], row[i].length());
        }
    }

    // Set output to use wide characters
    std::wcout.imbue(std::locale(""));

    // Print top border
    printHorizontalLine(colWidths, TOP_LEFT, T_DOWN, TOP_RIGHT);

    // Print rows
    for (size_t i = 0; i < table.size(); ++i)
    {
        std::wcout << VERTICAL;
        for (size_t j = 0; j < table[i].size(); ++j)
        {
            std::wstring wstr(table[i][j].begin(), table[i][j].end());
            std::wcout << ' ' << std::left << std::setw(colWidths[j]) << wstr << ' ';
            std::wcout << VERTICAL;
        }
        std::wcout << '\n';

        // Print horizontal line after header or between rows
        if (i < table.size() - 1)
        {
            printHorizontalLine(colWidths, T_RIGHT, CROSS, T_LEFT);
        }
    }

    // Print bottom border
    printHorizontalLine(colWidths, BOTTOM_LEFT, T_UP, BOTTOM_RIGHT);
}

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

std::vector<std::vector<std::string>> applyFunctions(const std::vector<std::vector<double>> &snapshot)
{
    std::vector<std::vector<std::string>> result;
    for (const auto &screen : snapshot)
    {
        std::vector<std::string> row = {
            std::to_string(screen[1]),
            std::to_string(f(screen[1])),
            std::to_string(df(screen[1])),
            std::to_string(fabs(screen[1] - screen[0]))
        };
        result.push_back(row);
    }
    return result;
}

std::vector<std::vector<std::string>> applyFunctions2(const std::vector<std::vector<double>> &snapshot)
{
    std::vector<std::vector<std::string>> result;
    for (const auto &screen : snapshot)
    {
        std::vector<std::string> row = {
            std::to_string(screen[1]),
            std::to_string(f(screen[1])),
            std::to_string(1 / (1 + f(screen[1]))),
            std::to_string(fabs(screen[1] - screen[0]))
        };
        result.push_back(row);
    }
    return result;
}

std::pair<double, std::vector<std::vector<double>>> chord_method(double a, double b, const double EPS)
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

std::pair<double, std::vector<std::vector<double>>> newton_method(double x0, const double EPS)
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

int main()
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::vector<std::vector<std::string>> table = {
        {"x", "f(x)", "1/((1+fx1))", "|x1-x|"}};
    std::vector<std::vector<std::string>> table2 = {
        {"x", "f(x)", "f'(x)", "|x1-x|"}};

    double a = 0;
    double b = 1.5;
    double x0;
    const double EPS = 0.001;
    const auto tableStartIndex = table.end();

    // For chord method
    auto [chord_root, chord_steps] = chord_method(a, b, EPS);
    auto chord_values = applyFunctions(chord_steps);

    table.insert(
        tableStartIndex,
        std::make_move_iterator(chord_values.begin()),
        std::make_move_iterator(chord_values.end())
    );

    printTable(table);

    // For Newton's method
    if (f(a) * df2(a) >= 0)
        x0 = a;
    else if (f(b) * df2(b) >= 0)
        x0 = b;
    else
    {
        std::cout << "Ошибка. Хотя бы один из концов должен быть точкой перегиба!";
        return 0;
    }

    auto [newton_root, newton_steps] = newton_method(x0, EPS);
    auto newton_values = applyFunctions(newton_steps);

    table2.insert(
        table2.end(),
        std::make_move_iterator(newton_values.begin()),
        std::make_move_iterator(newton_values.end())
    );

    printTable(table2);

    return 0;
}
