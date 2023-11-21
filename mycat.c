#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFF_SIZE 16

int main(int argc, char* argv[])
{
	int ifd, ofd;
	char* buff;
	ssize_t nreads, nwrites;

	if (argc < 2) {
		fprintf(stdout, "인수가 없습니다.\n");
		return 0;
	} else if (argc == 2) {
		// 모니터에 출력
		ifd = open(argv[1], O_RDONLY);
		if (ifd == -1) {
			fprintf(stderr, "입력 파일을 열 수 없습니다: %s\n", strerror(errno));
			return -1;
		}

		buff = (char*) malloc(sizeof(char) * BUFF_SIZE);
		if (!buff) {
			fprintf(stderr, "메모리 할당에 실패했습니다.\n");
			close(ifd);
			return -1;
		}

		while ((nreads = read(ifd, buff, BUFF_SIZE)) > 0) {
			nwrites = write(STDOUT_FILENO, buff, nreads);
			if (nwrites == -1) {
				fprintf(stderr, "모니터로 출력하는 도중 오류가 발생했습니다: %s\n", strerror(errno));
				close(ifd);
				free(buff);
				return -1;
			}
		}

		close(ifd);
		free(buff);
	} else if (argc == 4 && !strcmp(argv[2], "into")) {
		// 파일로 복사
		ifd = open(argv[1], O_RDONLY);
		if (ifd == -1) {
			fprintf(stderr, "입력 파일을 열 수 없습니다: %s\n", strerror(errno));
			return -1;
		}

		ofd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (ofd == -1) {
			fprintf(stderr, "출력 파일을 열 수 없습니다: %s\n", strerror(errno));
			close(ifd);
			return -1;
		}

		buff = (char*) malloc(sizeof(char) * BUFF_SIZE);
		if (!buff) {
			fprintf(stderr, "메모리 할당에 실패했습니다.\n");
			close(ifd);
			close(ofd);
			return -1;
		}

		while ((nreads = read(ifd, buff, BUFF_SIZE)) > 0) {
			nwrites = write(ofd, buff, nreads);
			if (nwrites == -1) {
				fprintf(stderr, "파일로 복사하는 도중 오류가 발생했습니다: %s\n", strerror(errno));
				close(ifd);
				close(ofd);
				free(buff);
				return -1;
			}
		}

		close(ifd);
		close(ofd);
		free(buff);
	} else {
		fprintf(stdout, "잘못된 인수입니다.\n");
		return 0;
	}

	return 0;
}
