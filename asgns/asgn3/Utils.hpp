#ifndef UTILS_H_
#define UTILS_H_

template<typename T>
class CircularQueue{
    private:
        int front, rear;
        int size;
        T *arr;
    public:
        CircularQueue(int size){
            this->front = -1;
            this->rear = -1;
            this->size = size;
            this->arr = new T[size];
        }
        T getFront(){
            return arr[front];
        }
        T getRear(){
            return arr[rear];
        }
        /* Function to create Circular queue */
        void enQueue(T value) 
        { 
            if ((front == 0 && rear == size-1) || 
                    (rear == (front-1)%(size-1))) 
            { 
                printf("\nQueue is Full"); 
                return; 
            } 
        
            else if (front == -1) /* Insert First Element */
            { 
                front = rear = 0; 
                arr[rear] = value; 
            } 
        
            else if (rear == size-1 && front != 0) 
            { 
                rear = 0; 
                arr[rear] = value; 
            } 
        
            else
            { 
                rear++; 
                arr[rear] = value; 
            } 
        } 
        
        // Function to delete element from Circular Queue 
        T deQueue() 
        { 
            if (front == -1) 
            { 
                printf("\nQueue is Empty"); 
                return INT_MIN;  // todo
            } 
        
            T data = arr[front]; 
            arr[front] = -1; 
            if (front == rear) 
            { 
                front = -1; 
                rear = -1; 
            } 
            else if (front == size-1) 
                front = 0; 
            else
                front++; 
        
            return data; 
        } 
};

/*
template<typename T>
class SelectiveReject{
    private:
        CircularQueue <T> window;
    public:
        SelectiveReject(int windowSize)
            : window(windowSize){}
        
        CircularQueue <T> &getWindow(){
            return this->window;
        }


};
*/


#endif