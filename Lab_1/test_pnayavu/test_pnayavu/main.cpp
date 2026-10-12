#include <iostream>
using namespace std;

enum etype { laborant, sekretar, menedzher, director };

struct date
{
    int day;
    int month;
    int year;
};

bool is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year)
{
    int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month < 1 || month > 12)
        return 0;

    if (month == 2 && is_leap_year(year))
        return 29;

    return days[month - 1];
}

bool is_valid_date(const date &d)
{
    if (d.year < 1900 || d.year > 2100)  
        return false;

    if (d.month < 1 || d.month > 12)
        return false;

    int max_day = days_in_month(d.month, d.year);
    if (d.day < 1 || d.day > max_day)
        return false;

    return true;
}

class employee
{
    int number;         
    float salary;        
    date hire_date;     
    etype status;        

public:
    employee()             
    {
        number = 0;
        salary = 0;
        hire_date.day = hire_date.month = hire_date.year = 0;
        status = laborant;
    }

    employee(const employee &other)    
    {
        number = other.number;
        salary = other.salary;
        hire_date = other.hire_date;
        status = other.status;
    }

    ~employee()              
    {
    }

    int get_number(void) const
    {
        return number;
    }

    const char* status_name(etype st)
    {
        switch (st)
        {
            case laborant:  return "лаборант";
            case sekretar:  return "секретарь";
            case menedzher: return "менеджер";
            case director:  return "директор";
            default:        return "неизвестно";
        }
    }

    void putemploy(void)
    {
        cout << "Номер сотрудника : " << number << endl;
        cout << "Оклад            : " << salary << endl;
        cout << "Дата приема      : "
             << hire_date.day << "."
             << hire_date.month << "."
             << hire_date.year << endl;
        cout << "Статус           : " << status_name(status) << endl;
    }

    void getemploy(employee *staff, int count)
    {
        bool number_ok;
        do
        {
            number_ok = true;
            cout << "Введите номер сотрудника: ";
            cin >> number;

            for (int i = 0; i < count; i++)
            {
                if (staff[i].get_number() == number)
                {
                    cout << "Ошибка: сотрудник с номером " << number
                         << " уже существует. Введите другой номер.\n";
                    number_ok = false;
                    break;
                }
            }
        } while (!number_ok);

        cout << "Введите оклад сотрудника: ";
        cin >> salary;

        bool date_ok;
        do
        {
            cout << "Введите дату приема на работу (день месяц год): ";
            cin >> hire_date.day >> hire_date.month >> hire_date.year;

            date_ok = is_valid_date(hire_date);

            if (!date_ok)
                cout << "Ошибка: такой даты не существует. Попробуйте снова.\n";
        } while (!date_ok);

        int choice;
        cout << "Выберите статус сотрудника:\n"
             << " 0 - лаборант\n"
             << " 1 - секретарь\n"
             << " 2 - менеджер\n"
             << " 3 - директор\n"
             << "Ваш выбор: ";
        cin >> choice;

        if (choice < 0 || choice > 3)
            choice = 0;

        status = static_cast<etype>(choice);
    }
};

int main(void)
{
    int n;
    cout << "Введите количество сотрудников: ";
    cin >> n;

    employee *staff = new employee[n];

    for (int i = 0; i < n; i++)
    {
        cout << "\nСотрудник №" << (i + 1) << endl;
        staff[i].getemploy(staff, i);
    }

    cout << "\n=== Сведения о сотрудниках ===\n";
    for (int i = 0; i < n; i++)
        staff[i].putemploy();

    if (n > 0)
    {
        cout << "\n=== Демонстрация конструктора копирования ===\n";
        cout << "Создание копии первого сотрудника (staff[0])...\n";

        employee copied_emp = staff[0];

        copied_emp.putemploy();
    }

    delete [] staff;

    return 0;
}