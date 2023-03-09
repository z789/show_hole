#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd = -1;
	off_t b_pos, e_pos, d_pos;
	struct stat st;
	int count = 0;
	int ret = -1;

	if (argc != 2) {
		fprintf(stdout, "Usage: %s filename\n", argv[0]);
		goto end;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("fstat");
		goto end;
	}
	if (fstat(fd, &st) < 0) {
		perror("fstat");
		goto end;
	}

	fprintf(stdout, "file %s size %ld st_blksize:%ld st_blocks:%ld\n",
			argv[1], st.st_size, st.st_blksize, st.st_blocks);
	b_pos = 0;
	while (b_pos < st.st_size) { 
		e_pos = lseek(fd, b_pos, SEEK_HOLE); 
		if (e_pos < 0 || e_pos >= st.st_size)
			break;

		d_pos = lseek(fd, e_pos, SEEK_DATA); 
		if (d_pos < 0)
			d_pos = st.st_size;

		if (count == 0) {
			fprintf(stdout, "hole:\n[%ld-%ld)", e_pos, d_pos);
		} else if ((count+1) % 10) {
			fprintf(stdout, " [%ld-%ld)", e_pos, d_pos);
		} else {
			fprintf(stdout, " [%ld-%ld)\n", e_pos, d_pos);
		}
		count++;
		b_pos = d_pos;
	}
	if (count % 10)
		fprintf(stdout, "\n");

	ret = 0;
end:
	if (fd >= 0)
		close(fd);
	return ret;
}
