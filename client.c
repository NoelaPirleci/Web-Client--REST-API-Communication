#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

/* Definirea unor constante */
#define HOST "34.246.184.49"
#define PORT 8080
#define BUFFER_LEN 100
#define COMMAND_LEN 20
#define URL_LEN 50
#define LEN 100

/* Comenzi */
#define REGISTER_COMMAND "register"
#define LOGIN_COMMAND "login"
#define ENTER_LIBRARY_COMMAND "enter_library"
#define GET_BOOKS_COMMAND "get_books"
#define GET_COMMAND "get_book"
#define ADD_COMMAND "add_book"
#define DELETE_COMMAND "delete_book"
#define LOGOUT_COMMAND "logout"
#define EXIT_COMMAND "exit"

/* Variabile globale */
char *user_info[LEN];
char *message;
char *request;
char tokens[500];
char cookies[500];
int is_logged_in = 0;
int gained_access = 0;

/* Functie cu ajutorul careia transform in format JSON */
char* convert_to_JSON_format(char username[], char password[]) {

    JSON_Value *value = json_value_init_object();
    if (value == NULL) {
        printf("JSON_Value failed!\n");
        return;
    }

    JSON_Object *object = json_value_get_object(value);
    if (object == NULL) {
        printf("JSON_Object failed!\n");
        return;
    }

    json_object_set_string(object, "username", username);
    json_object_set_string(object, "password", password);

    return json_serialize_to_string_pretty(value);
}

/* Functie care verifica daca username-ul si parola nu au spatii */
int verify_if_username_password_are_ok(char *username, char *password) {

    while(*username != '\0') {
        if (strncmp(username, " ", 1) == 0) {
            return 1;
        }
        username++;
    }

    while (*password != '\0') {
        if (strncmp(password, " ", 1) == 0) {
            return 1;
        }
        password++;
    }
    
    return 0;  
}

/* Functie care verifica daca id-ul unei carti este format doar din cifre */
int verify_if_id_book_is_valid(char *book_id) {

    int length = strlen(book_id);  
    for (int i = 0; i < length; i++) {
        if (book_id[i] < '0' || book_id[i] > '9') {  
            printf("ERROR: Invalid book id!\n");
            return 1;
        }
    }
    return 0;
}

/* Functie care verifica daca numarul de pagini este format doar din cifre */
int verify_if_details_are_ok(char *page_count) {

    int length = strlen(page_count);  
    for (int i = 0; i < length; i++) {
        if (page_count[i] < '0' || page_count[i] > '9') {  
            printf("ERROR: Wrong data type for page_count!\n");
            return 1;
        }
    }
    return 0;
}

/* Functie care preia cookie-ul de sesiune */
void return_cookie_session(char *only_cookie) {

    only_cookie = only_cookie + 12;
    only_cookie[strlen(only_cookie) - 1] = '\0';
    only_cookie = strtok(only_cookie, ";");
    strcpy(cookies, only_cookie);
}

/* Functia REGISTER */
void register_command(int sockfd, char url[]) {

    char username[LEN];
    char password[LEN];

    printf("username=");
    fgets(username, LEN, stdin);
    username[strlen(username) - 1] = '\0';

    printf("password=");
    fgets(password, LEN, stdin);
    password[strlen(password) - 1] = '\0';

    /* Copii ale datelor initiale pentru care testez corectitudinea lor */
    char *copy_username = (char *)malloc(LEN * sizeof(char));
    char *copy_password = (char *)malloc(LEN * sizeof(char));

    strcpy(copy_username, username);
    strcpy(copy_password, password);

    int all_good = verify_if_username_password_are_ok(copy_username, copy_password);
    if (all_good == 1) {
        printf("ERROR: Username or password are not valid!\n");
        return;
    }

    /* Convertirea datelor in format JSON */
    char *user_data = convert_to_JSON_format(username, password);
    char *data[] = {user_data};
    /* Realizarea cererii POST */
    request = compute_post_request("34.246.184.49:8080", url, "application/json", data, 1, NULL, NULL, NULL);
    /* Trimiterea cererii catre server*/
    send_to_server(sockfd, request);
    /* Primirea raspunsului de la server */
    message = receive_from_server(sockfd);

    /* Verificare raspunsului */
    if (strstr(message, "ok") != NULL) {
        printf("SUCCESS: Registered successfully!\n");
    } else if (strstr(message, "error") != NULL) {
        char *error = strstr(message, "error");
        error[strlen(error) - 1] = '\0';
        error = error + 8;
        error[strlen(error) - 1] = '\0';
        printf("ERRROR: %s\n", error);
    } else {
        printf("ERROR!\n");
    }

    /* Elibererea resurselor folosite */
    free(copy_username);
    free(copy_password);
    free(user_data);
    free(request);
    free(message);   
}

/* Functia LOGIN */
void login_command(int sockfd, char url[]) {

    char username[LEN];
    char password[LEN];

    printf("username=");
    fgets(username, LEN, stdin);
    username[strlen(username) - 1] = '\0'; 

    printf("password=");
    fgets(password, LEN, stdin);
    password[strlen(password) - 1] = '\0';

    /* Copii ale datelor initiale pentru care testez corectitudinea lor */
    char *copy_username = (char *)malloc(LEN * sizeof(char));
    char *copy_password = (char *)malloc(LEN * sizeof(char));

    strcpy(copy_username, username);
    strcpy(copy_password, password);

    int all_good = verify_if_username_password_are_ok(copy_username, copy_password);
    if (all_good == 1) {
        printf("ERROR: Username or password are not valid!\n");
        return;
    }

    /* Convertirea datelor in format JSON */
    char *user_data = convert_to_JSON_format(username, password);
    char *data[] = {user_data};
    /* Realizarea cererii POST */
    request = compute_post_request("34.246.184.49:8080", url, "application/json", data, 1, NULL, NULL, NULL);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);

    /* Verificarea raspunsului */
    if (strstr(message, "ok") != NULL) {
        printf("SUCCESS: Logged in successfully!\n");
        /* Marcarea unei variabile care retine daca s-a logat un user */
        is_logged_in = 1;
        char *only_cookie = strstr(message, "Set-Cookie");
        /* Intoarcerea unei sesiuni de cookie */
        return_cookie_session(only_cookie);  
    } else if (strstr(message, "error") != NULL) {
        char *error = strstr(message, "error");
        error[strlen(error) - 1] = '\0';
        error = error + 8;
        error[strlen(error) - 1] = '\0';
        printf("ERRROR: %s\n", error);
    } else {
        printf("ERROR!\n");
    }

    /* Eliberarea resurselor */
    free(copy_username);
    free(copy_password);
    free(user_data);
    free(request);
    free(message);
}

/* Functia ENTER_LIBRARY */
void enter_library_command(int sockfd, char url[]) {

    /* Verificare daca s-a logat un user */
    if (is_logged_in == 0) {
        printf("ERROR: Log in first!\n");
        return;
    }
    
    char *cookies_data[] = {cookies};
    /* Realizarea cererii GET */
    request = compute_get_request("34.246.184.49:8080", url, NULL, cookies_data, 1, NULL, 0);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);
    
    /* EXtragerea token-ului */
    char *token = strstr(message, "token");

    if (token != NULL) {
        token[strlen(token) - 1] = '\0';
        token = token + 8;
        token[strlen(token) - 1] = '\0';
        strcpy(tokens, token);
    }

    /* Verificarea raspunsului */
    if (strstr(message, "ok") != NULL) {
        printf("SUCCESS: Entered library successfully!\n");
        /* Marcarea unei variabile care demonstreaza ca un user are 
        acces la biblioteca */
        gained_access = 1;
        } else if (strstr(message, "error") != NULL) {
        char *error = strstr(message, "error");
        error[strlen(error) - 1] = '\0';
        error = error + 8;
        error[strlen(error) - 1] = '\0';
        printf("ERRROR: %s\n", error);
    } else {
        printf("ERROR!\n");
    }

    /* Eliberarea resurselor */
    free(request);
    free(message);
}

/* Functia GET_BOOKS */
void get_books_command(int sockfd, char url[]) {

    /* Verificare daca user-ul are acces la biblioteca */
    if (gained_access == 0) {
        printf("ERROR: Access the library first!\n");
        return;
    }

    /* Realizarea cererii GET */
    request = compute_get_request("34.246.184.49:8080", url, NULL, NULL, 0, tokens, 0);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);

    /* Extrage lista de carti in format JSON */
    char *list_books = strtok(message, "[");
    list_books = strtok(NULL, "\n[");
    char str[strlen(list_books) + 2];
    str[0] = '[';
    strcpy(&str[1], list_books);
    str[strlen(list_books) + 1] = '\0';

    JSON_Value *val = json_parse_string(str);
    char *books = json_serialize_to_string_pretty(val);
    printf("%s\n\n", books);

    /* Eliberarea resurselor */
    json_free_serialized_string(books);
    json_value_free(val);
    free(request);
    free(message);
}

/* Functia GET_BOOK */
void get_book_command(int sockfd, char url[]) {

    char id_book[LEN];
    printf("id=");
    fgets(id_book, LEN, stdin);
    id_book[strlen(id_book) - 1] = '\0';

    /* Verificare daca user-ul are acces la biblioteca */
    if (gained_access == 0) {
        printf("ERROR: Access the library first!\n");
        return;
    }

    /* Verificare daca id-ul cartii este valid */
    int id_is_ok = verify_if_id_book_is_valid(id_book);
    if (id_is_ok == 1) {
        printf("ERROR: Invalid book id!\n");
        return;
    }

    /* Concatenarea id-ului cartii la url */
    strcat(url, id_book);

    /* Realizarea cererii GET */
    request = compute_get_request("34.246.184.49:8080", url, NULL, NULL, 0, tokens, 0);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);

    /* Verificarea raspunsului */
    if (strstr(message, "OK") != NULL) {
        char *tmp = strtok(message, "{");
        tmp = strtok(NULL, "\n{");
        char str[strlen(tmp) + 2];
        str[0] = '{';
        strcpy(&str[1], tmp);
        str[strlen(tmp) + 1] = '\0';
        /* Extrage cartea in format JSON */
        JSON_Value *val = json_parse_string(str);
        char *book = json_serialize_to_string_pretty(val);
        printf("%s\n\n", book);
        free(book);
    } else if (strstr(message, "error") != NULL) {
        char *tmp = strtok(message, "{");
        tmp = strtok(NULL, "\n{");
        tmp += 9;
        tmp[strlen(tmp) - 2] = '\0';

        char str[strlen(tmp)];
        strcpy(str, tmp);
        str[strlen(tmp) + 1] = '\0';

        printf("ERROR: %s\n", str);
    } else {
        printf("ERROR!\n");
    }

    /* Eliberarea resurselor */
    free(request);
    free(message);
}

/* Functia ADD_BOOK */
void add_book_command(int sockfd, char url[]) {

    char title[LEN];
    char author[LEN];
    char genre[LEN];
    char publisher[LEN];
    char page_count[LEN];

    /* Verificare daca user-ul are acces la biblioteca */
    if (gained_access == 0) {
        printf("ERROR: Access the library first!\n");
        return;
    }

    printf("title=");
    fgets(title, LEN, stdin);
    title[strlen(title) - 1] = '\0';

    printf("author=");
    fgets(author, LEN, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    fgets(genre, LEN, stdin);
    genre[strlen(genre) - 1] = '\0';

    printf("page_count=");
    fgets(page_count, LEN, stdin);
    page_count[strlen(page_count) - 1] = '\0';

    printf("publisher=");
    fgets(publisher, LEN, stdin);
    publisher[strlen(publisher) - 1] = '\0';

    /* Verificare daca numarul de pagini este valid */
    if (verify_if_details_are_ok(page_count) == 1) {
        return;
    }

    /* Crearea unei carti in format JSON */
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    json_object_set_string(object, "title", title);
    json_object_set_string(object, "author", author);
    json_object_set_string(object, "genre", genre);
    json_object_set_number(object, "page_count", atoi(page_count));
    json_object_set_string(object, "publisher", publisher);

    char *book_data = json_serialize_to_string_pretty(value);
    char *data[] = {book_data};

    /* Realizarea cererii POST */
    request = compute_post_request("34.246.184.49:8080", url, "application/json", data, 1, NULL, NULL, tokens);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);

    /* Verificarea raspunsului */
    if (strstr(message, "ok") != NULL) {
        printf("SUCCESS: Book %s from %s added successfully!\n", title, author);
    } else if (strstr(message, "error") != NULL) {
        char *error = strstr(message, "error");
        error[strlen(error) - 1] = '\0';
        error = error + 8;
        error[strlen(error) - 1] = '\0';
        printf("ERRROR: %s\n", error);
    } else {
        printf("ERROR!\n");
    
    }

    /* Eliberarea resurselor */
    free(book_data);
    json_value_free(value);
    free(request);
    free(message);
}

/* Functia DELETE_BOOK */
void delete_book_command(int sockfd, char url[]) {

    /* Verificare daca user-ul are acces la biblioteca */
    if (gained_access == 0) {
        printf("ERROR: Access the library first!\n");
        return;
    }

    char id_book[LEN];
    printf("id=");
    fgets(id_book, LEN, stdin);
    id_book[strlen(id_book) - 1] = '\0';

    /* Verificare daca id-ul cartii este valid */
    int id_book_is_ok = verify_if_id_book_is_valid(id_book);
    if (id_book_is_ok == 1) {
        printf("ERROR: Invalid book id!\n");
        return;
    }

    /* Concatenarea id-ului cartii la url */
    strcat(url, id_book);

    /* Realizarea cererii DELETE */
    request = compute_get_request("34.246.184.49:8080", url, NULL, NULL, 0, tokens, 1);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);

    /* Verificarea raspunsului */
    if (strstr(message, "ok") != NULL) {
        printf("SUCCESS: Book deleted successfully!\n");
    } else if (strstr(message, "error") != NULL) {
        char *tmp = strtok(message, "{");
        tmp = strtok(NULL, "\n{");
        tmp += 9;
        tmp[strlen(tmp) - 2] = '\0';
        char str[strlen(tmp)];
        strcpy(str, tmp);
        str[strlen(tmp) + 1] = '\0';
        printf("ERROR: %s\n", str);
    } else {
        printf("ERROR!\n");
    }

    /* Eliberarea resurselor */
    free(request);
    free(message);
}

/* Functia LOGOUT */
void logout_command(int sockfd, char url[]) {

    /* Verificare daca user-ul este logat */
    if (is_logged_in == 0) {
        printf("ERROR: Log in before logging out!\n");
        return;
    }

    char *cookies_data[] = {cookies};
    /* Realizarea cererii GET */
    request = compute_get_request("34.246.184.49:8080", url, NULL, cookies_data, 1, NULL, 0);
    /* Trimiterea cererii catre server */
    send_to_server(sockfd, request);
    /* Primirea mesajului de la server */
    message = receive_from_server(sockfd);
    
    /* Actualizarea variabilelor care demonstreaza cu un user este 
    logat si are acces la biblioteca */
    is_logged_in = 0;
    gained_access = 0;

    /*Eliberarea cookie-urilor si a token-urilor*/
    memset(tokens, 0, sizeof(tokens));
    memset(cookies, 0, sizeof(cookies));

    /* Verificarea raspunsului */
    if (strstr(message, "ok") != NULL) {
        printf("SUCCESS: Logged out successfully!\n");
        return;
    } else if (strstr(message, "error") != NULL) {
        char *tmp = strtok(message, "{");
        tmp = strtok(NULL, "\n{");
        tmp += 9;
        tmp[strlen(tmp) - 2] = '\0';
        char str[strlen(tmp)];
        strcpy(str, tmp);
        str[strlen(tmp) + 1] = '\0';
        printf("ERROR: %s\n", str);
    } else {
        printf("ERROR!\n");
    }

    /* Eliberarea resurselor */
    free(request);
    free(message);
}


int main(int argc, char *argv[])
{
    int sockfd;

    char current_command[COMMAND_LEN];
    char url[URL_LEN];

    while (1) {

        fgets(current_command, COMMAND_LEN, stdin);
        current_command[strlen(current_command) - 1] = '\0';

        if (strcmp(current_command, REGISTER_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/auth/register");
            register_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, LOGIN_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/auth/login");
            login_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, ENTER_LIBRARY_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/library/access");
            enter_library_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, GET_BOOKS_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/library/books");
            get_books_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, GET_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/library/books/");
            get_book_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, ADD_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/library/books");
            add_book_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, DELETE_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/library/books/");
            delete_book_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, LOGOUT_COMMAND) == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            strcpy(url, "/api/v1/tema/auth/logout");
            logout_command(sockfd, url);
            close_connection(sockfd);
        } else if (strcmp(current_command, EXIT_COMMAND) == 0) {
            break;
        } else {
            printf("INVALID COMMAND!\n");
        }
    }

    return 0;
}
