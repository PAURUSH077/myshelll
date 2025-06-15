#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <cstring>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fstream>

using namespace std;

#define HISTORY_FILE "/home/paurush/.myshell_history"

void loadHistory() {
    read_history(HISTORY_FILE);
}

void saveHistory() {
    write_history(HISTORY_FILE);
}

vector<string> tokenize(const string& input) {
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token) tokens.push_back(token);
    return tokens;
}

bool isBuiltIn(const string& cmd) {
    return cmd == "cd" || cmd == "clear" || cmd == "history";
}

void runBuiltIn(const vector<string>& tokens, const vector<string>& history) {
    if (tokens[0] == "cd") {
        if (tokens.size() < 2) cerr << "cd: missing operand\n";
        else if (chdir(tokens[1].c_str()) != 0) perror("cd failed");
    } else if (tokens[0] == "clear") {
        system("clear");
    } else if (tokens[0] == "history") {
        HIST_ENTRY** histList = history_list();
        if (histList) {
            for (int i = 0; histList[i]; i++)
                cout << i + 1 << ": " << histList[i]->line << '\n';
        }
    }
}

vector<char*> vecToCharArray(const vector<string>& tokens) {
    vector<char*> args;
    for (const string& s : tokens) {
        char* arg = new char[s.size() + 1];
        strcpy(arg, s.c_str());
        args.push_back(arg);
    }
    args.push_back(nullptr);
    return args;
}

void executeSimpleCommand(vector<string> tokens) {
    vector<char*> args = vecToCharArray(tokens);
    if (execvp(args[0], args.data()) == -1)
        perror("Command failed");
    for (char* arg : args) delete[] arg;
    exit(1);
}

void handleRedirection(vector<string>& tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == ">") {
            int fd = open(tokens[i + 1].c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            break;
        } else if (tokens[i] == "<") {
            int fd = open(tokens[i + 1].c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            break;
        }
    }
}

void executePipedCommands(const string& leftCmd, const string& rightCmd) {
    int pipefd[2];
    pipe(pipefd);
    pid_t pid1 = fork();

    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        vector<string> leftTokens = tokenize(leftCmd);
        handleRedirection(leftTokens);
        executeSimpleCommand(leftTokens);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);

        vector<string> rightTokens = tokenize(rightCmd);
        handleRedirection(rightTokens);
        executeSimpleCommand(rightTokens);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);
}

int main() {
    loadHistory();
    string input;

    while (true) {
        char* line = readline("MyShell> ");
        if (!line) break;
        input = string(line);
        free(line);

        if (input.empty()) continue;
        if (input == "exit") break;

        add_history(input.c_str());

        if (input.find('|') != string::npos) {
            size_t pipePos = input.find('|');
            string leftCmd = input.substr(0, pipePos);
            string rightCmd = input.substr(pipePos + 1);
            executePipedCommands(leftCmd, rightCmd);
            continue;
        }

        vector<string> tokens = tokenize(input);
        if (isBuiltIn(tokens[0])) {
            runBuiltIn(tokens, tokens); // reusing tokens as history source
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            handleRedirection(tokens);
            executeSimpleCommand(tokens);
        } else {
            wait(nullptr);
        }
    }

    saveHistory();
    cout << "Exiting MyShell. Goodbye!" << endl;
    return 0;
}
