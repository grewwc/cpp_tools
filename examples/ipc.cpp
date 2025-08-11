#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

#define KEY 0x12341

int write() {
    int id = shmget(KEY, 1024, IPC_CREAT | 0666);
    if (id == -1) {
        perror("shmget error: ");
        return 1;
    } else {
        cout << "key id: " << id << endl;
    }
    cout << id << endl;
    void* addr = shmat(id, NULL, SHM_W);
    if (addr == (void*)-1) {
        perror("shmat error");
        return 1;
    }
    cout << addr << endl;
    cout << "write: ";
    cin.getline((char*)addr, 1023);
    if (shmdt(addr) != 0) {
        perror("shmdt error");
        return 1;
    }
    return 0;
}

void read(bool destroy = false) {
    int id = shmget(KEY, 1024, 0444);
    char* data = (char*)shmat(id, nullptr, IPC_R);
    cout << data << endl;
    if (destroy && shmctl(id, IPC_RMID, nullptr) != 0) {
        perror("shmctl error");
        return;
    }
}

int main() {
}