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
	printf("type\tprocess id\t priority\tcomputing time\n");
	if (!is_empty(queue))
	{
		int i = queue->front;
		do {
			i = (i + 1) % MAX_QUEUE_SIZE;
			printf("  %d\t     %d\t            %d\t              %d\n", \
				queue->process[i]->type, queue->process[i]->process_id, \
				queue->process[i]->priority, queue->process[i]->computing_time);
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
		queue->process[queue->rear]->arrival_time = 0;

		if (feof(fp))
		{
			printf("-1이 입력되지 않았습니다.\n");
			printf("입력 프로세스 구성을 마치려면 가장 마지막 줄에 -1을 입력하세요.\n");
			exit(1);
		}
	}
	print_queue(queue);

}

void FCFS_sort(queue_type* queue)
{
	unsigned int arrival_time;
	process_type* tmp;
	if (!is_empty(queue))
	{
		for (int i = (queue->front + 1) % MAX_QUEUE_SIZE; i != queue->rear; i = (i + 1) % MAX_QUEUE_SIZE)
		{
			arrival_time = queue->process[i]->arrival_time;
			for (int j = (i + 1) % MAX_QUEUE_SIZE; j != (queue->rear + 1) % MAX_QUEUE_SIZE; j = (j + 1) % MAX_QUEUE_SIZE)
			{
				if (arrival_time > queue->process[j]->arrival_time)
				{
					tmp = queue->process[i];
					queue->process[i] = queue->process[j];
					queue->process[j] = tmp;
					arrival_time = queue->process[i]->arrival_time;
				}

			}
		}
	}
}
void start_FCFS(queue_type* input_queue, queue_type* ready_queue, queue_type* finish_queue)
{
	unsigned int tick = 0;
	queue_type* tmp_queue = (queue_type*)malloc(sizeof(queue_type));
	int tmp;
	int process_num = 0;
	double normalized_att = 0;
	process_type* p;

	printf("\nFCFS 스케줄링을 시작합니다.\n");

	printf("\n시간 할당량이 다 된 프로세스는 완료 큐로 이동하고, 입력 큐는 도착시간 순으로 정렬합니다.\n");

	while (!is_empty(input_queue))
	{
		if (input_queue->process[(input_queue->front + 1) % MAX_QUEUE_SIZE]->type == 1)
			enqueue(finish_queue, dequeue(input_queue));
		else
			enqueue(tmp_queue, dequeue(input_queue));
	}
	while (!is_empty(tmp_queue))
	{
		enqueue(input_queue, dequeue(tmp_queue));
	}
	free(tmp_queue);

	FCFS_sort(input_queue);
	printf("\n입력 큐 출력\n");
	print_queue(input_queue);
	printf("\n완료 큐 출력\n");
	print_queue(finish_queue);
	printf("\n프로세스 수행을 시작합니다.\n");
	printf("\n프로그램 출력\n");
	printf("process id\t priority\tcomputing time\t\tturn around time\n");
	while ((!is_empty(input_queue)) || (!is_empty(ready_queue)))
	{
		if ((!is_empty(input_queue)) && (tick >= input_queue->process[(input_queue->front + 1) % MAX_QUEUE_SIZE]->arrival_time))
		{
			while (1)
			{
				if (tick >= input_queue->process[(input_queue->front + 1) % MAX_QUEUE_SIZE]->arrival_time)
				{
					enqueue(ready_queue, dequeue(input_queue));
					if (is_empty(input_queue))
						break;
				}
				else
					break;
			}
		}
		else if((!is_empty(ready_queue)) && (tick >= ready_queue->process[(ready_queue->front + 1) % MAX_QUEUE_SIZE]->arrival_time))
		{
			p = dequeue(ready_queue);
			process_num++;
			tmp = p->computing_time;
			while (tmp > 0)
			{
				tmp = tmp - 1;
				tick++;
			}
			p->completion_time = tick;
			normalized_att = normalized_att + ((p->completion_time - p->arrival_time) / (double)p->computing_time);

			printf("  %d\t\t     %d\t               %d\t                %d\n", \
				p->process_id, p->priority, p->computing_time, p->completion_time - p->arrival_time);
		}
		else
			tick++;
	}
	normalized_att = normalized_att / process_num;
	printf("\n모든 작업을 마쳤습니다.\n");
	printf("경과 시간 : %d\n", tick);
	printf("프로세스 개수 : %d\n", process_num);
	
	printf("normalized average turnaround time : %f\n", normalized_att);
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

	start_FCFS(&input_queue, &ready_queue, &finish_queue);
	free_memory(&input_queue);
	free_memory(&ready_queue);
	free_memory(&finish_queue);

	return 0;
}
