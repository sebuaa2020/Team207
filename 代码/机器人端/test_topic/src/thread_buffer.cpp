#include <c++/5/queue>
#include <c++/5/iostream>
#include <pthread.h>
#include <semaphore.h>

class Thread_buffer {
    private:
        std::queue<std::string>   send_buffer;
        std::queue<std::string>   recv_buffer;
        pthread_mutex_t send_lock;
        pthread_mutex_t recv_lock;
        sem_t send_sem;
        sem_t recv_sem;

    public:
        Thread_buffer() {
            sem_init(&send_sem, 0, 0);
            sem_init(&recv_sem, 0, 0);
        }

        ~Thread_buffer() {
            while (!recv_buffer.empty()) {
                recv_buffer.pop();
            }
            while (!send_buffer.empty()) {
                send_buffer.pop();
            }
        }

        void set_send(std::string msg) {
            pthread_mutex_lock(&send_lock);
                send_buffer.push(msg);
                std::cout<<"TB: push send: "<<msg<<std::endl;
                sem_post(&send_sem);
            pthread_mutex_unlock(&send_lock);
        }

        void get_send(std::string *msg) {
            sem_wait(&send_sem);
            pthread_mutex_lock(&send_lock);
                *msg = send_buffer.front();
                std::cout<<"TB: pop send: "<<*msg<<std::endl;
                send_buffer.pop();
            pthread_mutex_unlock(&send_lock);
        }

        void set_recv(std::string msg) {
            pthread_mutex_lock(&recv_lock);
                recv_buffer.push(msg);
                sem_post(&recv_sem);
            pthread_mutex_unlock(&recv_lock);
        }

        void get_recv(std::string *msg) {
            sem_wait(&recv_sem);
            pthread_mutex_lock(&recv_lock);
                *msg = recv_buffer.front();
                recv_buffer.pop();
            pthread_mutex_unlock(&recv_lock);
        }

        void clear_buff() {
            while (!recv_buffer.empty()) {
                recv_buffer.pop();
            }
            while (!send_buffer.empty()) {
                send_buffer.pop();
            }
            sem_init(&send_sem, 0, 0);
            sem_init(&recv_sem, 0, 0);
        }

};