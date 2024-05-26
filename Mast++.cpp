/*
 * Mast++.cpp
 *
 *  Created on: 11 mag 2017
 *      Author: fp
 */
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <string>
#include <ctime>
#include <cctype>

//10-12-9
//6-7-4
//11-12-10
//Numero di colori (che poi vengono sempre usati come numeri)
#define N_COLS 6
//Numero massimo di tentativi
#define N_ATTEMS 7
//Numero di colori da indovinare
#define N_POS 4
//Numero di partite che fa a ripetizione
#define N_ALONE 100

/*************************************************************/
//USING

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::rand;
using std::string;
using std::next_permutation;
using std::isspace;
using std::find_if;

/*************************************************************/
//CLASSI


//rules è una classe in cui si salvano i limiti vari (le regole)
//per ogni partita, di modo che si possa decidere se cambiarle
class rules {
public:
  int colours;   //Numero di colori (che poi vengono sempre
                 //usati come numeri)
  int attempts;  //Numero massimo di tentativi
  int positions; //Numero di colori da indovinare

  rules(const int a, const int b, const int c) { colours = a; attempts = b; positions = c; }
  rules(const vector<int>& settings) { colours = settings[0]; attempts = settings[1]; positions = settings[2]; }
  rules(): colours(0), attempts(0), positions(0) { }
};


//Questa classe contiene tutti quanti i tentativi fatti per
//indovinare la sequenza (attempts) e i relativi risultati (ox).
//Le "O" indicano colore giusto + posozione giusta
//Le "X" indicano colore giusto + posizione sbagliata
class generic_table {
public:
  vector<vector<int> > attempts;
  vector<vector<int> > ox;  //Il vettore ox relativo a un
                            //tentativo conterrà 2 numeri:
                            //il primo indicherà le "O", il
                            //sedondo le "X"
                            
  rules my_rules;  //Ogni tabella ha le proprie regole

  void print() const;
  
  //Restituisce il numerondi tentativi fatti (ovvero la
  //lunghezza della tavola)
  vector<int>::size_type size() const {
    return attempts.size();
  }

  generic_table(): attempts(), ox(), my_rules() {}
};


/*************************************************************/
//DEFINIZIONI DI FUNZIONI


//Restituisce un intero casuale nel range [0, n).
int nrand(int n);

//Restituisce la soluzione generata casualmente
vector<int> gen_solution(const rules& my_r);

//Legge dall'utente una combinazione che può ossere o un
//tentativo durante il gioco oppure la scelta della soluzione
//all'inizio
vector<int> read_combination(const generic_table& tab);

//Restituisce un vettere (di lunghezza 2) che, data una
//soluzione e un tentarivo, contiene i risultati del controllo
vector<int> check(vector<int> attempt, vector<int> solut);

//Stampa la tabella e le varie regole
void print_table(const generic_table& tab);

//Data una soluzione e inizializzata la tabella con relative
//regole il computer cerca di indovinare. Restituisce l'esito
bool comp_plays(generic_table& tab, const vector<int>& solution, vector<int> check_method(vector<int> a, vector<int> b) = check);

//Analizzata la tabella (tab) e dato il vettore di "colori"
//che crede componga la combinazione vincente, restituisce
//la combinazione che crede migliore
vector<int> best_attempt(const generic_table& tab, vector<int> poss_cols);

//Come comp_plays, ma gioca l'utente. Restituisce l'esito
bool user_plays(generic_table& tab, const vector<int>& solution, vector<int> check_method(vector<int> a, vector<int> b) = check);

//Dato il numero si colori, mette in colours_order l'ordine
//in cui il computer dovrà provare i vari colori
void shuffle_colours(const int num_of_cols, vector<int>& colours_order);

//L'utente dice l'esito del tentativo nel caso in cui stia
//giocando una partita vera
vector<int> check_user(vector<int> a, vector<int> b);

//Funzione di appoggio che serve a read_combinarion (isspace è
//overloadata e serviva una funzione unica per il template
//find_if
bool not_space(char c) { return !isspace(c); }

//Per cambiare le impostazioni di gioco
void change_settings(vector<int>& setts);


/*************************************************************/
//MAIN


int main() {

  srand(std::time(0));
  vector<int> sol;  //la soluzione
  vector<int> settings = {N_COLS, N_ATTEMS, N_POS, N_ALONE};

  int new_game;  //indicherà se l'utente vuole giovare ancora
                 //(new_game) != 0) oppure no (== 0);
  
  //Invariante: non ci sono partite in corso, se si deve giocare
  //una partita la solizione è ancora da decidere e la tavola
  //di gioco ancora da inizializzare
  do {
    
  generic_table my_tab;  //La tavola di gioco
  
  //Imposto le relative regole (da costanti, non do quindi la
  //possibilità di cambiare impostazioni)
  my_tab.my_rules = rules(settings);

  int player;   //indica chi imposterà la combinazione vincente
                //e chi dovrà indovinare
                
  std::system("clear\n");
  cout << "0: Change settings (or proceed with default)" << endl
          << "1: Player vs Player" << endl
          << "2: The Player sets up, the Computer plays" << endl
          << "3: The Computer sets up, the Player plays" << endl
          << "4: Computer vs Computer" << endl
		  << "5: The Computer plays " << settings[3] << " times by itself and shows its stats" << endl
		  << "6: If you're playing in real life the computer tells you what to do" << endl;
	
	//Alla fine del ciclo è stato impostato correttamente "player"
	//ovvero sarà un intero nel range [1, 5]
	bool invalid_input;  //Imput da ripetere o no
  do {
		  cin >> player;
		  invalid_input = !cin || player < 0 || player > 6;
		  if (invalid_input) {
			  cout << "wrong input, repeat." << endl;
			  cin.clear();  //nel caso di un input non numerico
		  }
	  } while (invalid_input);

    //Si sceglie quali settaggi adottare
    if (player == 0) {
      change_settings(settings);
      new_game = 1;  //Questo perché così va alla fine del ciclo
                     //e lo ricomincia ripartendo dal menù
                     //principale senza chiedere se si vuole
                     //fare una nuova partita
    }
    //Un utente sceglie la soluzione e un utente gioca
	  else if (player == 1) {
  		std::system("clear\n");
		  cout << "Write the combination: ";
		  sol = read_combination(my_tab);
		  user_plays(my_tab, sol);
	  }
	  //Un utente sceglie la soluzione e il Computer gioca
	  else if (player == 2){
  		std::system("clear\n");
  		cout << "Write the combination: ";
  		sol = read_combination(my_tab);
  		comp_plays(my_tab, sol);
	  }
	  //La soluzione è generata casualmente, l'utente gioca
	  else if (player == 3){
  		sol = gen_solution(my_tab.my_rules);
  		user_plays(my_tab, sol);
	  }
	  //La soluzione è generata casualmente, il Computer gioca
	  else if (player == 4){
  		sol = gen_solution(my_tab.my_rules);
  		comp_plays(my_tab, sol);
	  }
	  //Il Computer gioca 500 volte e dice com'è andato
	  else if (player == 5) {

		  int wins(0);  //Contatore di vittorie
		  
		  //Invariante: giocate i partite, vinte wins partite
		  for (int i = 0; i != settings[3]; i++) {
		    
			  generic_table comp_tab;  //Definisco una nuova tabella
			                           //perché voglio che ogni volta
			                           //venga distrutta e ricreata
			  comp_tab.my_rules = rules(settings);
			  sol = gen_solution(my_tab.my_rules);
			  cout << i + 1 << endl;
			  wins += comp_plays(comp_tab, sol);
		  }
		  //Printo i risultati
		  cout << wins << "/" << settings[3] << " (" << (double)wins * 100 / settings[3] << "%)." << endl;
	  }
	  //Giochi una partita vera
	  else if (player == 6) {
	    comp_plays(my_tab, sol, check_user);
	  }

    //Non deve chiedere di fare una nuova partita nel caso
    //in cui l'utente abbia solo chiesto di cambiare settings
    if (player) {
  	  cout << "New game?" << endl << "0: No" << endl << "1: Yes" << endl;
  	  cin >> new_game;
    }
  } while(new_game);

  return 0;
}


/*************************************************************/
//FUNCTIONS


//Restituisce un intero casuale nel range [0, n).
int nrand(int n)
{
 if (n <= 0 || n > RAND_MAX)
   throw std::domain_error("Argument to nrand is out of range");
   
 const int bucket_size = RAND_MAX / n;
 int r;
 do r = rand() / bucket_size;
 while (r >= n);
 return r;
}

/******************************/


//Restituisce la soluzione generata casualmente
vector<int> gen_solution(const rules& my_r) {
  vector<int> ret;   //Vettore che verrà returnato

  //invariante: messo in ret (i) colori scelti casualmente
  //nel range [0, my_r.colours)
  for (int i = 0; i != my_r.positions; ++i)
    ret.push_back(nrand(my_r.colours));

   return ret;
}


/******************************/

//Member function per printare solo tabella con realtivi esiti
void generic_table::print() const {
  
  //Gli asterischi rappresentano la soluzione nascosta
  cout << "\t\t*";
  //Invariante: printati i asterischi
  for (int i = 1; i != my_rules.positions; ++i)
    cout << "\t*";
  cout << endl << endl;
  typedef vector<vector<int> >::size_type vc_sz;
  
  //Invatiante: printati i tentativi con relativi esiti
  for (vc_sz i = 0; i != size(); ++i) {
    cout << i + 1;
    cout << "\t\t" << attempts[i][0] << "\t";
    //Invariante: printati j colori dell'i-esimo tentativo
    for (int j = 1; j != my_rules.positions; ++j)
      cout << attempts[i][j] << "\t";
    //Vengono printati esiti:
    //O = colore giusto + posto giusto
    //X = colore giisto + posto sbagliato
    for (int j = 0; j != ox[i][0]; ++j)
      cout << "O";
    for (int j = 0; j != ox[i][1]; ++j)
      cout << "X";
    cout << endl << endl;
  }
  cout << endl;
}


/******************************/

//Legge dall'utente una combinazione che può ossere o un
//tentativo durante il gioco oppure la scelta della soluzione
//all'inizio
//Viene letto per comodità un intero di 4 cifee e verrà
//spezzettato e messe le cifre in un vettore. Per
//spezzettarlo si dividerà questo numero per potenze di 10
vector<int> read_combination(const generic_table& tab) {
  unsigned long int pow = 1;  //Sarà la potenza di 10 per estrarre la cifra
                //dal numero
  unsigned long int combination;   //E' l'intero che rappresenta la
                     //combinazione
  cin >> combination;
  vector<int> to_push;  //Sarà proprio la combinazione che
                        //viene restituita
  //Si rende pow dello stesso ordine di grandezza della prima
  //cifra da sinistra
  for (int i = 1; i != tab.my_rules.positions; i++)
    pow*=10;
    
  //Invariante: estratte i cifre da combination e messe in
  //to_push. combination è stroncato di i cifre da sinistra.
  //pow ha l'ordine di grandezza della cifra più a sinistra
  for (int i = 0; i != tab.my_rules.positions; i++) {
      to_push.push_back(combination / pow);
      combination -= (combination / pow) * pow;
      pow /= 10;
  }
  return to_push;
}




/******************************/


//Restituisce un vettere (di lunghezza 2) che, data una
//soluzione e un tentarivo, contiene i risultati del controllo
vector<int> check(vector<int> attempt, vector<int> solut) {
  
    vector<int> ret(2, 0);  //Vettore di esiti da returnare,
                            //le "O" e le "X" vengono messe a 0
    typedef vector<int>::size_type vc_sz;
    vc_sz size_att(attempt.size());  //size_att è la lunghezza
                                     //delle combinazioni,
                                     //corrisponde a positions
                                     //in generic_table
    //Invariante: verificato che i primi i elementi siano "O" e
    //ret[0] corrisponde al loro numero
    for (vc_sz i = 0; i != size_att; ++i) {
        if (solut[i] == attempt[i]) {
            attempt[i] = solut[i] = -1;  //Si mettono a -1
                                         //perché poi questi
                                         //elementi vengono
                                         //eliminati
            ret[0]++;
        }
    }
    
    //Vengono elominati da attempt e da solut tutti gli
    //elementi "O" perché non devono essere analizzati 2 volte
    attempt = vector<int>(attempt.begin(), remove(attempt.begin(), attempt.end(), -1));
    solut = vector<int>(solut.begin(), remove(solut.begin(), solut.end(), -1));
    
    //Invariante: veroficato che i primi i elementi di solut
    //siano "X". Questi vengono eliminati da attempt di modo
    //da non essere analizzati 2 volte. Il numero di "X" è in
    //ret[1]
    for (vc_sz i = 0; i != solut.size(); ++i) {
        vector<int>::iterator it = find(attempt.begin(), attempt.end(), solut[i]);
        
        //Se l'elemento uguale viene trovato
        if (it != attempt.end()) {
            //attempt.erase(it);  //erase è lento per i vettori
            *it = -1;
            ret[1]++;
        }
    }
    return ret;
}


/******************************/

//Data una soluzione e inizializzata la tabella con relative
//regole il computer cerca di indovinare. Restituisce l'esito.
//L'idea è che alla prima mossa prova solo un colore, vede
//quanti ce ne sono di quel colore e alla mossa dopo prova
//con quel numero di quel colore più un altro colore, di cui
//adesso conosce il numero e così via. A ogni mossa prova, dati
//i colori a cui si sta restringendo al momento, una
//combinazione che, assegnata a check assieme a ogni tentativo
//fatto fino ad allora, dia come esito lo stesso associato al
//rispettivo tentativo
bool comp_plays(generic_table& tab, const vector<int>& solution, vector<int> check_method(vector<int> a, vector<int> b)) {
    vector<int> colours_order;  //E' il vettore che conserva
                                //l'ordine con cui vengono
                                //provati i vari colori
    //Si genera casualmente l'ordine dei colori e viene
    //messo, appunto, in colours_order
    shuffle_colours(tab.my_rules.colours, colours_order);

    //next_colour_it è l'iteratore che punta al prossimo
    //colore che verrà provato
    vector<int>::const_iterator next_colour_it = colours_order.begin();

    //possible_colours indica gli attuali colori che verranno
    //provati. Inizialmente è fatto da un solo colore, ovvero
    //il primo di colours_order
    vector<int> possible_colours(tab.my_rules.positions, *next_colour_it);

    //Invariante: fatti tab.size() tentativi. L'esito
    //dell'ultimo tentativo è in tab.ox[tab.size() - 1].
    do {
        vector<int> next_attempt;  //Conterrà il prossimo
                                   //tentativo
        print_table(tab);
        
        //La prima volta prova un solo colore
        if (!tab.size()) {
            next_attempt = possible_colours;
        } else {
          //Dalla seconda volta in poi può controllare le
          //mosse precedenti e viene chiamata best_attempt
          next_attempt = best_attempt(tab, possible_colours);
        }

        //viene aggiornata la tavola
        tab.attempts.push_back(next_attempt);
        tab.ox.push_back(check_method(next_attempt, solution));

        
        int correct_colours = tab.ox[tab.size() - 1][0] + tab.ox[tab.size() - 1][1]; //Numero di X + numero di O per
                           //capire quanti colori sono giusti
                           
        //new_possible_colours è il vettore che conterrà il
        //nuovo colore. La sua lunghezza dinenderà da quanti
        //colori giusti sono atati trovati fino a qui. Questo
        //vettore viene poi copiato in possible_colours
        //sovrascrivendo i colori che sicurp non sono giusti
        vector<int> new_possible_colours(tab.my_rules.positions - correct_colours, *++next_colour_it);
        
        if (!new_possible_colours.empty())
            copy(new_possible_colours.begin(), new_possible_colours.end(), possible_colours.begin() + correct_colours);

    //Continua se hai ancora tentativi a disposizione e ancora
    //non hai vinto
    } while (tab.ox[tab.size() - 1][0] != tab.my_rules.positions && tab.size() != tab.my_rules.attempts);

  print_table(tab);

  bool won = tab.ox[tab.size() - 1][0] == tab.my_rules.positions;  //Se hai vinto è uguale a true
  if (won)
    cout << "Computer won!" << endl;
  else
    cout << "Computer lost!" << endl;
  cout << "The combination was:";
  for (vector<int>::size_type i = 0; i != solution.size(); i++)
    cout << " " << solution[i];
  cout << endl << endl;

  return won;
}


/******************************/


//Analizzata la tabella (tab) e dato il vettore di "colori"
//che crede componga la combinazione vincente, restituisce
//la combinazione che crede migliore
vector<int> best_attempt(const generic_table& tab, vector<int> poss_cols) {
  
  bool matching_results;  //Se l'esito appena ottenuto è
                          //coincidente con quello nella
                          //tavola assume valore true
                          
    //Qui si provano tutte le combinazioni con i colori dati
    //fino a che una possa comportarsi da soluzioni per tutti
    //i tentativi fatti
    do {
        int attempts_checked = 0; //tentativi della tavola
                                  //controllati con una stessa
                                  //combinazione

        //Qui si prova l'attuale combinazione con i tentativi
        //nella tavola. Se l'esito è uguale a quello nella
        //tavola allora quella attuale potrebbe essere la
        //combinazione vincente,
        //Invariante: attempts_checked indica il numero
        //di tentativi, presenti nella tavola, controllati
        do {
          matching_results = tab.ox[tab.size() - 1 - attempts_checked] == check(tab.attempts[tab.size() - 1 - attempts_checked], poss_cols);  //Il controllo va dall'ultimo
                                //elemento al pirmo per
                                //stroncare in maniera
                                //anticipata più combinazioni
                                //sbagliate possibile
          attempts_checked++;
        } while (attempts_checked != tab.size() && matching_results);

        //Se matching_results è false significa che l'esito
        //non va bene per almeno uno dei tentativi fatti,
        //si cambia combinazione e si ricomincia a provare dal
        //primo tentativo nella tavola
        if (!matching_results) next_permutation(poss_cols.begin(), poss_cols.end());
    } while (!matching_results);

    return poss_cols;
}


/******************************/

//Stampa la tabella e le varie regole
void print_table(const generic_table& tab) {

  std::system("clear\n");
    cout << "Numbers are in the range [0, " << tab.my_rules.colours - 1 << "]." << endl
            << "Attempts done: " << tab.size() << "/" << tab.my_rules.attempts << "." << endl << endl;
    tab.print();
}


/******************************/

//Come comp_plays, ma gioca l'utente. Restituisce l'esito
bool user_plays(generic_table& tab, const vector<int>& solution, vector<int> check_method(vector<int>, vector<int>)) {
  vector<int> user_attempt;  //la combinazione dell'utente
  do {
    print_table(tab);
    cout << endl << "Write the combination: ";
    user_attempt = read_combination(tab);

        //aggiorna la tabella
        tab.attempts.push_back(user_attempt);
        tab.ox.push_back(check_method(user_attempt, solution));

  
    //Continua se hai ancora tentativi a disposizione e ancora
    //non hai vinto
  } while (tab.ox[tab.size() - 1][0] != tab.my_rules.positions && tab.size() != tab.my_rules.attempts);

  print_table(tab);

  bool won = tab.ox[tab.size() - 1][0] == tab.my_rules.positions;   //Se hai vinto è uguale a true
  if (won)
    cout << "You won!" << endl;
  else
    cout << "You lost!" << endl;

  cout << "The combination was:";
  for (vector<int>::size_type i = 0; i != solution.size(); i++)
    cout << " " << solution[i];
  cout << endl << endl;

  return won;
}


/******************************/

//Dato il numero si colori, mette in colours_order l'ordine
//in cui il computer dovrà provare i vari colori
void shuffle_colours(const int num_of_cols, vector<int>& colours_order) {
   vector<int> cols;  //vettore fatto dai colori messi in
                      //ordine

   colours_order.clear();  //toglie la sporcizia da
                           //colours_order
   //viene costruito cols come voluto
   for (int i = 0; i != num_of_cols; ++i)
     cols.push_back(i);

   //Invariante: estratti da cols (cols.size() (originale) -
   //cols.size() (attuale)) colori in maniera casuale,
   //cancellati da cols e messi in colours_order
   while (!cols.empty()) {
     vector<int>::iterator it = cols.begin();
     if (cols.size() > 1)
       it += nrand(cols.size() - 1);
     //cout << "hi" << endl;
     colours_order.push_back(*it);
     cols.erase(it);
   }
}


/******************************/

/*
//Restituisce un vettere (di lunghezza 2) che, data una
//soluzione e un tentarivo, contiene i risultati del controllo.
//E' un tentativo di fare una versione più veloce di check
//PER ORA NON VA BENE
vector<int> check_fast(vector<int> attempt, vector<int> solut) {
  
    vector<int> ret(2, 0);  //Vettore di esiti da returnare,
                            //le "O" e le "X" vengono messe a 0
            
    for (vector<int>::size_type i = 0; i != attempt.size(); ++i) {
      vector<int>::iterator it = attempt.begin();
      
      while (it != attempt.end() && *it != solut[i])
        ++it;
        
      if (it != attempt.end()) {
        if (it - attempt.begin() == i)
          ret[0]++;
        else
          ret[1]++;
        *it = -1;
      }   
   }
   
   return ret;
}
*/


/******************************/

//L'utente dice l'esito del tentativo nel caso in cui stia
//giocando una partita vera
vector<int> check_user(vector<int> try_this, vector<int> b) {
  vector<int> ret;
  
  cout << endl << "Try this:" << endl;
  for (vector<int>::const_iterator it = try_this.begin(); it !=  try_this.end(); it++)
    cout << *it << " ";
  cout << endl;
  cout << "Now insert the number of Os and of Xs: ";
  
  int ox;
  
  for (int i = 0; i != 2; ++i) {
    cin >> ox;
    ret.push_back(ox);
  }
  
  cout << endl;
  
  return ret;
}
  

/******************************/

/*

vector<int> read_combination(const generic_table& tab) {
  vector<int> ret(tab.my_rules.positions, 0);
  string combination;
  
  cin.clear();
  std::getline(cin, combination);
  
  string::reverse_iterator it = combination.rbegin();
  vector<int>::reverse_iterator ret_it = ret.rbegin();
  
  while (it != combination.rend()) {
    int pow = 1;
    it = find_if(it, combination.rend(), not_space);
    while (it != combination.rend() && !isspace(*it)) {
      *ret_it += (*it++ - '0') * *pow;
       pow *= 10;
    }
    ret_it++;
  }
  
  return ret;
}
    
  */
  
/******************************/
  
  

//Per cambiare le impostazioni di gioco
//O si scelgono settaggi già esistenti oppure
//si scelgono personalizzati.
void change_settings(vector<int>& setts) {
  system("clear\n");
  
  int choice;
  int times = setts[3];
  
  cout << "0: Back" << endl
       << "1: Standard" << endl
       << "2: Very big" << endl
       << "3: Massive" << endl
       << "4: Personalize" << endl
       << "5: Chose number of times the Computer plays alone" << endl;
  cout << "User's choice: ";
  cin >> choice;
  
  if (!choice)
    return;
  else if (choice == 1) {
    setts.clear();
    setts.push_back(6);
    setts.push_back(7);
    setts.push_back(4);
  }
  else if (choice == 2) {
    setts.clear();
    setts.push_back(10);
    setts.push_back(12);
    setts.push_back(9);
  }
  else if (choice == 3) {
    setts.clear();
    setts.push_back(11);
    setts.push_back(12);
    setts.push_back(10);
  }
  else if (choice == 4) {
    setts.clear();
    int value;
    cout << endl << "Write, in order:" << endl
         << "number of colours" << endl
         << "maximum number of attemps" << endl
         << "lenght of combinations" << endl;
    cout << "User's choice: ";
    
    for (int i = 0; i != 3; ++i) {
      cin >> value;
      setts.push_back(value);
    }
  }
  else if (choice == 5) {
    cout << endl << "User's choice: ";
    cin >> times;
    setts.erase(setts.begin()+3);
  }
  setts.push_back(times);
  
}