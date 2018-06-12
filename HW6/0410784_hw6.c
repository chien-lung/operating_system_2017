#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h> 
#include <string.h>
#include <math.h>
int inode_in=0, filename_in=0, size_min_in=0, size_max_in=0;

void search(char* path_name, char* filename, long unsigned int inode, double size_min, double size_max);
int main(int argc, char *argv[]) {
	char path_name[100];
	char filename[50];
	long unsigned int inode;
	double size_min, size_max;

	strcpy(path_name, argv[1]);
	for(int i=2;i<argc;i+=2){
		if( strcmp(argv[i],"-name") == 0){
			strcpy(filename,argv[i+1]);
			filename_in = 1;
		}
		else if(strcmp(argv[i],"-inode") == 0){
			sscanf(argv[i+1],"%lu",&inode);
			inode_in = 1;
		}
		else if( strcmp(argv[i],"-size_min") == 0){
			sscanf(argv[i+1],"%lf",&size_min);
			size_min_in = 1;
		}
		else if( strcmp(argv[i],"-size_max") == 0){
			sscanf(argv[i+1],"%lf",&size_max);
			size_max_in = 1;
		}
	}

	search(path_name,filename,inode,size_min,size_max);

	return(0);
}
void search(char* path_name, char* filename, long unsigned int inode, double size_min, double size_max){
	DIR *d;
	struct dirent *dir;
	struct stat buf; 
	d = opendir(path_name);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if( strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ){
				char file_path_name[1000];
				strcpy(file_path_name,path_name);
				strcat(file_path_name,"/");
				strcat(file_path_name,dir->d_name);

				stat(file_path_name,&buf);
				double size_MB = buf.st_size/(pow(2.,20.));
				int file_type = dir->d_type; // 4:directory / 8: txt, c, md ...
				
				if(file_type == 4)
					search(file_path_name, filename, inode, size_min, size_max);

				if(filename_in == 1)
					if(strcmp(dir->d_name,filename) != 0)
						continue;

				if(inode_in == 1)
					if(inode != buf.st_ino)
						continue;
					
				if(size_min_in == 1)
					if(size_MB < size_min)
						continue;
					
				if(size_max_in == 1)
					if(size_MB > size_max)
						continue;
				
				printf("%s %lu %lf MB\n", file_path_name, buf.st_ino, size_MB);

			}
		}
		closedir(d);
	}
}
