
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
void remove_runs(int run_number);
int compare(const void* a, const void* b);

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
	printf("\n");

	printf("파일을 분할하여 정렬합니다.\n");

	double M[200];
	int M_item_count = 0;
	int run_count = 0;
	while (fscanf_s(inputFile, "%lf", &M[M_item_count]) != EOF) {
		M_item_count++;
		if (M_item_count == 200) {
			qsort(M,sizeof(M)/sizeof(M[0]),sizeof(M[0]), compare);
			make_new_run(M, M_item_count , run_count);

			run_count++;
			if (run_count == INT_MAX) {
				fprintf(stderr, "파일 분할의 갯수가 너무 많습니다! run_%d.txt파일에서 분할을 중단합니다.\n", run_count);
				exit(1);
			}
			M_item_count = 0;
		}
	}
	if (M_item_count != 0) {
		make_new_run(M, M_item_count, run_count);

		run_count++;
	}

	printf("파일의 분할 정렬이 완료되었습니다.\n");
	printf("\n");

	printf("분할된 파일들을 병합합니다.\n");

	printf("파일 병합이 완료되었습니다.\n");
	printf("\n");
	

	fclose(inputFile);

	// ■ 파일 또 닫아야돼.
	// ■ 동적할당 해제해야돼.
	// ■ 과제 조건에 맞는지 확인해야돼.

	printf("분할된 파일들을 지웁니다.\n");
	remove_runs(run_count);

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

void remove_runs(int run_number) {

	const char* fileName;
	for (int i = 0; i < run_number; i++) {
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