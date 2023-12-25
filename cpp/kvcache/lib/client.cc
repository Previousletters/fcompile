#include <cstdint>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <map>
#include "model.h"
using namespace std;


class RPCClient {
typedef int (*exe_t)(RPCClient*, string);
  public:
    static Module mod;
    RPCClient(int server) {
        this->server = server;
        command["rmod"] = run_model;
        command["load"] = load_model;
        command["iptr"] = set_inputs;
        command["optr"] = set_outputs;
        memset(r_buffer,0,sizeof(r_buffer));
        memset(s_buffer,0,sizeof(s_buffer));
    }
    ~RPCClient() { close(this->server); }

    void wrap_recv() {
        memset(r_buffer,0,sizeof(r_buffer));
        if (recv(server, r_buffer, sizeof(r_buffer), 0)<=0) {
            perror("receive"); 
            exit(-1);
        }
    }

    void wrap_send() {
        if (send(server, s_buffer, sizeof(s_buffer), 0)<=0) {
            perror("send"); 
            exit(-1);
        }
        memset(s_buffer,0,sizeof(s_buffer));
    }

    int main_loop() {
        wrap_recv();
        char* message = r_buffer;
        cout << message << endl;
        if (isEqual("command", message)) {
            message += 7;
            string name;
            string attrs;
            name.assign(message, 4);
            message += 4;
            if (isEqual("attrs", message)) {
                message += 5;
                int size = atoi(string().assign(message, 4).c_str());
                message += 4;
                attrs.assign(message, size);
                message += size;
            } else {
                attrs = "";
            }
            exe_t func;
            if ((func = command[name]) != nullptr) {
                int ret_state = func(this, attrs);
                sprintf(s_buffer, "command%sret%d", name.c_str(), ret_state);
                cout << "response:" << s_buffer << endl;
            } else {
                sprintf(s_buffer, "not found command function, please retry!");
            }
        } else {
            if (isEqual("quit", message)) {
                return 0; 
            }
            sprintf(s_buffer, "not rpc command, please retry!");
        }
        wrap_send();
        return 1; 
    }

    static int load_model(RPCClient* client, string attrs) {
        return mod.load_model(attrs);
    }

    static int run_model(RPCClient* client, string attrs) {
        return mod.run();
    }

    // %04d%04d%04d, index, times, last
    static int set_inputs(RPCClient* client, string attrs) {
        char* attrs_ptr = const_cast<char*>(attrs.c_str());
        int index = atoi(string().assign(attrs_ptr, 4).c_str()); attrs_ptr += 4;
        int times = atoi(string().assign(attrs_ptr, 4).c_str()); attrs_ptr += 4;
        int dlast = atoi(string().assign(attrs_ptr, 4).c_str());
        uint8_t* ptr = (uint8_t*)mod.input_ptr(index);
        if (ptr == nullptr)
            return 0;
        sprintf(client->s_buffer, "commandloadret1");
        client->wrap_send();
        for (int n = 0; n < times - 1; n++) {
            client->wrap_recv();
            memcpy(ptr, client->r_buffer, 1024);
            ptr += 1024;
            sprintf(client->s_buffer, "commandloadret1");
            client->wrap_send();
        }
        client->wrap_recv();
        memcpy(ptr, client->r_buffer, dlast);
        for (int i = 0; i < dlast/2; i++) {
            cout << ((int16_t*)ptr)[i] << " ";
        }
        cout << endl;
        return 1;
    }

    // %04d%04d%04d, index, times, last
    static int set_outputs(RPCClient* client, string attrs) {
        char* attrs_ptr = const_cast<char*>(attrs.c_str());
        int index = atoi(string().assign(attrs_ptr, 4).c_str()); attrs_ptr += 4;
        int times = atoi(string().assign(attrs_ptr, 4).c_str()); attrs_ptr += 4;
        int dlast = atoi(string().assign(attrs_ptr, 4).c_str());
        uint8_t* ptr = (uint8_t*)mod.output_ptr(index);
        for (int n = 0; n < times - 1; n++) {
            memcpy(client->s_buffer, ptr, 1024);
            client->wrap_send();
            ptr += 1024;
        }
        cout << "output: " << index << ", " << times << ", " << dlast << endl;
        memcpy(client->s_buffer, ptr, dlast);
        client->wrap_send();
        return 1;
    }
  private:
    bool isEqual(const string& target, char* source) {
        for (int i = 0; i < target.size(); i++) {
            if (target[i] != source[i]) {
                return false;
            }
        }
        return true;
    }
    char r_buffer[1024];
    char s_buffer[1024];
    int server;
    map<string, exe_t> command;
};

Module RPCClient::mod;

int main(int argc,char *argv[])
{
  int sockfd;
  if ((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) { 
      perror("socket"); 
      return -1; 
  }
 
  struct sockaddr_in servaddr;
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(8000); // server's port
  servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");//server's ip
  if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0) { 
      perror("connect"); 
      close(sockfd); 
      return -1; 
  }

  RPCClient client(sockfd);
  while(client.main_loop()) {};
  return 0;
}

