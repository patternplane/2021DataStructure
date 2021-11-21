
/**
* @studentId
* @author 백성수
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
	printf("정렬할 파일을 입력하세요 : ");
	while (true) {
		get_user_input(&inputFileName, &inputFileName_len);
		fopen_s(&inputFile, inputFileName, "r");
		if (inputFile != NULL)
			break;
		printf("그런 파일이 없습니다. 파일(혹은 경로)명을 확인해주세요\n");
	}
	free(inputFileName);
	printf("\n");

	printf("파일을 분할하여 정렬합니다.\n");

	double M[200];
	int M_item_count = 0;
	int run_count = 0;
	while (fscanf_s(inputFile, "%lf", &M[M_item_count]) != EOF) {
		M_item_count++;
		if (M_item_count == 200) {
			qsort(M,sizeof(M)/sizeof(M[0]),sizeof(M[0]), compare);
			make_new_run(M, M_item_count , run_count++);

			if (run_count == INT_MAX) {
				fprintf(stderr, "파일 분할의 갯수가 너무 많습니다! run_%d.txt파일에서 분할을 중단합니다.\n", run_count);
				exit(1);
			}
			M_item_count = 0;
		}
	}
	if (M_item_count != 0) {
		qsort(M, M_item_count, sizeof(M[0]), compare);
		make_new_run(M, M_item_count, run_count++);
	}

	printf("파일의 분할 정렬이 완료되었습니다.\n");
	printf("\n");
	if (run_count == 0)
		return 0;

	printf("분할된 파일들을 병합합니다.\n");

	merge_runs(200, run_count);

	printf("파일 병합이 완료되었습니다.\n");
	printf("\n");
	
	fclose(inputFile);
	// ■ 과제 조건에 맞는지 확인해야돼.

	printf("분할된 파일들을 지웁니다.\n");
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
		fprintf(stderr, "분할 파일을 생성하는 중 오류가 발생했습니다. (에러코드 %d)\n",errcode);
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
			printf("\"%s\"파일 삭제 실패\n",fileName);
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

// 1. n개씩 병합
// 2. 개별 파일들을 다시 병합
int treeHeight;
unsigned int run_start_pos;
int* selectTree_looser;
int* winners;
double* items;
int items_len;
FILE** merging_files;
void merge_runs_r(int merge_size, int run_count,int start_number) {

	if (merge_size < run_count) {
		int merged_count = 0;
		int new_run_count = run_count/ merge_size;
		for (; merged_count < new_run_count; merged_count++) {
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
			fprintf(stderr, "분할 파일을 생성하는 중 오류가 발생했습니다.\n");
			exit(1);
		}

		items_len = ((run_count < merge_size)?run_count:merge_size);

		int i;
		for (i = 0; i < items_len; i++) {
			fopen_s(&merging_files[i], get_runFileName(start_number + i), "r");
			if (merging_files[i] == NULL) {
				fprintf(stderr, "작업중 분할파일이 손실됨.\n");
				exit(1);
			}
			fscanf_s(merging_files[i],"%lf\n", &items[i]);

			if (i % 2) {
				selectTree_looser[(run_start_pos + i) / 2] = ((items[i-1] >= items[i]) ? (i-1) : i );
				winners[i/2] = ((items[i - 1] < items[i]) ? (i - 1) : i);
			}
		}
		if (i % 2) {
			selectTree_looser[(run_start_pos + (i - 1)) / 2] = (i - 1);
			winners[i / 2] = ((items[i - 1] < items[i]) ? (i - 1) : i);
			i++;
		}
		for (; i < run_start_pos; i += 2)
			selectTree_looser[(run_start_pos + i) / 2] = -1;

		int current_start_pos = run_start_pos / 2;
		int current_number;
		for (; current_start_pos > 1; current_start_pos /= 2) {
			for (current_number = 0; current_number < current_start_pos; current_number+=2) {
				if (selectTree_looser[current_start_pos + current_number + 1] == -1) {
					selectTree_looser[(current_start_pos + current_number) / 2] = -1;
					winners[current_number / 2] = winners[ current_number];
				}

				else if (items[winners[current_number]] > items[winners[current_number + 1]]) {
					selectTree_looser[(current_start_pos + current_number) / 2] = winners[current_number];
					winners[current_number / 2] = winners[current_number + 1];
				}

				else {
					selectTree_looser[(current_start_pos + current_number) / 2] = winners[current_number+1];
					winners[current_number / 2] = winners[current_number];
				}
			}
		}
		selectTree_looser[0] = winners[0];

		int start_index;
		int winner;
		int done_count = 0;
		while (true) {
			fprintf(mergedFile,"%f\n",items[selectTree_looser[0]]);

			if (fscanf_s(merging_files[selectTree_looser[0]], "%lf",&items[selectTree_looser[0]]) == -1) {
				done_count++;
				if (done_count == items_len)
					break;

				winner = -1;
			}
			else
				winner = selectTree_looser[0];

			start_index = (run_start_pos + selectTree_looser[0]) / 2;
			for (int tmp; start_index >= 1; start_index = start_index >> 1)
				if (selectTree_looser[start_index] != -1) {
					if (winner == -1) {
						winner = selectTree_looser[start_index];
						selectTree_looser[start_index] = -1;
					}
					else if (items[selectTree_looser[start_index]] < items[winner]) {
						tmp = selectTree_looser[start_index];
						selectTree_looser[start_index] = winner;
						winner = tmp;
					}
				}
			selectTree_looser[0] = winner;
		}
		
		const char* name = get_runFileName(start_number / merge_size);
		for (i = 0; i < items_len; i++) 
			fclose(merging_files[i]);
		fclose(mergedFile);
		remove(name);
		rename("output_writing.txt", name);
	}
}

void merge_runs(int merge_size, int run_count) {

	treeHeight = 1;
	run_start_pos = 1; // 마지막 레벨의 노드 수와 동일한 값
	for (; run_start_pos < merge_size; treeHeight++, run_start_pos = run_start_pos << 1);
	selectTree_looser = (int*)malloc_s((run_start_pos) * sizeof(int));
	winners = (int*)malloc_s((run_start_pos>>1) * sizeof(int));
	items = (double*)malloc_s((run_start_pos<<1) * sizeof(double));
	merging_files = (FILE**)malloc_s(merge_size*sizeof(FILE*));

	merge_runs_r(merge_size, run_count, 0);

	remove("output.txt");
	if (rename("run_0.txt", "output.txt")) {
		fprintf(stderr, "작업중인 파일에 접근할 수 없음 : run_0.txt.\n");
		exit(1);
	}

	free(selectTree_looser);
	free(winners);
	free(items);
	free(merging_files);
}