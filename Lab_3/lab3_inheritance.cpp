
// Что демонстрируется:
//   1. Простое наследование            Entity -> NamedEntity -> Person
//   2. Множественное наследование      Team : NamedEntity + Serializable
//   3. Виртуальное наследование (ромб) PlayerCoach : Player + Coach (оба virtual Person)
//   4. Виртуальные функции             role(), show() и виртуальные деструкторы
//   5. Модификаторы доступа            Referee : private Person
//
// Сборка (macOS / Linux):  g++ -std=c++17 -Wall -Wextra lab3_inheritance.cpp -o lab3
#include <iostream>
#include <cstring>

using namespace std;

#ifndef _MSC_VER
inline int strcpy_s(char* dest, size_t size, const char* src)
{
	if (dest == nullptr || src == nullptr || size == 0)
		return 22;                       // неверные аргументы
	if (strlen(src) >= size) {           // строка не помещается в буфер
		dest[0] = '\0';
		return 34;
	}
	memcpy(dest, src, strlen(src) + 1);
	return 0;
}
#endif

// Размеры буферов. В UTF-8 одна русская буква занимает 2 байта,
// поэтому массивы взяты с запасом.
const int NAME_LEN = 50;
const int TEXT_LEN = 30;

// Интерфейс для сохранения в файл (понадобится в курсовом проекте)

class Serializable
{
public:
	Serializable() { cout << "  [ctor] Serializable" << endl; }
	virtual ~Serializable() { cout << "  [dtor] Serializable" << endl; }
	virtual void serialize(ostream& os) const = 0;   // чисто виртуальная функция
};

// Уровень 1. Базовая сущность: есть у любого объекта чемпионата
class Entity
{
protected:
	int id_;                 // protected: доступно в производных классах, снаружи - нет
public:
	Entity(int id);
	virtual ~Entity();       // виртуальный деструктор (см. вывод в разделе 4)
	int getId() const { return id_; }
	virtual const char* role() const;
	virtual void show() const;
};

Entity::Entity(int id)
{
	cout << "  [ctor] Entity #" << id << endl;
	id_ = id;
}

Entity::~Entity()
{
	cout << "  [dtor] Entity #" << id_ << endl;
}

const char* Entity::role() const
{
	return "Сущность";
}

void Entity::show() const
{
	cout << role() << " #" << id_;
}

// Уровень 2. Сущность с названием
class NamedEntity : public Entity
{
protected:
	char name_[NAME_LEN];
public:
	NamedEntity(int id, const char* name);
	~NamedEntity();
	const char* getName() const { return name_; }
	// role() здесь НЕ переопределяется: используется версия из Entity.
	virtual void show() const;
};

NamedEntity::NamedEntity(int id, const char* name) : Entity(id)
{
	cout << "  [ctor] NamedEntity \"" << name << "\"" << endl;
	strcpy_s(name_, sizeof(name_), name);
}

NamedEntity::~NamedEntity()
{
	cout << "  [dtor] NamedEntity \"" << name_ << "\"" << endl;
}

void NamedEntity::show() const
{
	Entity::show();
	cout << " \"" << name_ << "\"";
}

// Уровень 3 (простое наследование): человек
class Person : public NamedEntity
{
protected:
	int birthYear_;
public:
	Person(int id, const char* name, int birthYear);
	~Person();
	int getBirthYear() const { return birthYear_; }
	virtual const char* role() const;
	virtual void show() const;
};

Person::Person(int id, const char* name, int birthYear) : NamedEntity(id, name)
{
	cout << "  [ctor] Person" << endl;
	birthYear_ = birthYear;
}

Person::~Person()
{
	cout << "  [dtor] Person" << endl;
}

const char* Person::role() const
{
	return "Человек";
}

void Person::show() const
{
	NamedEntity::show();
	cout << ", год рождения: " << birthYear_;
}

// Множественное наследование: команда = именованная сущность + сохранение в файл
// Конструкторы баз вызываются в порядке перечисления: NamedEntity, затем Serializable.
class Team : public NamedEntity, public Serializable
{
	char city_[TEXT_LEN];
public:
	Team(int id, const char* name, const char* city);
	~Team();
	virtual const char* role() const;
	virtual void show() const;
	virtual void serialize(ostream& os) const;
};

Team::Team(int id, const char* name, const char* city) : NamedEntity(id, name), Serializable()
{
	cout << "  [ctor] Team" << endl;
	strcpy_s(city_, sizeof(city_), city);
}

Team::~Team()
{
	cout << "  [dtor] Team" << endl;
}

const char* Team::role() const
{
	return "Команда";
}

void Team::show() const
{
	NamedEntity::show();
	cout << ", город: " << city_;
}

void Team::serialize(ostream& os) const
{
	os << id_ << ";" << name_ << ";" << city_;
}


// Матч: тоже множественное наследование (Entity + Serializable)
class Match : public Entity, public Serializable
{
	const Team* home_;
	const Team* away_;
	int homeGoals_;
	int awayGoals_;
public:
	Match(int id, const Team* home, const Team* away, int homeGoals, int awayGoals);
	~Match();
	virtual const char* role() const;
	virtual void show() const;
	virtual void serialize(ostream& os) const;
};

Match::Match(int id, const Team* home, const Team* away, int homeGoals, int awayGoals)
	: Entity(id), Serializable()
{
	cout << "  [ctor] Match" << endl;
	home_ = home;
	away_ = away;
	homeGoals_ = homeGoals;
	awayGoals_ = awayGoals;
}

Match::~Match()
{
	cout << "  [dtor] Match" << endl;
}

const char* Match::role() const
{
	return "Матч";
}

void Match::show() const
{
	Entity::show();
	cout << ": " << home_->getName() << " " << homeGoals_ << ":" << awayGoals_
	     << " " << away_->getName();
}

void Match::serialize(ostream& os) const
{
	os << id_ << ";" << home_->getId() << ";" << away_->getId() << ";"
	   << homeGoals_ << ";" << awayGoals_;
}

// Виртуальное наследование: ромб Person -> (Player, Coach) -> PlayerCoach
// Благодаря virtual в PlayerCoach будет ОДИН экземпляр Person.
class Player : virtual public Person
{
protected:
	int number_;
	char position_[TEXT_LEN];
public:
	Player(int id, const char* name, int birthYear, int number, const char* position);
	~Player();
	virtual const char* role() const;
	virtual void show() const;
};

// Person инициализируется здесь только при создании «чистого» Player.
// Если создаётся PlayerCoach, Person инициализирует именно он.
Player::Player(int id, const char* name, int birthYear, int number, const char* position)
	: Person(id, name, birthYear)
{
	cout << "  [ctor] Player" << endl;
	number_ = number;
	strcpy_s(position_, sizeof(position_), position);
}

Player::~Player()
{
	cout << "  [dtor] Player" << endl;
}

const char* Player::role() const
{
	return "Игрок";
}

void Player::show() const
{
	Person::show();
	cout << ", номер: " << number_ << ", позиция: " << position_;
}

class Coach : virtual public Person
{
protected:
	char licence_[TEXT_LEN];
public:
	Coach(int id, const char* name, int birthYear, const char* licence);
	~Coach();
	virtual const char* role() const;
	virtual void show() const;
};

Coach::Coach(int id, const char* name, int birthYear, const char* licence)
	: Person(id, name, birthYear)
{
	cout << "  [ctor] Coach" << endl;
	strcpy_s(licence_, sizeof(licence_), licence);
}

Coach::~Coach()
{
	cout << "  [dtor] Coach" << endl;
}

const char* Coach::role() const
{
	return "Тренер";
}

void Coach::show() const
{
	Person::show();
	cout << ", лицензия: " << licence_;
}

class PlayerCoach : public Player, public Coach
{
public:
	PlayerCoach(int id, const char* name, int birthYear, int number,
	            const char* position, const char* licence);
	~PlayerCoach();
	virtual const char* role() const;
	virtual void show() const;
};

// Конструктор самого «дальнего» класса обязан сам вызвать конструктор
// виртуальной базы Person. Вызовы Person(...) внутри Player и Coach
// в этом случае игнорируются.
PlayerCoach::PlayerCoach(int id, const char* name, int birthYear, int number,
                         const char* position, const char* licence)
	: Person(id, name, birthYear),
	  Player(id, name, birthYear, number, position),
	  Coach(id, name, birthYear, licence)
{
	cout << "  [ctor] PlayerCoach" << endl;
}

PlayerCoach::~PlayerCoach()
{
	cout << "  [dtor] PlayerCoach" << endl;
}

const char* PlayerCoach::role() const
{
	return "Игрок-тренер";
}

// Player и Coach оба переопределяют show() из общей виртуальной базы,
// поэтому PlayerCoach ОБЯЗАН переопределить её сам (иначе ошибка
// компиляции «no unique final overrider»).
void PlayerCoach::show() const
{
	Person::show();
	cout << ", номер: " << number_ << ", позиция: " << position_
	     << ", лицензия: " << licence_;
}

// ============================================================
// Закрытое наследование: судья «использует» Person, но не является им для внешнего кода
// ============================================================
class Referee : private Person
{
	char category_[TEXT_LEN];
public:
	Referee(int id, const char* name, int birthYear, const char* category);
	~Referee();
	using Person::getName;      // открываем наружу только одну функцию базы
	void info() const;
};

Referee::Referee(int id, const char* name, int birthYear, const char* category)
	: Person(id, name, birthYear)
{
	cout << "  [ctor] Referee" << endl;
	strcpy_s(category_, sizeof(category_), category);
}

Referee::~Referee()
{
	cout << "  [dtor] Referee" << endl;
}

void Referee::info() const
{
	cout << "Судья " << getName() << ", категория " << category_ << " | ";
	Person::show();
	cout << endl;
}

// Демонстрации
void title(const char* text)
{
	cout << endl << "==================== " << text << " ====================" << endl;
}

void demoSingleInheritance()
{
	title("1. Простое наследование: Entity -> NamedEntity -> Person");
	cout << "Создание объекта:" << endl;
	Person p(1, "Иван Петров", 1995);
	cout << "Описание: ";
	p.show();
	cout << endl;
	cout << "Конец блока, вызов деструкторов (в обратном порядке):" << endl;
}

void demoMultipleInheritance()
{
	title("2. Множественное наследование: Team : NamedEntity, Serializable");
	cout << "Создание объекта:" << endl;
	Team t(10, "Динамо", "Минск");
	cout << "Описание:     ";
	t.show();
	cout << endl << "В файл пишем: ";
	t.serialize(cout);
	cout << endl;
	cout << "Конец блока, вызов деструкторов:" << endl;
}

void demoVirtualInheritance()
{
	title("3. Виртуальное наследование (ромб): PlayerCoach");
	cout << "Создание объекта (Entity, NamedEntity, Person создаются ОДИН раз):" << endl;
	PlayerCoach pc(5, "Алексей Смирнов", 1988, 8, "Полузащитник", "UEFA B");

	// Один подобъект Person: обращение без указания пути однозначно.
	// Без virtual компилятор выдал бы ошибку неоднозначности.
	cout << "Имя: " << pc.getName() << ", id: " << pc.getId() << endl;

	const Player* asPlayer = &pc;
	const Coach* asCoach = &pc;
	const Person* viaPlayer = asPlayer;
	const Person* viaCoach = asCoach;
	cout << "Person через Player и через Coach - один и тот же объект? "
	     << (viaPlayer == viaCoach ? "да" : "нет") << endl;

	// Полиморфизм: через любую из баз вызывается версия PlayerCoach.
	cout << "Через Player*: ";
	asPlayer->show();
	cout << endl << "Через Coach*:  ";
	asCoach->show();
	cout << endl << "Через Person*: ";
	viaPlayer->show();
	cout << endl;
	cout << "Конец блока, вызов деструкторов:" << endl;
}

void demoPolymorphism()
{
	title("4. Виртуальные функции и полиморфизм");

	const int N = 8;
	Entity* p[N];            // массив указателей на базовый класс, как в методичке

	cout << "Создаём объекты чемпионата:" << endl;
	p[0] = new NamedEntity(1, "Высшая лига");
	p[1] = new Person(2, "Сергей Иванов", 1990);
	p[2] = new Player(3, "Иван Петров", 1995, 9, "Нападающий");
	p[3] = new Coach(4, "Пётр Сидоров", 1975, "UEFA Pro");
	p[4] = new PlayerCoach(5, "Алексей Смирнов", 1988, 8, "Полузащитник", "UEFA B");
	p[5] = new Team(10, "Динамо", "Минск");
	p[6] = new Team(11, "БАТЭ", "Борисов");
	p[7] = new Match(100, static_cast<Team*>(p[5]), static_cast<Team*>(p[6]), 2, 1);

	cout << endl << "Один цикл по Entity* - у каждого объекта своя версия функций:" << endl;
	for (int i = 0; i < N; i++) {
		cout << "  [" << p[i]->role() << "] ";
		p[i]->show();
		cout << endl;
	}
	cout << "(Высшая лига: role() не переопределён в NamedEntity -> версия Entity.)" << endl;

	cout << endl << "Полное имя функции отключает виртуальный механизм:" << endl;
	cout << "  p[4]->show()          : ";
	p[4]->show();
	cout << endl << "  p[4]->Entity::show()  : ";
	p[4]->Entity::show();
	cout << endl;

	cout << endl << "Поиск объектов, которые умеют сохраняться в файл (dynamic_cast):" << endl;
	for (int i = 0; i < N; i++) {
		Serializable* s = dynamic_cast<Serializable*>(p[i]);
		if (s != nullptr) {
			cout << "  " << p[i]->role() << " -> ";
			s->serialize(cout);
			cout << endl;
		}
	}

	cout << endl << "Удаление через Entity* (работают виртуальные деструкторы):" << endl;
	for (int i = 0; i < N; i++)
		delete p[i];
}

void demoAccess()
{
	title("5. Модификаторы доступа: Referee : private Person");
	cout << "Создание объекта:" << endl;
	Referee r(7, "Николай Орлов", 1980, "FIFA");

	cout << "getName() открыт через using: " << r.getName() << endl;
	r.info();

	// Следующие строки НЕ скомпилируются (раскомментируйте для проверки):
	// r.getBirthYear();        // ошибка: функция private в Referee
	// r.show();                // ошибка: private в Referee
	// Entity* e = &r;          // ошибка: преобразование к закрытой базе запрещено
	cout << "Конец блока, вызов деструкторов:" << endl;
}

int main()
{
	demoSingleInheritance();
	demoMultipleInheritance();
	demoVirtualInheritance();
	demoPolymorphism();
	demoAccess();
	return 0;
}