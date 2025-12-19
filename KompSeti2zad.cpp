#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>


using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define SRV_PORT 1234


struct Emp {
    char name[50];
    int projects;
    int overtime;
    int efficiency;
    int initiatives;
} E;

string calcBonus() {
    int score = E.projects * 2 + E.overtime + E.efficiency * 3 + E.initiatives * 2;

    if (score < 10) return "NO_BONUS";
    else if (score < 20) return "STANDARD_BONUS";
    else if (score < 30) return "MEDIUM_BONUS";
    else return "HIGH_BONUS";
}

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsa;
    SOCKET servSock, cliSock;
    sockaddr_in servAddr, cliAddr;
    int cliSize = sizeof(cliAddr);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    servSock = socket(AF_INET, SOCK_STREAM, 0);

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(SRV_PORT);

    bind(servSock, (sockaddr*)&servAddr, sizeof(servAddr));
    listen(servSock, SOMAXCONN);

    cout << "TCP сервер запущен... Ожидание данных" << endl;

    while (true) {
        cliSock = accept(servSock, (sockaddr*)&cliAddr, &cliSize);

        char ip[INET_ADDRSTRLEN];
        InetNtopA(AF_INET, &(cliAddr.sin_addr), ip, INET_ADDRSTRLEN);
        cout << "Клиент подключен: " << ip << endl;

        while (true) {
            int received = recv(cliSock, (char*)&E, sizeof(E), 0);

            if (received <= 0) {
                cout << "Клиент отключился" << endl;
                break;
            }

            if (string(E.name) == "exit") {
                cout << "Клиент вышел" << endl;
                break;
            }

            cout << "Данные: " << E.name << endl;
            cout << "Проекты: " << E.projects
                << ", Сверхурочные: " << E.overtime
                << ", Эффективность: " << E.efficiency
                << ", Инициативы: " << E.initiatives << endl;

            string bonus = calcBonus();
            cout << "Код премии: " << bonus << endl;

            send(cliSock, bonus.c_str(), bonus.size() + 1, 0);
        }

        closesocket(cliSock);
        cout << "Соединение закрыто" << endl;
    }

    closesocket(servSock);
    WSACleanup();
    return 0;
}


client

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>


using namespace std;

#pragma comment(lib, "ws2_32.lib")


#define SRV_HOST "127.0.0.1"
#define SRV_PORT 1234
#define BUF_SIZE 64

struct Emp {
    char name[50];
    int projects;
    int overtime;
    int efficiency;
    int initiatives;
} E;

string getBonusText(const string& code) {
    if (code == "NO_BONUS") return "Премия не назначена";
    else if (code == "STANDARD_BONUS") return "Стандартная премия (10% от оклада)";
    else if (code == "MEDIUM_BONUS") return "Средняя премия (25% от оклада)";
    else if (code == "HIGH_BONUS") return "Высокая премия (50% от оклада)";
    else return "Неизвестный код";
}

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsa;
    SOCKET sock;
    sockaddr_in servAddr;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SRV_PORT);
    InetPtonA(AF_INET, SRV_HOST, &servAddr.sin_addr);

    if (connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == 0) {
        cout << "Подключение к серверу установлено!" << endl;
        cout << "Вводите данные или 'exit' для выхода" << endl;

        while (true) {
            cout << "\n--- Новый сотрудник ---" << endl;
            cout << "Имя (или 'exit'): ";
            cin.getline(E.name, 50);

            if (string(E.name) == "exit") {
                send(sock, (char*)&E, sizeof(E), 0);
                cout << "Выход..." << endl;
                break;
            }

            cout << "Завершенные проекты: ";
            cin >> E.projects;

            cout << "Сверхурочные часы: ";
            cin >> E.overtime;

            cout << "Эффективность (1-5): ";
            cin >> E.efficiency;

            cout << "Инициативы: ";
            cin >> E.initiatives;

            cin.ignore();

            send(sock, (char*)&E, sizeof(E), 0);
            cout << "Данные отправлены" << endl;

            char bonusCode[BUF_SIZE];
            int received = recv(sock, bonusCode, sizeof(bonusCode), 0);

            if (received > 0) {
                string code(bonusCode);
                string bonusText = getBonusText(code);

                cout << "\n>>> РЕЗУЛЬТАТ <<<" << endl;
                cout << "Сотрудник: " << E.name << endl;
                cout << "Премия: " << bonusText << endl;
                cout << "----------------" << endl;
            }
        }
    }
    else {
        cout << "Ошибка подключения!" << endl;
    }

    closesocket(sock);
    WSACleanup();
return;
}





    2вариант udp
        #include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <locale>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define SRV_PORT 1234


struct Emp {
    char name[50];
    int projects;
    int overtime;
    int efficiency;
    int initiatives;
} E;

string calcBonus() {
    int score = E.projects * 2 + E.overtime + E.efficiency * 3 + E.initiatives * 2;

    if (score < 10) return "NO_BONUS";
    else if (score < 20) return "STANDARD_BONUS";
    else if (score < 30) return "MEDIUM_BONUS";
    else return "HIGH_BONUS";
}

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsa;
    SOCKET sock;
    sockaddr_in servAddr, cliAddr;
    int cliSize = sizeof(cliAddr);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(SRV_PORT);

    bind(sock, (sockaddr*)&servAddr, sizeof(servAddr));

    cout << "UDP сервер запущен..." << endl;

    while (true) {
        int received = recvfrom(sock, (char*)&E, sizeof(E), 0,
            (sockaddr*)&cliAddr, &cliSize);

        if (received > 0) {
            if (string(E.name) == "exit") {
                cout << "Клиент вышел" << endl;
                continue;
            }

            char ip[INET_ADDRSTRLEN];
            InetNtopA(AF_INET, &(cliAddr.sin_addr), ip, INET_ADDRSTRLEN);

            cout << "От " << ip << ": " << E.name << endl;
            cout << "Проекты: " << E.projects
                << ", Сверхурочные: " << E.overtime
                << ", Эффективность: " << E.efficiency
                << ", Инициативы: " << E.initiatives << endl;

            string bonus = calcBonus();
            cout << "Премия: " << bonus << endl;

            sendto(sock, bonus.c_str(), bonus.size() + 1, 0,
                (sockaddr*)&cliAddr, cliSize);
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
client
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <locale>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define SRV_HOST "127.0.0.1"
#define SRV_PORT 1234
#define BUF_SIZE 64

struct Emp {
    char name[50];
    int projects;
    int overtime;
    int efficiency;
    int initiatives;
} E;

string getBonusText(const string& code) {
    if (code == "NO_BONUS") return "Премия не назначена";
    else if (code == "STANDARD_BONUS") return "Стандартная премия (10% от оклада)";
    else if (code == "MEDIUM_BONUS") return "Средняя премия (25% от оклада)";
    else if (code == "HIGH_BONUS") return "Высокая премия (50% от оклада)";
    else return "Неизвестный код";
}

int main() {
    setlocale(LC_ALL, "Russian");

    WSADATA wsa;
    SOCKET sock;
    sockaddr_in servAddr;
    int addrSize = sizeof(servAddr);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SRV_PORT);
    InetPtonA(AF_INET, SRV_HOST, &servAddr.sin_addr);

    cout << "UDP сервер подключен!" << endl;
    cout << "Вводите данные или 'exit' для выхода" << endl;

    while (true) {
        cout << "\n--- Новый сотрудник ---" << endl;
        cout << "Имя (или 'exit'): ";
        cin.getline(E.name, 50);

        if (string(E.name) == "exit") {
            sendto(sock, (char*)&E, sizeof(E), 0,
                (sockaddr*)&servAddr, addrSize);
            cout << "Выход..." << endl;
            break;
        }

        cout << "Завершенные проекты: ";
        cin >> E.projects;

        cout << "Сверхурочные часы: ";
        cin >> E.overtime;

        cout << "Эффективность (1-5): ";
        cin >> E.efficiency;

        cout << "Инициативы: ";
        cin >> E.initiatives;

        cin.ignore();

        sendto(sock, (char*)&E, sizeof(E), 0,
            (sockaddr*)&servAddr, addrSize);

        cout << "Данные отправлены" << endl;

        char bonusCode[50];
        int received = recvfrom(sock, bonusCode, sizeof(bonusCode), 0,
            (sockaddr*)&servAddr, &addrSize);

        if (received > 0) {
            string code(bonusCode);
            string bonusText = getBonusText(code);

            cout << "\n>>> РЕЗУЛЬТАТ <<<" << endl;
            cout << "Сотрудник: " << E.name << endl;
            cout << "Премия: " << bonusText << endl;
            cout << "----------------" << endl;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

 
