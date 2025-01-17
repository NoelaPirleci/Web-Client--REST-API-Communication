# TEMA 4 - Client Web. Comunicatie cu REST API
## Pirleci Noela Elena, 325CA

**Descriere**

Clientul implementat este un program care accepta comenzi de la tastatura si, in functie de fiecare comanda, trimite cereri catre un server. Scopul principal al acestui client este de a accesa o biblioteca virtuala prin intermediul unor comenzi date de la tastatura.


**Comenzi implementate**

Am implementat urmatoarele comenzi:
1. *register*
2. *login*
3. *enter_library*
4. *get_books*
5. *get_book*
6. *add_book*
7. *delete_book*
8. *logout*


**Flow-ul programului**

Am inceput programul prin a citi, in main, fiecare comanda.   
Pentru **register** citesc mai intai username-ul si parola. Inainte sa continui, verific daca datele introduse de la tastatura sunt valide sau nu(daca exista spatii sau nu). In cazul in care datele sunt in regula, le convertesc in format JSON, cu ajutorul unei biblioteci speciale(voi detalia mai multe despre aceasta alegere mai jos). Apoi, realizez o cerere de tip POST catre server, iar in functie de raspunsul primit de la acesta, afisez un mesaj pe ecran: de *succes* daca utilizatorul este inregistrat, sau *eroare* daca user-ul deja exista.  
Pentru *login* urmez aproximativ aceiasi pasi ca si la *register*. Dupa ce citesc datele legate de utilizator si verific daca acestea sunt in regula, trimit o cerere POST catre server, iar din raspunsul acestuia extrag cookie-urile, setez o variabila care ma ajuta sa retin daca este conectat un user si afisez un mesaj de succes sau intorc un mesaj de eroare.  
Pentru *enter_library* verific initial daca este logat un user in acel moment, iar daca aceasta conditie se indeplineste, trimit o cerere de tip GET catre server, primesc un raspuns de la acesta si in caz de succes, afisez un mesaj corespunzator, setez o alta variabila(*gained_access*), care ma ajuta la urmatoarele functii si extrag token-ul din mesaj.  
Pentru *get_books*, mai intai verific daca user-ul are deja acces la biblioteca, iar apoi trimit o cerere de tip GET catre server, iar din mesajul intors preiau lista de carti, pe care o convertesc in format JSON si o afisez.  
Pentru *get_book* verific din nou daca user-ul are acces la biblioteca, iar apoi verific daca id-ul cartii pe care o caut este valid. In cazul in care id-ul este bun, concatenez id-ul cartii la url, iar apoi trimit o cerere de tip GET, iar din raspunsul primit de la server afisez ori cartea, ori un mesaj de eroare.  
Pentru *add_book* verific iar daca user-ul are acces la biblioteca, iar apoi citesc datele despre o carte. Ma asigur ca numarul de pagini este format doar din cifre, iar apoi transform aceste date despre carte in format JSON. Trimit o cerere de tip POST catre server, iar in functie de raspunsul primit afisez un mesaj de succes daca am adaugat cartea, sau unul de eroare.  
Pentru *delete_book* verific, din nou, daca user-ul are acces la biblioteca, iar apoi ma asigur daca id-ul cartii pe care vrea sa o stearga este valid. In caz afirmativ, concatenez id-ul cartii la url si trimit o cerere catre server de tip POST(DELETE), iar in functie de raspuns afisez un mesaj de succes daca am eliminat cartea, sau unul de eroare.  
Pentru *logout* verific sa fie mai intai un user logat. Apoi, realizez o cerere de tip GET catre server, actualizez variabilele care ma ajutau sa retin ca un user este logat si ca are acces la biblioteca(*is_logged_in*, *gained_access*), eliberez cookie-urile si token-urile, iar in functie de raspunsul primit de la server afisez un mesaj de succes daca user-ul a fost delogat sau unul de eroare.  


**Erori tratate** 

Pentru comanda **REGISTER**:  
- Username-ul si parola (nu) contin spatii.

Pentru comanda **LOGIN**:  
- Username-ul si parola (nu) contin spatii.

Pentru comanda **ENTER_LIBRARY**:
- Utilizatorul este/ nu este logat.

Pentru comanda **GET_BOOKS**:
- Utilizatorul (nu) are acces la biblioteca.

Pentru comanda **GET_BOOK**:
- Utilizatorul (nu) are acces la biblioteca.
- ID-ul cartii cautate (nu) este valid.

Pentru comanda **ADD_BOOK**:
- Utilizatorul (nu) are acces la biblioteca.
- Numarul de pagini al cartii (nu) este valid.

Pentru comanda **DELETE_BOOK**: 
- Utilizatorul (nu) are acces la biblioteca.
- ID-ul cartii dorite (nu) este valid.

Pentru comanda **LOGOUT**:
- Utilizatorul este/ nu este logat.


**Alegere folosire biblioteca speciala pentru JSON**

Pentru parsarea datelor, am ales sa folosesc biblioteca *parson*, deoarece mi s-a parut mai simplu sa folosesc niste functii deja implementate, decat sa incerc sa fac manual aceste afisari. Dupa ce am terminat tema, cred ca a fost o decizie corecta sa fac acest lucru, pentru ca altfel mi-ar fi luat mult mai mult timp sa implementez si aceste aspecte. 
