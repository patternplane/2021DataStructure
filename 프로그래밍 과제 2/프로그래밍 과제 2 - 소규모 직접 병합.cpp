
/**
* @studentId
* @author �鼺��
*/

#include <stdio.h>
#include <stdlib.h>

void* malloc_s(size_t size);
void* realloc_s(void* block, size_t size);
void get_user_input(char** buffer, int* size);
const char* get_runFileName( int number);
void make_new_run(double* data, int data_len,int run_number);
void remove_runs(int start_num, int run_count);
int compare(const void* a, const void* b);
void merge_runs(int merge_size, int run_count);

int main() {

	FILE* inputFile;
	char* inputFileName = NULL;
	int inputFileName_len = 0;
	printf("������ ������ �Է��ϼ��� : ");
	while (true) {
		get_user_input(&inputFileName, &inputFileName_len);
		fopen_s(&inputFile, inputFileName, "r");
		if (inputFile != NULL)
			break;
		printf("�׷� ������ �����ϴ�. ����(Ȥ�� ���)���� Ȯ�����ּ���\n");
	}
	free(inputFileName);
	printf("\n");

	printf("������ �����Ͽ� �����մϴ�.\n");

	double M[200];
	int M_item_count = 0;
	int run_count = 0;
	while (fscanf_s(inputFile, "%lf", &M[M_item_count]) != EOF) {
		M_item_count++;
		if (M_item_count == 200) {
			qsort(M,sizeof(M)/sizeof(M[0]),sizeof(M[0]), compare);
			make_new_run(M, M_item_count , run_count++);

			if (run_count == INT_MAX) {
				fprintf(stderr, "���� ������ ������ �ʹ� �����ϴ�! run_%d.txt���Ͽ��� ������ �ߴ��մϴ�.\n", run_count);
				exit(1);
			}
			M_item_count = 0;
		}
	}
	if (M_item_count != 0) {
		qsort(M, M_item_count, sizeof(M[0]), compare);
		make_new_run(M, M_item_count, run_count++);
	}

	printf("������ ���� ������ �Ϸ�Ǿ����ϴ�.\n");
	printf("\n");
	if (run_count == 0)
		return 0;

	printf("���ҵ� ���ϵ��� �����մϴ�.\n");

	merge_runs(200, run_count);

	printf("���� ������ �Ϸ�Ǿ����ϴ�.\n");
	printf("\n");
	
	fclose(inputFile);
	// �� ���� ���ǿ� �´��� Ȯ���ؾߵ�.

	printf("���ҵ� ���ϵ��� ����ϴ�.\n");
	remove_runs(1, run_count);

	return 0;
}

void* malloc_s(size_t size) {
	void* new_mem = (void*)malloc(size);
	if (new_mem == NULL) {
		fprintf(stderr,"Cannot allocate memory\n");
		exit(1);
	}
	
	return new_mem;
}

void* realloc_s(void* block, size_t size) {
	void* re_mem = (void*)realloc(block, size);
	if (re_mem == NULL) {
		fprintf(stderr, "Cannot allocate memory\n");
		exit(1);
	}

	return re_mem;
}

void get_user_input(char** buffer, int* size) {
	
	int done_count = 0;
	if (*size == 0) {
		*size += 50;
		*buffer = (char*)malloc_s((*size) * sizeof(char));
	}

	int newChar;
	while ((newChar = getchar()) != '\n' && newChar != EOF) {
		(*buffer)[done_count++] = newChar;
		
		if (*size <= done_count) {
			*size += 50;
			*buffer = (char*)realloc_s(*buffer, (*size) * sizeof(char));
		}
	}
	(*buffer)[done_count] = '\0';
}

char runName[50];
const char* get_runFileName(int number) {

	sprintf_s(runName, 50, "run_%d.txt", number);

	return (const char*)runName;
}

void make_new_run(double* data, int data_len, int run_number) {
	
	FILE* file = NULL;
	errno_t errcode = fopen_s(&file, get_runFileName(run_number), "w");
	if (errcode || file == NULL) {
		fprintf(stderr, "���� ������ �����ϴ� �� ������ �߻��߽��ϴ�. (�����ڵ� %d)\n",errcode);
		exit(1);
	}

	for (int i = 0; i < data_len; i++)
		fprintf(file, "%f\n", data[i]);

	fclose(file);

}

void remove_runs(int start_num, int run_count) {

	const char* fileName;
	for (int i = start_num; i < run_count; i++) {
		fileName = get_runFileName(i);
		if (remove(fileName))
			printf("\"%s\"���� ���� ����\n",fileName);
	}

}

int compare(const void* a, const void* b) {
	if (*(double*)a > *(double*)b)
		return 1;
	else if (*(double*)a == *(double*)b)
		return 0;
	else // a < b
		return -1;
}

#define FIRST_INDEX_OF_LEAFS(leafNodesCount) (leafNodesCount)
#define GET_INDEX_OF_SMALL(indexA, indexB, Array) (((indexA) == -1)? (indexB) : (((indexB) == -1)?(indexA):((Array[indexA] > Array[indexB])?(indexB):(indexA))))
#define GET_INDEX_OF_BIG(indexA, indexB, Array) (((indexA) == -1)? (indexA) : (((indexB) == -1)?(indexB):((Array[indexA] > Array[indexB])?(indexA):(indexB))))

int* indexLST;

int* winnerIndexQueue;
int WIQ_size;
int WIQ_front, WIQ_rear;

double* leaf_item;
int leaf_item_count;

FILE** merging_files;

void* make_LST(size_t dataSize, int leafNodesCount) {

	int nonRootNodes_count = 2 * leafNodesCount - 1;

	return (void*)malloc_s((nonRootNodes_count + 1) * dataSize);

}

void WIQ_add(int item) {
	WIQ_rear = (WIQ_rear + 1) % WIQ_size;
	winnerIndexQueue[WIQ_rear] = item;
}
int WIQ_delete() {
	if (WIQ_front == WIQ_rear)
		return -1;

	WIQ_front = (WIQ_front + 1) % WIQ_size;
	return winnerIndexQueue[WIQ_front];
}
int WIQ_len() {
	if (WIQ_front <= WIQ_rear)
		return WIQ_rear - WIQ_front;
	else
		return WIQ_size - WIQ_front + WIQ_rear;
}

void merge_runs_r(int merge_size, int run_count,int start_number) {

	if (merge_size < run_count) {

		int merged_count = 0;
		for (; merged_count < run_count / merge_size; merged_count++) {
			merge_runs_r(merge_size, merge_size, start_number + merged_count * merge_size);
		}
		if (run_count % merge_size) {
			merge_runs_r(merge_size, run_count % merge_size, start_number + merged_count * merge_size);
			merged_count++;
		}

		merge_runs_r(merge_size, merged_count, 0);
	}

	else {

		FILE* mergedFile = NULL;
		fopen_s(&mergedFile, "output_writing.txt", "w");
		if (mergedFile == NULL) {
			fprintf(stderr, "���� ������ �����ϴ� �� ������ �߻��߽��ϴ�.\n");
			exit(1);
		}

		leaf_item_count = ((run_count < merge_size)?run_count:merge_size);
		WIQ_front = 0;
		WIQ_rear = 0;

		int i;
		for (i = 0; i < leaf_item_count;i++) {
			fopen_s(&merging_files[i], get_runFileName(start_number + i), "r");
			if (merging_files[i] == NULL) {
				fprintf(stderr, "���� �۾��� ���������� �սǵ�.\n");
				exit(1);
			}
			fscanf_s(merging_files[i],"%lf", &leaf_item[i]);
			
			indexLST[FIRST_INDEX_OF_LEAFS(leaf_item_count) + i] = i;
			WIQ_add((leaf_item_count - 1) - i);
		}

		int queue_len, repeat;
		int index1, index2;
		int current_index = FIRST_INDEX_OF_LEAFS(leaf_item_count) - 1;
		while ((queue_len = WIQ_len()) != 1) {
			repeat = queue_len / 2;
			for (int i = 0; i < repeat; i++) {
				index1 = WIQ_delete();
				index2 = WIQ_delete();
				WIQ_add(GET_INDEX_OF_SMALL(index1, index2, leaf_item));
				indexLST[current_index--] = GET_INDEX_OF_BIG(index1, index2, leaf_item);
			}
		}
		indexLST[current_index] = WIQ_delete();

		int LST_index;
		int winner_index;
		int done_count = 0;
		while (true) {
			fprintf(mergedFile,"%f\n",leaf_item[indexLST[0]]);

			LST_index = FIRST_INDEX_OF_LEAFS(leaf_item_count) + indexLST[0];
			if (fscanf_s(merging_files[indexLST[0]], "%lf",&leaf_item[indexLST[0]]) == -1) {
				done_count++;
				if (done_count == leaf_item_count)
					break;

				indexLST[LST_index] = winner_index = -1;
			}
			else
				indexLST[LST_index] = winner_index = indexLST[0];

			LST_index /= 2;
			for (int index1, index2; LST_index > 0; LST_index /= 2) {
				index1 = indexLST[LST_index];
				index2 = winner_index;
				indexLST[LST_index] = GET_INDEX_OF_BIG(index1, index2, leaf_item);
				winner_index = GET_INDEX_OF_SMALL(index1, index2, leaf_item);
			}
			indexLST[LST_index] = winner_index;
		}
		
		const char* name = get_runFileName(start_number / merge_size);
		for (i = 0; i < leaf_item_count; i++) 
			fclose(merging_files[i]);
		fclose(mergedFile);
		remove(name);
		rename("output_writing.txt", name);
	}
}

void merge_runs(int merge_size, int run_count) {

	merging_files = (FILE**)malloc_s(merge_size * sizeof(FILE*));
	leaf_item = (double*)malloc_s(merge_size * sizeof(double));
	indexLST = (int*)make_LST(sizeof(int), merge_size+1);
	WIQ_size = merge_size + 1;
	winnerIndexQueue = (int*)malloc_s(WIQ_size * sizeof(int));

	merge_runs_r(merge_size, run_count, 0);

	remove("output.txt");
	if (rename("run_0.txt", "output.txt")) {
		fprintf(stderr, "�۾����� ���Ͽ� ������ �� ���� : run_0.txt.\n");
		exit(1);
	}

	free(leaf_item);
	free(merging_files);
	free(indexLST);
	free(winnerIndexQueue);
}