#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

class MyException : public std::exception
{
protected:
	int code_;
	std::string message_;
	std::string fullMessage_;

public:
	MyException(int code, const std::string& message)
		: code_(code), message_(message)
	{
		fullMessage_ = "[" + std::to_string(code_) + "] " + message_;
	}

	const char* what() const noexcept override
	{
		return fullMessage_.c_str();
	}

	int code() const noexcept
	{
		return code_;
	}

	const std::string& message() const noexcept
	{
		return message_;
	}
};

class InputExp : public MyException
{
private:
	std::string example_;

public:
	InputExp(int code, const std::string& message, const std::string& example)
		: MyException(code, message), example_(example)
	{
		fullMessage_ = "[" + std::to_string(code_) + "] " + message_
			+ "\nПример: " + example_;
	}

	const std::string& example() const noexcept
	{
		return example_;
	}
};

class FileExp : public MyException
{
private:
	std::string fileName_;

public:
	FileExp(int code, const std::string& message, const std::string& fileName)
		: MyException(code, message), fileName_(fileName)
	{
		fullMessage_ = "[" + std::to_string(code_) + "] " + message_
			+ "\nФайл: " + fileName_;
	}

	const std::string& fileName() const noexcept
	{
		return fileName_;
	}
};

class BoundaryExp : public MyException
{
private:
	int minimum_;
	int maximum_;

public:
	BoundaryExp(int code, const std::string& message, int minimum, int maximum)
		: MyException(code, message), minimum_(minimum), maximum_(maximum)
	{
		fullMessage_ = "[" + std::to_string(code_) + "] " + message_
			+ "\nДопустимый диапазон: [" + std::to_string(minimum_)
			+ "; " + std::to_string(maximum_) + "]";
	}

	int minimum() const noexcept
	{
		return minimum_;
	}

	int maximum() const noexcept
	{
		return maximum_;
	}
};

class ListExp : public MyException
{
public:
	using MyException::MyException;
};

template <typename T>
struct ListNode
{
	T data;
	ListNode* prev;
	ListNode* next;

	explicit ListNode(const T& value)
		: data(value), prev(nullptr), next(nullptr) {}
	explicit ListNode(T&& value)
		: data(std::move(value)), prev(nullptr), next(nullptr) {}
};

template <typename T>
class List
{
private:
	ListNode<T>* head_;
	ListNode<T>* tail_;
	ListNode<T>* current_;
	size_t size_;

	void destroy() noexcept
	{
		ListNode<T>* node = head_;
		while (node != nullptr)
		{
			ListNode<T>* next = node->next;
			delete node;
			node = next;
		}
		head_ = nullptr;
		tail_ = nullptr;
		current_ = nullptr;
		size_ = 0;
	}

public:
	List() : head_(nullptr), tail_(nullptr), current_(nullptr), size_(0) {}
	~List() { destroy(); }

	void push_back(const T& value)
	{
		ListNode<T>* node = new ListNode<T>(value);
		node->prev = tail_;
		if (tail_ != nullptr)
			tail_->next = node;
		else
			head_ = node;
		tail_ = node;
		if (current_ == nullptr)
			current_ = head_;
		++size_;
	}

	void push_front(const T& value)
	{
		ListNode<T>* node = new ListNode<T>(value);
		node->next = head_;
		if (head_ != nullptr)
			head_->prev = node;
		else
			tail_ = node;
		head_ = node;
		if (current_ == nullptr)
			current_ = head_;
		++size_;
	}

	T pop_front()
	{
		if (head_ == nullptr)
			throw ListExp(401, "Список пуст: удаление элемента невозможно");

		ListNode<T>* removed = head_;
		T value = removed->data;
		head_ = removed->next;
		if (head_ != nullptr)
			head_->prev = nullptr;
		else
			tail_ = nullptr;
		if (current_ == removed)
			current_ = head_;
		delete removed;
		--size_;
		return value;
	}

	T pop_back()
	{
		if (tail_ == nullptr)
			throw ListExp(402, "Список пуст: удаление элемента невозможно");

		ListNode<T>* removed = tail_;
		T value = removed->data;
		tail_ = removed->prev;
		if (tail_ != nullptr)
			tail_->next = nullptr;
		else
			head_ = nullptr;
		if (current_ == removed)
			current_ = tail_;
		delete removed;
		--size_;
		return value;
	}

	T& peek()
	{
		if (head_ == nullptr)
			throw ListExp(403, "Список пуст: первый элемент отсутствует");
		return head_->data;
	}

	T& current()
	{
		if (current_ == nullptr)
			throw ListExp(404, "Текущий элемент отсутствует");
		return current_->data;
	}

	List& operator++()
	{
		if (current_ != nullptr)
			current_ = current_->next;
		return *this;
	}

	List& operator--()
	{
		if (current_ != nullptr)
			current_ = current_->prev;
		else if (tail_ != nullptr)
			current_ = tail_;
		return *this;
	}

	void reset_current()
	{
		current_ = head_;
	}

	void clear()
	{
		destroy();
	}

	void print(std::ostream& output = std::cout) const
	{
		output << "[";
		bool first = true;
		for (ListNode<T>* node = head_; node != nullptr; node = node->next)
		{
			if (!first)
				output << ", ";
			output << node->data;
			first = false;
		}
		output << "]";
	}

	void printValues(std::ostream& output = std::cout) const
	{
		for (ListNode<T>* node = head_; node != nullptr; node = node->next)
			output << node->data << '\n';
	}

	bool empty() const noexcept
	{
		return size_ == 0;
	}

	size_t size() const noexcept
	{
		return size_;
	}
};

template <typename T>
std::ostream& operator<<(std::ostream& output, const List<T>& list)
{
	list.print(output);
	return output;
}

int readInteger(const std::string& prompt)
{
	std::cout << prompt;
	std::string line;
	if (!std::getline(std::cin, line))
		throw InputExp(101, "Ввод прерван или достигнут конец потока", "12");

	if (line.empty())
		throw InputExp(102, "Ввод не должен быть пустым", "12");

	size_t processed = 0;
	long number;
	try
	{
		number = std::stol(line, &processed);
	}
	catch (const std::invalid_argument&)
	{
		throw InputExp(103, "Введите корректное целое число", "12");
	}
	catch (const std::out_of_range&)
	{
		throw BoundaryExp(201, "Число выходит за допустимый диапазон",
			std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	}

	if (processed != line.size())
		throw InputExp(104, "Ввод содержит лишние символы", "12");

	if (number < std::numeric_limits<int>::min()
		|| number > std::numeric_limits<int>::max())
		throw BoundaryExp(202, "Число выходит за допустимый диапазон int",
			std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

	return static_cast<int>(number);
}

int readMenuChoice()
{
	int choice = readInteger("Выберите действие: ");
	if (choice < 0 || choice > 12)
		throw BoundaryExp(203, "Выбрано действие вне границ меню", 0, 12);
	return choice;
}

void saveList(const List<int>& list, const std::string& fileName)
{
	std::ofstream output(fileName);
	if (!output.is_open())
		throw FileExp(301, "Не удалось открыть файл для записи", fileName);

	output << list.size() << '\n';
	list.printValues(output);
	if (!output)
		throw FileExp(302, "Ошибка записи в файл", fileName);
}

void loadList(List<int>& list, const std::string& fileName)
{
	std::ifstream input(fileName);
	if (!input.is_open())
		throw FileExp(303, "Не удалось открыть файл для чтения", fileName);

	size_t count = 0;
	input >> count;
	if (!input)
		throw FileExp(304, "Неверный формат файла", fileName);

	list.clear();
	for (size_t i = 0; i < count; ++i)
	{
		int value = 0;
		input >> value;
		if (!input)
			throw FileExp(305, "В файле недостаточно целых чисел", fileName);
		list.push_back(value);
	}
}

int main()
{
	List<int> list;

	while (true)
	{
		std::cout << "\nМеню списка\n";
		std::cout << "1. Добавить в начало\n";
		std::cout << "2. Добавить в конец\n";
		std::cout << "3. Удалить с начала\n";
		std::cout << "4. Удалить с конца\n";
		std::cout << "5. Показать первый элемент\n";
		std::cout << "6. Показать текущий элемент\n";
		std::cout << "7. Перейти к следующему элементу\n";
		std::cout << "8. Перейти к предыдущему элементу\n";
		std::cout << "9. Сбросить указатель на начало\n";
		std::cout << "10. Вывести весь список\n";
		std::cout << "11. Сохранить список в файл\n";
		std::cout << "12. Загрузить список из файла\n";
		std::cout << "0. Выход\n";

		try
		{
			int command = readMenuChoice();
			if (command == 0)
			{
				std::cout << "Программа завершена.\n";
				break;
			}

			switch (command)
			{
			case 1:
				list.push_front(readInteger("Введите элемент: "));
				break;
			case 2:
				list.push_back(readInteger("Введите элемент: "));
				break;
			case 3:
				std::cout << "Удалено с начала: " << list.pop_front() << '\n';
				break;
			case 4:
				std::cout << "Удалено с конца: " << list.pop_back() << '\n';
				break;
			case 5:
				std::cout << "Первый элемент: " << list.peek() << '\n';
				break;
			case 6:
				std::cout << "Текущий элемент: " << list.current() << '\n';
				break;
			case 7:
				++list;
				std::cout << "Переход выполнен\n";
				break;
			case 8:
				--list;
				std::cout << "Переход выполнен\n";
				break;
			case 9:
				list.reset_current();
				std::cout << "Указатель установлен на начало\n";
				break;
			case 10:
				std::cout << "Список: " << list << '\n';
				std::cout << "Размер: " << list.size() << '\n';
				break;
			case 11:
				saveList(list, "lab5_list.txt");
				std::cout << "Список сохранён в lab5_list.txt\n";
				break;
			case 12:
				loadList(list, "lab5_list.txt");
				std::cout << "Список загружен из lab5_list.txt\n";
				break;
			default:
				break;
			}
		}
		catch (const InputExp& error)
		{
			std::cout << "Ошибка ввода: " << error.what() << '\n';
		}
		catch (const BoundaryExp& error)
		{
			std::cout << "Ошибка границы: " << error.what() << '\n';
		}
		catch (const FileExp& error)
		{
			std::cout << "Ошибка файла: " << error.what() << '\n';
		}
		catch (const ListExp& error)
		{
			std::cout << "Ошибка списка: " << error.what() << '\n';
		}
		catch (const MyException& error)
		{
			std::cout << "Базовая ошибка: " << error.what() << '\n';
		}
		catch (const std::exception& error)
		{
			std::cout << "Стандартная ошибка: " << error.what() << '\n';
		}
	}

	return 0;
}
