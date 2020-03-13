#include <iostream>
#include <thread>
#include <algorithm>
#include "Graphic.h"
#include "Table.h"
#include "Worker.h"
#include "Supplier.h"

std::vector<int> generate_pattern(int n) {
    std::vector<int> vec(n);
    std::for_each(vec.begin(), vec.end(), [](int& v){ v = rand()%10; });
	return vec;
}

std::vector<Object::text> generate_vec(int y, int x, int n, int hsep = 2, int vsep = 1) {
	std::vector<Object::text> vec(n);
	for (int i(0); i < n; i++)
    {
        if(i == 5)
        {
            y += vsep;
            x -= hsep*i;
        }
		vec[i] = Object::text{x + hsep*i, y, Object::Color::normal, L"0", true};
    }
	return vec;
}

int main()
{
	int elements_n = 10;
	int workers_n  = 32;
	int amount     = 200;
	int t_rest     = 7;
	int t_collect  = 18;
	int t_work     = 7;
	int t_sup      = 2;

    char def_sett;
	std::cout<<"Parametry domyslne[t/n]?\n";   std::cin>>def_sett;

	if(def_sett == 'n')
    {
        std::cout<<"Liczba elementow(1-10): ";   std::cin>>elements_n;
        std::cout<<"Liczba pracownikow(1-32): "; std::cin>>workers_n;
        std::cout<<"Rozmiar dostaw(1-200): ";    std::cin>>amount;
        std::cout<<"Czas odpoczynku(1-30)[s]: "; std::cin>>t_rest;
        std::cout<<"Czas zbierania(1-30)[s]: ";  std::cin>>t_collect;
        std::cout<<"Czas pracy(1-30)[s]: ";      std::cin>>t_work;
        std::cout<<"Czas dostaw(1-30)[s]: ";     std::cin>>t_sup;

        if(     elements_n >  10) elements_n =  10;
        else if(elements_n <   1) elements_n =   1;
        if(      workers_n >  32)  workers_n =  32;
        else if( workers_n <   1)  workers_n =   1;
        if(         amount > 200)     amount = 200;
        else if(    amount <   1)     amount =   1;
        if(         t_rest >  30)     t_rest =  30;
        else if(    t_rest <   1)     t_rest =   1;
        if(      t_collect >  30)  t_collect =  30;
        else if( t_collect <   1)  t_collect =   1;
        if(         t_work >  30)     t_work =  30;
        else if(    t_work <   1)     t_work =   1;
        if(          t_sup >  30)      t_sup =  30;
        else if(     t_sup <   1)      t_sup =   1;
    }

	Graphic graphic;

	Table tab(graphic.CreateObject(generate_vec(3, 29, elements_n, 5, 2)), elements_n);

	Supplier supplier(&tab, graphic.CreateObject(generate_vec(4, 29, elements_n, 5, 2)),
                            graphic.CreateObject(0, 38), t_sup, amount, elements_n);

	Worker** worker = new Worker*[workers_n];
	std::thread* th = new std::thread[workers_n];

	supplier.Fill();
	supplier.Start();
	for (int i(0), w(0), h(0); i < workers_n; i++, w++)
    {
        if(h < 2 && w == 2)
            w = 4;
        else if(w == 6)
        {
            w = 0;
            h++;
        }
        worker[i] = new Worker(&tab, graphic.CreateObject(h*4, 2 + w*13),
                                     graphic.CreateObject(h*4, 8 + w*13),
                                     graphic.CreateObject(generate_vec(1 + h*4, 3 + w*13, elements_n)),
                                     generate_pattern(elements_n));

		th[i] = std::thread([&t_rest, &t_collect, &t_work](Worker* w) {
			while (w->Rest(t_rest) && w->Collect(t_collect) && w->Work(t_work)) {};
			}, worker[i]);
    }

	graphic.Start(workers_n, elements_n, [&tab]{ tab.Update(); });

	while (getch() != 'q');

	supplier.Stop();
	Worker::EndAll();
	graphic.Stop();

	for (int i(workers_n - 1); i >= 0; i--) {
		th[i].join();
		delete worker[i];
	}
	delete[] worker;
	delete[] th;

	return 0;
}
