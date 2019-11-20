#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <string>
#include <initializer_list>

// Declaraciones
std::string join(std::initializer_list<std::string> vec, char sep);
std::vector<std::string> split(std::string str, char sep);
std::string intWithZeros(int x, int lendigits); // rellena zeros a la izquierda
std::string packageField(std::string field, int lendigits);
std::string arrcharToString(char* arr, int sz); // Permite '\0' en str
char getch(); // Recibe entrada teclado sin "Enter"
std::vector<std::string> tokenizeCommand(std::string s); // Util en parsear comandos de usuario

// Definiciones
std::string join(std::initializer_list<std::string> vec, char sep=' ')
{
    std::string ans;
    for(std::string s: vec)
      ans += s + sep;
    ans.pop_back();
    return ans;
}

std::vector<std::string> split(std::string str, char sep=' ')
{
    std::vector<std::string> ans;
    std::string buf;
    for(char c: str)
        if(c == sep)
        {
            ans.push_back(buf);
            buf.clear();
        }
        else 
            buf += c;
    ans.push_back(buf);
    return ans;
}

// 011
std::string intWithZeros(int x, int lendigits) // rellena zeros a la izquierda
{
    int cnt = std::to_string(x).length();
    std::string zeros(lendigits-cnt, '0');
    return zeros + std::to_string(x);
}

// 011 mensaje1234
std::string packageField(std::string field, int lendigits)
{
    std::string fieldcnt = intWithZeros(field.size(), lendigits);
    return fieldcnt + " " + field;
}



std::string arrcharToString(char* arr, int sz) // Permite '\0' en str
{
    std::string str(sz, '\0');
    memcpy((char*)str.c_str(), arr, sz);
    return str;
}

char getch() 
{
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
    perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror ("tcsetattr ~ICANON");
    return (buf);
}

// Token: cadena de texto sin espacio(como std::cin) | cadena entre comillas("")
// No soporta: "MSG "tarea: ejemplos usando comillas "" " "  
// Command puede ser: "GET test.txt" | "MSG destinatario1 "Hey, como estas!!""
// Packages puede ser: "1 08 test.txt" | "MSG "

std::vector<std::string> tokenizeCommand(std::string s)
{
    std::vector<std::string> tokens;
    std::string tok;
    bool usingCom = false; // Usa comillas
    for(size_t i=0; i<s.size(); ++i)
    {
        if(!usingCom) // Caso std::cin 
        {
            if(s[i] != ' ')
            {
                if(s[i] == '"') usingCom = true;
                else            tok.push_back(s[i]);
            }
            else
            {
                if(!tok.empty()) 
                {
                    tokens.push_back(tok);
                    tok.clear();
                }
            }
        }
        else // Caso ""
        {
            if(s[i] == '"') 
            {
                usingCom = false;
                tokens.push_back(tok);
                tok.clear();
            }
            else
            {    
                tok.push_back(s[i]);
            }           
        }
    }
    tokens.push_back(tok);
    return tokens;
}


#endif