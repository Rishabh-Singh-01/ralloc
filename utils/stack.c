/*#include <stdio.h>*/
/*#include <stdlib.h>*/
/**/
/*// Define the structure for a stack node*/
/*typedef struct Node {*/
/*    void *data;         // Pointer to store any type of data*/
/*    struct Node *next;  // Pointer to the next node in the stack*/
/*} Node;*/
/**/
/*// Define the stack structure*/
/*typedef struct {*/
/*    Node *top;          // Pointer to the top of the stack*/
/*} Stack;*/
/**/
/*// Function to initialize an empty stack*/
/*Stack* createStack() {*/
/*    Stack *stack = (Stack*)malloc(sizeof(Stack));*/
/*    if (stack == NULL) {*/
/*        perror("Memory allocation failed for stack");*/
/*        exit(EXIT_FAILURE);*/
/*    }*/
/*    stack->top = NULL;*/
/*    return stack;*/
/*}*/
/**/
/*// Function to push a pointer onto the stack*/
/*void push(Stack *stack, void *ptr) {*/
/*    Node *newNode = (Node*)malloc(sizeof(Node));*/
/*    if (newNode == NULL) {*/
/*        perror("Memory allocation failed for new node");*/
/*        exit(EXIT_FAILURE);*/
/*    }*/
/*    newNode->data = ptr;*/
/*    newNode->next = stack->top;*/
/*    stack->top = newNode;*/
/*}*/
/**/
/*// Function to pop a pointer from the stack*/
/*void* pop(Stack *stack) {*/
/*    if (stack->top == NULL) {*/
/*        fprintf(stderr, "Stack Underflow: Cannot pop from an empty
 * stack.\n");*/
/*        return NULL;*/
/*    }*/
/*    Node *temp = stack->top;*/
/*    void *poppedData = temp->data;*/
/*    stack->top = temp->next;*/
/*    free(temp);*/
/*    return poppedData;*/
/*}*/
/**/
/*// Function to peek at the top pointer without removing it*/
/*void* peek(Stack *stack) {*/
/*    if (stack->top == NULL) {*/
/*        fprintf(stderr, "Stack is empty: Cannot peek.\n");*/
/*        return NULL;*/
/*    }*/
/*    return stack->top->data;*/
/*}*/
/**/
/*// Function to check if the stack is empty*/
/*int isEmpty(Stack *stack) {*/
/*    return (stack->top == NULL);*/
/*}*/
/**/
/*// Function to free the memory allocated for the stack*/
/*void destroyStack(Stack *stack) {*/
/*    while (!isEmpty(stack)) {*/
/*        pop(stack); // Pop and free each node*/
/*    }*/
/*    free(stack);*/
/*}*/

#include <stdio.h>
#include <stdlib.h> // For exit()

#define MAX_SIZE 100 // Define the maximum size of the stack

typedef struct {
  int arr[MAX_SIZE];
  int top; // Index of the top element
} Stack;

// Function to initialize the stack
void initStack(Stack *s) {
  s->top = -1; // -1 indicates an empty stack
}

// Function to check if the stack is full
int isFull(Stack *s) { return s->top == MAX_SIZE - 1; }

// Function to check if the stack is empty
int isEmpty(Stack *s) { return s->top == -1; }

// Function to push an element onto the stack
void push(Stack *s, int value) {
  if (isFull(s)) {
    return;
  }
  s->arr[++(s->top)] = value; // Increment top and then add the value
}

// Function to pop an element from the stack
int pop(Stack *s) {
  if (isEmpty(s)) {
    // In a real application, you might handle this more gracefully,
    // e.g., by returning a special error value or using a flag.
    exit(EXIT_FAILURE);
  }
  return s->arr[(s->top)--]; // Return top element and then decrement top
}

// Function to peek at the top element without removing it
int peek(Stack *s) {
  if (isEmpty(s)) {
    exit(EXIT_FAILURE);
  }
  return s->arr[s->top];
}

// Function to display the elements of the stack
void display(Stack *s) {
  if (isEmpty(s)) {
    printf("Stack is empty.\n");
    return;
  }
  printf("Stack elements (top to bottom):\n");
  for (int i = s->top; i >= 0; i--) {
    printf("%d\n", s->arr[i]);
  }
}
