#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#define NAME_LENGTH 255

static char **file_name;
static int file_count = 0;
static int *file_offset_end;

#define STORE_FILE "/home/toyer/PMVS_4/PMVS4/all_file"

static int path_index(const char* path)
{
	int i  = 0;
	for(i = 0; i < file_count; i++) {
		if(strcmp(file_name[i], path)==0) {
			return i;
		}
	}
	return -1;
}
//get file attributes
static int getattr_callback(const char *path, struct stat *stbuf) 
{
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	} else {
		int index = path_index(path);
		if(index == -1){
			return -ENOENT;
		}
		stbuf->st_mode = S_IFREG | 0777; 
		stbuf->st_nlink = 1; 	
		int start = index == 0 ? 0 : file_offset_end[index-1];
		int size = file_offset_end[index]-start;
		printf("%d\n", size);
		stbuf->st_size = size;
		return 0;
	}
	return -ENOENT;
}
//Read directory
static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) 
{
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	int i;
	for(i = 0; i < file_count; i++) {
		if(strlen(file_name[i])!= 0) {
			filler(buf, file_name[i]+1, NULL, 0);
		}
	}
	return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) 
{
  	int index = path_index(path);
	if(index==-1)
		return -ENOENT;
	return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) 
{
	(void) fi;
	int index = path_index(path);
	FILE *file_in = fopen(STORE_FILE, "rb");
	int start = index == 0 ? 0 : file_offset_end[index-1];
	fseek(file_in, start, SEEK_SET);
	fread(buf, file_offset_end[index] - start, 1,file_in);
	size = file_offset_end[index]-start;
	printf("%d\n",file_offset_end[index]-start);
	printf("%s\n", buf);
	fclose(file_in);
	return size;
}

static struct fuse_operations fuse_example_operations = {
	.getattr = getattr_callback,
	.open = open_callback,
	.read = read_callback,
	.readdir = readdir_callback,
};
int main(int argc, char *argv[])
{
  return fuse_main(argc, argv, &fuse_example_operations, NULL);
}
