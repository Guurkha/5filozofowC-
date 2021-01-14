
#include <array>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

std::mutex g_lockprint;

bool isDinner = false;



//klasa widelca, zawiera ID widelca
class Widelec {
	int ID;
public:
	std::mutex mutex;
	Widelec(int _ID)
	{
		ID = _ID;
	}
};


class Kelner {
	std::array<Widelec, 5> widelce
	{
	   {
		  { 0 },{ 1 },{ 2 },{ 3 },{ 4 },
	   }
	};

	std::mutex m1;
public:
	void popros_widelec(int ID)
	{
		//filozof bierze jedzenie i zaczyna jeść Blkowanie dwoch widelcow
		widelce[(ID + 1) % 5].mutex.lock(); //kazdy widelec ma swoj mutex, tutaj blokuemy  semafor daje widelce
		widelce[ID].mutex.lock();  //ten watek który czekał ma dostep do widelcow konczy jes
	}
	void opusc_widelec(int ID)
	{
		//filzof oddaje widelec i przestaje jesc, odblokwanie dwoch widelcow 
		widelce[(ID + 1) % 5].mutex.unlock();  //oddaje 
		widelce[ID].mutex.unlock();
	}
};




class Filozof {
private:
	//id filozofa
	int ID;
	//licznik ile zjadl
	std::string name;
	int licznik = 0;
	//kelner
	Kelner &w;
public:
	//kazdy filozof ma swoj watek
	std::thread t;
	//konstrukror filozofa, 
	Filozof(std::string _name, int _ID, Kelner& _w) : w(_w), t(&Filozof::dine, this) //tworzy sie nowy wątek (kontruktor dla thread) funkcja która przyjmuje 
	{
		name = _name;
		ID = _ID;
	}
	~Filozof()
	{
		if (t.joinable()) //sprawdza czy watek sie skonczyl
			t.join(); // zakoczenie watka
	}
	void dine()
	{
		do
		{
			myslenie(); //sleepy
			jedzenie(); //sleepy
		} while (isDinner);
	}
	void print(std::string text)
	{
		std::lock_guard<std::mutex> cout_lock(g_lockprint);
		std::cout
			<< name << text << std::endl;
	}
	void jedzenie()
	{
		//request forks
		w.popros_widelec(ID); //sprawdza czy są dostepne widelce dla ID filizofa
		print(" je");
		//printf("%d is eating\n", ID);

		//simulating eating time for philosopher
		std::this_thread::sleep_for(std::chrono::seconds(rand() % 2));
		//zwiekszamy oo 1
		incrementuj_licznik();
		print("  przestal jesc ");
		//opuszcza widelec
		w.opusc_widelec(ID);
	}
	void myslenie()
	{
		//po prostu czeka randmową liczbe 
		std::this_thread::sleep_for(std::chrono::seconds(rand() % 2));
		print(" mysli");
	}
	void incrementuj_licznik()  //inkrementujemy licznik ile razy ktoś zjadł
	{
		this->licznik++;
	}
	int get_licznik() //zwraca liczbe ile razy ktos zjadl
	{
		return this->licznik;
	}
	int getID() //zwraca id filozofa
	{
		return this->ID;
	}
	std::string getName() //zwraca imie filozofa
	{
		return this->name;
	}
};







int main() {
	//tworzymy kelnera
	Kelner w;
	//wykorzystujemy czas
	srand(time(NULL));
	//ustawiamy zmienna bool na true
	isDinner = true;
	//tworzymy kontener rozmiaru 5 na filozofów 
	std::array<Filozof, 5> filozofowie
	{
	   {
			//tworzymy 5 filozofów 
		  {"Sokrates", 0, w },{"Kartezjusz", 1, w },{"Platon", 2, w },{"Asterix", 3, w },{"Obelix", 4, w },
	   }
	};

	//symulacja tego kto zjadl 
	std::this_thread::sleep_for(std::chrono::seconds(rand() % 5)); //jak dlugo trwa uczta
	isDinner = false;


	for (int i = 0; i < 5; i++)
	{
		filozofowie[i].~Filozof();
	}
	
	std::cout << "\n\n-----PODSUMOWANIE---- \n\n";

		std::cout << "Sokrates zjadl: " << filozofowie[0].get_licznik() << " razy\n";
		std::cout << "Kartezjusz zjadl: " << filozofowie[1].get_licznik() << " razy\n";
		std::cout << "Platon zjadl: " << filozofowie[2].get_licznik() << " razy\n";
		std::cout << "Asterix zjadl: " << filozofowie[3].get_licznik() << " razy\n";
		std::cout << "Obelix zjadl: " << filozofowie[4].get_licznik() << " razy\n";

}
