#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

/*
 * Header
 */
#define HEADER_SIZE	2048
#define HEADER_MAGIC	"UNITY FILE V1.10 / RICOH COMPANY"

struct fhdr {
	uint32_t	bstart;
	uint32_t	len;
	char		name[11];
	uint8_t		u2;
	uint32_t	u3[3];
	uint32_t	u4[8];
};

struct hdr {
    char	magic[32];
    uint32_t	len;
    uint32_t	dircount;
    uint32_t	unknown_1[6];
    struct fhdr dir[31];
};

/*
 * Data block
 */
#define BLOCK_SIZE		512
#define BLOCK_KEY		0xF8E69612
#define BLOCK_KEY_DIFF		0xBE39B193
#define BLOCK_KEY_NEXT(k)	((k)+BLOCK_KEY_DIFF)

static void
xor_block(uint32_t *b, uint32_t k)
{
	int i;

	for (i=0; i<BLOCK_SIZE/sizeof(uint32_t); i++)
		b[i] ^= ntohl(k);
}

int
main(int argc, char *argv[])
{
	struct hdr h;
	char filename[128];
	uint8_t buffer[BLOCK_SIZE];
	FILE *in, *out;
	uint32_t k;
	int i, len, bsize, do_xor;

	if (argc != 2) {
		fprintf(stderr, "ricohdec <firmware>\n");
		return EXIT_FAILURE;
	}

	in = fopen(argv[1], "r");
	if (in == NULL) {
		fprintf(stderr, "cannot open file <%s>\n", argv[1]);
		return EXIT_FAILURE;
	}

	while (1)
	{
		if (fread(&h, sizeof(h), 1, in) != 1) {
			if (!feof(in))
				perror("read error\n");
			break;
		}

		if (memcmp(h.magic, HEADER_MAGIC, sizeof(HEADER_MAGIC)-1) != 0) {
			fprintf(stderr, "invalid magic\n");
			break;
		}
		h.len = ntohl(h.len);
		h.dircount = ntohl(h.dircount);
		fprintf(stdout, "filelen=%d dircount=%d\n", h.len, h.dircount);
		for (i = 0; i < h.dircount; i++) {
			h.dir[i].len = ntohl(h.dir[i].len);
			h.dir[i].bstart = ntohl(h.dir[i].bstart);
			fseek(in, h.dir[i].bstart * BLOCK_SIZE, SEEK_SET);
			snprintf(filename, sizeof(filename), "%.10s.bin", h.dir[i].name);
			out = fopen(filename, "w");
			if (out == NULL) {
				fprintf(stderr, "cannot open file <%s>\n", filename);
				return EXIT_FAILURE;
			}

			k = BLOCK_KEY;
			for (len = h.dir[i].len; len > 0; len -= bsize) {
				bsize = min(BLOCK_SIZE, len);
				if (fread(buffer, bsize, 1, in) != 1) {
					perror("read error\n");
					return EXIT_FAILURE;
				}
				if (len == h.dir[i].len) {
					do_xor = memcmp(buffer, HEADER_MAGIC, sizeof(HEADER_MAGIC)-1) != 0;
					fprintf(stdout, "flen=%d filename=%s mode=%s\n", 
					    h.dir[i].len, filename, do_xor ? "encrypted" : "container");
				}
				if (do_xor)
					xor_block((uint32_t*)buffer, k);
				if (fwrite(buffer, bsize, 1, out) != 1) {
				        perror("write error\n");
					return EXIT_FAILURE;
				}
				k = BLOCK_KEY_NEXT(k);
			}
			fclose(out);
		}
	}
	fclose(in);

	return EXIT_SUCCESS;
}
