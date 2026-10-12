#include <iostream>
#include <cstring>
using namespace std;

class MyString
{
    char *data;     
    size_t length;    

public:
    MyString()
    {
        length = 0;
        data = new char[1];
        data[0] = '\0';
    }

    MyString(const char *s)
    {
        if (s == nullptr)
        {
            length = 0;
            data = new char[1];
            data[0] = '\0';
        }
        else
        {
            length = strlen(s);
            data = new char[length + 1];
            strcpy(data, s);
        }
    }

    MyString(const MyString &other)
    {
        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);
    }

    MyString& operator=(const MyString &other)
    {
        if (this == &other)      
            return *this;

        delete [] data;           

        length = other.length;
        data = new char[length + 1];
        strcpy(data, other.data);

        return *this;
    }

    ~MyString()
    {
        delete [] data;
    }

    size_t size(void) const
    {
        return length;
    }

    const char* c_str(void) const
    {
        return data;
    }

    bool operator==(const MyString &other) const
    {
        return strcmp(data, other.data) == 0;
    }

    bool operator<(const MyString &other) const
    {
        return strcmp(data, other.data) < 0;
    }

    bool operator>(const MyString &other) const
    {
        return strcmp(data, other.data) > 0;
    }

    bool operator!=(const MyString &other) const
    {
        return !(*this == other);
    }

    bool operator<=(const MyString &other) const
    {
        return strcmp(data, other.data) <= 0;
    }

    bool operator>=(const MyString &other) const
    {
        return strcmp(data, other.data) >= 0;
    }

    friend ostream& operator<<(ostream &os, const MyString &s);
    friend istream& operator>>(istream &is, MyString &s);
};

ostream& operator<<(ostream &os, const MyString &s)
{
    os << s.data;
    return os;
}
// не забыть показать про временное выделение 
istream& operator>>(istream &is, MyString &s)
{
    string temp;
    is >> temp;

    if (is.fail())
        return is;

    char *new_data = new char[temp.size() + 1];
    strcpy(new_data, temp.c_str());

    delete [] s.data;
    s.length = temp.size();
    s.data = new_data;

    return is;
}

int main(void)
{
    cout << " Демонстрация класса MyString \n\n";

    MyString s1("Apple");
    MyString s2("Banana");

    cout << "s1 = " << s1 << endl;
    cout << "s2 = " << s2 << endl;

    cout << "\n Конструктор копирования \n";
    MyString s3 = s1;             
    cout << "s3 (копия s1) = " << s3 << endl;

    
    cout << "\n Операторы сравнения\n";
    cout << "s1 < s2  : " << (s1 < s2 ? "true" : "false") << endl;
    cout << "s1 > s2  : " << (s1 > s2 ? "true" : "false") << endl;
    cout << "s1 != s2 : " << (s1 != s2 ? "true" : "false") << endl;
    cout << "s1 <= s3 : " << (s1 <= s3 ? "true" : "false") << endl;
    cout << "s1 >= s3 : " << (s1 >= s3 ? "true" : "false") << endl;

    cout << "\n Ввод строки через operator>>\n";
    MyString s4;
    cout << "Введите строку (без пробелов): ";
    cin >> s4;
    cout << "Вы ввели: " << s4 << endl;
    cout << "Длина введенной строки: " << s4.size() << endl;

    cout << "\nВведенная строка < \"Banana\" ? "
         << (s4 < s2 ? "true" : "false") << endl;

    return 0;
}