#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE	100

typedef struct {
	int type;
	int process_id;
	int priority;
	unsigned int computing_time;
	unsigned int arrival_time;
	unsigned int completion_time;
}process_type;

typedef struct {
	process_type* process[MAX_QUEUE_SIZE];
	int front, rear;
}queue_type;

void init_queue(queue_type* queue)
{
	queue->front = 0;
	queue->rear = 0;
}

int is_full(queue_type* queue)
{
	return ((queue->rear + 1) % MAX_QUEUE_SIZE == queue->front);
}

int is_empty(queue_type* queue)
{
	return (queue->front == queue->rear);
}

void enqueue(queue_type* queue, process_type* process)
{
	if (is_full(queue))
	{
		printf("큐가 포화상태입니다.");
		return;
	}
	queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
	queue->process[queue->rear] = process;
}

process_type* dequeue(queue_type* queue)
{
	if (is_empty(queue))
	{
		printf("큐가 공백상태입니다.");
		return NULL;
	}
	queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
	return queue->process[queue->front];

}

void print_queue(queue_type* queue)
{
	printf("type\tprocess id\t priority\tcomputing time\t\tarrival time\n");
	if (!is_empty(queue))
	{
		int i = queue->front;
		do {
			i = (i + 1) % MAX_QUEUE_SIZE;
			printf("  %d\t     %d\t            %d\t              %d \t 	     %d\n", \
				queue->process[i]->type, queue->process[i]->process_id, \
				queue->process[i]->priority, queue->process[i]->computing_time, \
				queue->process[i]->arrival_time);
			if (i == queue->rear)
				break;
		} while (i != queue->front);

	}

}

void input_process(queue_type* queue)
{
	int tmp;
	printf("입력 프로세스 구성\n");
	FILE* fp;
	fp = fopen("process.txt", "r");
	while (1)
	{
		if (is_full(queue))
		{
			printf("큐가 포화상태입니다");
			return;
		}
		tmp = queue->rear;
		queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
		process_type* new = (process_type*)malloc(sizeof(process_type));
		queue->process[queue->rear] = new;
		fscanf(fp, "%d", &queue->process[queue->rear]->type);
		if (queue->process[queue->rear]->type == -1)
		{
			free(queue->process[queue->rear]);
			queue->rear = tmp;
			break;
		}
		fscanf(fp, "%d", &queue->process[queue->rear]->process_id);
		fscanf(fp, "%d", &queue->process[queue->rear]->priority);
		fscanf(fp, "%d", &queue->process[queue->rear]->computing_time);
		fscanf(fp, "%d", &queue->process[queue->rear]->arrival_time);

		if (feof(fp))
		{
			printf("-1이 입력되지 않았습니다.\n");
			printf("입력 프로세스 구성을 마치려면 가장 마지막 줄에 -1을 입력하세요.\n");
			exit(1);
		}
	}
	print_queue(queue);

}	

void arrival_time_ascending_order_sort(queue_type* queue)
{
	unsigned int tmp_arrival_time;
	process_type* tmp;
	if (!is_empty(queue))
	{
		for (int i = (queue->front + 1) % MAX_QUEUE_SIZE; i != queue->rear; i = (i + 1) % MAX_QUEUE_SIZE)
		{
			tmp_arrival_time = queue->process[i]->arrival_time;
			for (int j = (i + 1) % MAX_QUEUE_SIZE; j != (queue->rear + 1) % MAX_QUEUE_SIZE; j = (j + 1) % MAX_QUEUE_SIZE)
			{
				if (tmp_arrival_time > queue->process[j]->arrival_time)
				{
					tmp = queue->process[i];
					queue->process[i] = queue->process[j];
					queue->process[j] = tmp;
					tmp_arrival_time = queue->process[i]->arrival_time;
				}

			}
		}
	}

}

void SPN_sort(queue_type* queue)
{
	unsigned int process_time;
	process_type* tmp;
	if (!is_empty(queue))
	{
		for (int i = (queue->front + 1) % MAX_QUEUE_SIZE; i != queue->rear; i = (i + 1) % MAX_QUEUE_SIZE)
		{
			process_time = queue->process[i]->computing_time;
			for (int j = (i + 1) % MAX_QUEUE_SIZE; j != (queue->rear + 1) % MAX_QUEUE_SIZE; j = (j + 1) % MAX_QUEUE_SIZE)
			{
				if (process_time > queue->process[j]->computing_time)
				{
					tmp = queue->process[i];
					queue->process[i] = queue->process[j];
					queue->process[j] = tmp;
					process_time = queue->process[i]->computing_time;
				}

			}
		}
	}
}
void start_SPN(queue_type* input_queue, queue_type* ready_queue, queue_type* finish_queue)
{
	unsigned int tick = 0;
	process_type* p;
	printf("\nSPN 스케줄링을 시작합니다.\n");
	arrival_time_ascending_order_sort(input_queue);
	printf("입력 큐 도착 시간 순 정렬\n");
	print_queue(input_queue);
	//완료 큐와 준비 큐에 나누어 삽입
	while (!is_empty(input_queue))
	{
		if (input_queue->process[input_queue->front + 1]->type == 1)
			enqueue(finish_queue, dequeue(input_queue));
		else
			enqueue(ready_queue, dequeue(input_queue));
	}
	printf("준비 큐 출력\n");
	print_queue(ready_queue);
	//SPN_sort(input_queue);
	printf("완료 큐 출력\n");
	print_queue(finish_queue);
	printf("프로세스 수행을 시작합니다.\n");
	while (!is_empty(ready_queue))
	{
		if (tick >= ready_queue->process[ready_queue->front + 1]->arrival_time)
		{
			p = dequeue(ready_queue);
			print_queue(ready_queue);
			printf("ID %d 프로세스를 시작합니다. (도착시간 %d, 소요시간 %d)\n", p->process_id, p->arrival_time, p->computing_time);
			while (p->computing_time > 0)
			{
				p->computing_time = p->computing_time - 1;
				tick++;
			}
			printf("ID %d 프로세스를 마치고 완료 큐에 들어갑니다.\n", p->process_id);
			printf("경과 시간 : %d\n", tick);
			p->type = 1;
			p->process_id = 0;
			p->priority = 0;
			p->computing_time = 0;
			p->arrival_time = 0;
			enqueue(finish_queue, p);
			print_queue(finish_queue);
		}
		else
			tick++;
	}
	printf("모든 작업을 마쳤습니다.\n");
	printf("경과 시간 : %d\n", tick);
}
void free_memory(queue_type* queue)
{
	if (!is_empty(queue))
	{
		int i = queue->front;
		do {
			i = (i + 1) % MAX_QUEUE_SIZE;
			free(queue->process[i]);
			if (i == queue->rear)
				break;
		} while (i != queue->front);
	}
}

int main()
{
	queue_type input_queue, ready_queue, finish_queue;

	init_queue(&input_queue);
	init_queue(&ready_queue);
	init_queue(&finish_queue);

	input_process(&input_queue);

	start_SPN(&input_queue, &ready_queue, &finish_queue);
	free_memory(&input_queue);
	free_memory(&ready_queue);
	free_memory(&finish_queue);

	return 0;
}
