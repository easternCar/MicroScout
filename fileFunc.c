//중요도를 계산하는 알고리즘을 구현한 코드

#include "fileFunc.h"


void getFileScores(int count);
void getFileWeights(int* arr);
void getFileStats(FileStats* files, int count);



// 파일의 개수를 받아, 임시 dat 파일에 기록된 파일 정보들을 사용해
// 중요도 값 수치들을 얻어낸다.
void getFileScores(int count)
{
	FileStats* files = (FileStats*)malloc(sizeof(FileStats)*count);	//파일의 정보들이 담기는 구조체 배열
	int* weights;				//중요도 가중치를 저장할 정수 배열
	int idx;

	weights = (int*)malloc(sizeof(int)*FILE_CATEGORY);

	//파일 정보를 files 구조체 배열에 기록
	getFileStats(files, count);

	eval1_stat(files, count, SECTOR1);		//권한값을 이용해 점수 계산
	eval2_blks(files, count, SECTOR2);		//블록과 용량으로 점수 계산
	eval3_hardlinks(files, count, SECTOR3);		//하드 링크 개수로 점수 계산
	eval4_access_time(files, count, SECTOR4);	//접근 시간으로 점수 계산
	eval5_etc(files, count, SECTOR5);		//소유자, 디바이스 장치 정보를 이용해 점수 계산



	//--------------------테스트용 출력 코드
	for(idx = 0; idx < count; idx++)
	{
		printf("%s : %f %f %f %f %f\n", (files + idx)->name, (files + idx)->scores[0], (files + idx)->scores[1], (files + idx)->scores[2], (files + idx)->scores[3], (files + idx)->scores[4]);
	}

	//---------------가중치를 읽어오는 함수
	getFileWeights(weights);


	free(files);
	
}

//filestats.dat에 기록되어 있는 파일의 정보들을 얻어낸다.
void getFileStats(FileStats* files, int count)
{
	FILE* tmpF;				//정보를 불러올 임시 파일
	char* tempFname = "filestats.dat";	//정보를 불러올 임시 파일의 이름
	char tempName[MAXIMUM_PATH_LEN];	//잠깐 파일명을 저장할 곳
	int idx;

	tmpF = fopen(tempFname, "r");

	//파일과 그 정보들을 모두 구조체 배열에 저장한다.
	for(idx = 0; idx < count; idx++)
	{
		fscanf(tmpF, "%s %d %o %d %d %d %d %d %d %u\n", tempName, &((files + idx)->devNode), &((files + idx)->mode), 
 &((files + idx)->hardlinks), &((files + idx)->uid), &((files + idx)->gid), &((files + idx)->size),
&((files + idx)->blksize),&((files + idx)->blocks),&((files + idx)->atime));
		strcpy((files + idx)->name, tempName);
	}



	fclose(tmpF);

	//정보를 모두 옮겼으니 기록 파일을 삭제한다.
	if(remove(tempFname) == -1) perror("remove");

}

//일단 모든 가중치는 파일 weight.db에 저장되고, 여기의 값을 모두 temp에 옮긴 뒤, 프로세스 중요도 탐색 작업에 필요한 가중치들만을 골라서 arr에 가져오도록 한다.
//
//arr :		작업에 필요한 가중치 배열이다. 이는 그 작업에서 선언, 할당되어서 사용된다.
//			예를 들어 '파일 중요도 탐색' 작업을 수행한다면 그에 필요한 가중치들을 가져간다.
//			작업마다 필요한 가중치 개수는 위에서 상수로 지정되어있다.
void getFileWeights(int* arr)
{
	char* fname = "weight.db";		//가중치 값을 저장하는 파일
	FILE *f;
	int i;
	int temp[MAX_CATEGORY]; 		//임시 정수 배열로, 최대 100개까지의 값을 저장
		

	if((f = fopen(fname, "r")) == NULL)	//파일이 없는 경우 디폴트 값 생성.
	{
		f = fopen(fname, "w");
		fprintf(f, "%d %d %d %d %d ", 18, 19, 20, 21, 22);
		fprintf(f, "%d %d %d %d %d ", 16, 18, 20, 22, 24);
		fclose(f);
	}
	
	
	//프로세스 중요도 심사항목이 5개, 파일 중요도 심사항목이 5개라고 가정하면
	//데이터 파일에 저장된 가중치는 10개뿐이다.
	for(i = 0; i < PROC_CATEGORY + FILE_CATEGORY; i++)
	{
		fscanf(f, "%d", &temp[i]);
	}

	//이제 매개변수로 들어온 배열 arr에 가중치 값들을 집어넣는다.
	//파일의 가중치에 해당되는 값들을 불러온다.
	for(i = 0; i < FILE_CATEGORY; i++)
	{
		*(arr + i) = temp[FILE_OFFSET + i];
	}
		

	fclose(f);
}

