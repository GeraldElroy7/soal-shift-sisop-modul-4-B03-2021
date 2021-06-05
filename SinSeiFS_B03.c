#define FUSE_USE_VERSION 28
#include<stdio.h>
#include<stdbool.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<errno.h>
#include<dirent.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<fuse.h>

char dirpath[50] = "/home/nurputra/Downloads";

void makeLog(int opsi, char* path1, char* path2){
    if(opsi == 1){
            char logdata[1000];
            sprintf(logdata, "RENAME: %s --> %s", path1, path2);
            FILE *result = fopen("/home/nurputra/log", "a");
            fprintf(result, "%s\n", logdata);
            fclose(result);
    }else if(opsi == 2){
            char logdata[1000];
            sprintf(logdata, "MKDIR: %s", path2);
            FILE *result = fopen("/home/nurputra/log", "a");
            fprintf(result, "%s\n", logdata);
            fclose(result);
    }

}

void makeLogWarning(char *txt, char* path){
    char this_tm[30];
    char* inlev = "WARNING";
    char log[1000];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(this_tm, 30, "%d%m%Y-%H:%M:%S", p1);
    sprintf(log, "%s::%s::%s::%s", inlev, this_tm, txt, path);
	FILE *result = fopen("/home/nurputra/SinSeiFS.log", "a");
    fprintf(result, "%s\n", log);
    fclose(result);
}

void makeLogInfo(int opsi, char *txt, char* path, char* path2){
    if(opsi == 1){
        char this_tm[30];
        char logdata[1000];
        char* inlev = "INFO";
        time_t t = time(NULL);
        struct tm* p1 = localtime(&t);
        strftime(this_tm, 30, "%d%m%Y-%H:%M:%S", p1);
        sprintf(logdata, "%s::%s::%s::%s", inlev, this_tm, txt, path);
        FILE *result = fopen("/home/nurputra/SinSeiFS.log", "a");
        fprintf(result, "%s\n", logdata);
        fclose(result);
    }
    else if(opsi == 2){
        char this_tm[30];
        char logdata[1000];
        char* inlev = "INFO";
        time_t t = time(NULL);
        struct tm* p1 = localtime(&t);
        strftime(this_tm, 30, "%d%m%Y-%H:%M:%S", p1);
        sprintf(logdata, "%s::%s::%s::%s::%s", inlev, this_tm, txt, path, path2);
        FILE *result = fopen("/home/nurputra/SinSeiFS.log", "a");
        fprintf(result, "%s\n", logdata);
        fclose(result);
    }
}

int id = 0;
char eksten[100000] = "\0";

char key[60] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

char *encode(char* str, bool check){
	char *eksten = strrchr(str, '.');
	int idx, j, k = 0;
	if(check && eksten != NULL) {
            k = strlen(eksten);
	}
	for(idx = 0; idx < strlen(str) - k; idx++){
		for(j = 0; j < 52; j++){
			if(str[idx] == key[j]){
                if(j < 26){
                   str[idx] = key[(25-j)];
                }
                else{
                    str[idx] = key[(51-j) + 26];
                }
				break;
			}
		}
	}
	return str;
}

char *decode(char* str, bool check){
    char *eksten = strrchr(str, '.');
	int idx, j, k = 0;
	if(check && eksten != NULL) {
            k = strlen(eksten);
	}
	for(idx = 0; idx < strlen(str) - k; idx++){
		for(j = 0; j < 52; j++){
			if(str[idx] == key[j]){
                if(j < 26)
                    str[idx] = key[(25-j)];
                else
                    str[idx] = key[(51-j) + 26];
				break;
			}
		}
	}
	return str;
}

void sub_str(char *s, char *sub, int p, int l) {
   int indx = 0;
   while (indx < l){
      sub[indx] = s[p + indx];
      indx++;
   }
   sub[indx] = '\0';
}

int check_eksten(char* checkfile){
	id = 0;
	while(id < strlen(checkfile) && checkfile[id] != '.') id++;
	memset(eksten, 0, sizeof(eksten));
	strcpy(eksten, checkfile + id);
	return id;
}

char *mergePath(char *resmerge, char *str1, const char *str2){
	strcpy(resmerge, str1);
	if(!strcmp(str2, "/")) return resmerge;
	if(str2[0] != '/'){
		resmerge[strlen(resmerge) + 1] = '\0';
		resmerge[strlen(resmerge)] = '/';
	}
	sprintf(resmerge, "%s%s", resmerge, str2);
	return resmerge;
}

char *checkPath(char *str){
	bool encd;
	int first, id;
	encd = 0; first = 1;
	id = strchr(str + first, '/') - str - 1;
	char this_pos[1024];
	while(id < strlen(str)){
		strcpy(this_pos, "");
		strncpy(this_pos, str + first, id - first + 1);
		this_pos[id - first + 1] = '\0';
		if(encd){
			encode(this_pos, 0);
			strncpy(str + first, this_pos, id - first + 1);
		}
		if(!encd && strstr(str + first, "AtoZ_") == str + first) encd = 1;
		first = id + 2;
		id = strchr(str + first, '/') - str - 1;
	}
	id = strlen(str); id--;
	strncpy(this_pos, str + first, id - first + 1);
	this_pos[id - first + 1] = '\0';
	if(encd){
		encode(this_pos, 1);
		strncpy(str + first, this_pos, id - first + 1);
	}
	return str;
}

char *lastName(char *str){
	if(!strcmp(str, "/")) return NULL;
	return strrchr(str, '/') + 1;
}


void joinpart(char *str){
	int id = 0;
	char jpbuf[2048];
	char *eksten = str + strlen(str) - 4;
	if(strcmp(eksten, ".000")) return;
	eksten[0] = '\0';
	FILE *joined;
	joined = fopen(str, "wb");
	while(1){
		char filejoinname[1000000];
		sprintf(filejoinname, "%s.%03d", str, id);
		FILE *each;
		each = fopen(filejoinname, "rb");
		if(!each) break;
		fseek(each, 0L, SEEK_END);
		rewind(each);
		fread(jpbuf, sizeof(jpbuf), ftell(each), each);
		fwrite(jpbuf, sizeof(jpbuf), ftell(each), joined);
		fclose(each);
		unlink(filejoinname);
		id++;
	}
	fclose(joined);
}

void dispart(char *str){
	char dispart_path[10000000];
    int status;
	sprintf(dispart_path, "%s.", str);
	pid_t cid;
	cid = fork();
	if(cid == 0) {
		char *argv[] = {"split", "-b", "1024", "-d", "-a", "3", str, dispart_path, NULL};
		execv("/usr/bin/split", argv);
	}
	while(wait(&status) > 0);
	unlink(str);
}

int encdFull(char *str){
	char *temp_str2 = strtok(NULL, "/");
	char *temp_str = strtok(str, "/");
	int opsi_mode = 0;
	while(temp_str2){
		char substring1[1024];
		char substring2[1024];
		sub_str(temp_str, substring1, 0, 5);
		sub_str(temp_str, substring2, 0, 3);
		if(!strcmp(substring1, "AtoZ_")) opsi_mode |= 1;
		else if(!strcmp(substring2, "RX_")) opsi_mode |= 2;
		temp_str = temp_str2;
		temp_str2 = strtok(NULL, "/");
	}
	return opsi_mode;
}

int encdFolder(char *str){
	char *temp_str = strtok(str, "/");
	int opsi_mode = 0;
	while(temp_str){
		char substring1[1024];
		char substring2[1024];
		sub_str(temp_str, substring1, 0, 5);
		sub_str(temp_str, substring2, 0, 3);
		if(!strcmp(substring1, "AtoZ_")) opsi_mode |= 1;
		else if(!strcmp(substring2, "RX_")) opsi_mode |= 2;
		temp_str = strtok(NULL, "/");
	}
	return opsi_mode;
}

void EncodeAtoZRecur(char *str, int mode_fg){
	struct dirent *dp;
	DIR *dir = opendir(str);

	if(!dir) return;

	while((dp = readdir(dir)) != NULL){
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            char name[1000000];
        	char newname[1000000];
        	char path[2000000];

        	mergePath(path, str, dp->d_name);
			strcpy(name, dp->d_name);
			if(mode_fg == 1) mergePath(newname, str, encode(name, 1));
			else if(mode_fg == -1) mergePath(newname, str, decode(name, 1));
			if(dp->d_type == DT_REG) rename(path, newname);
			else if(dp->d_type == DT_DIR){
				rename(path, newname);
				EncodeAtoZRecur(newname, mode_fg);
			}
        }
	}
}

void encode1(char *str, int mode_fg){
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	EncodeAtoZRecur(str, mode_fg);
}

void EncodeRXRecur(char *str, int mode_fg){
	struct dirent *dp;
	DIR *dir = opendir(str);

	if(!dir) return;

	while((dp = readdir(dir)) != NULL){
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
			char path[2000000];
        	mergePath(path, str, dp->d_name);
			if(dp->d_type == DT_DIR) EncodeRXRecur(path, mode_fg);
			else if(dp->d_type == DT_REG){
				if(mode_fg == 1) dispart(path);
				if(mode_fg == -1) joinpart(path);
			}
        }
	}
}

void encode2(char *str, int mode_fg){
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	EncodeRXRecur(str, mode_fg);
}

static int xmp_access(const char *path, int mask){
    char fpath[1000];
	int res;
	mergePath(fpath, dirpath, path);
	res = access(checkPath(fpath), mask);
	if (res == -1) return -errno;
	makeLogInfo(1, "ACCESS", fpath, "");
	return 0;
}

static int xmp_readdir(const char *path, void *buf,
                       fuse_fill_dir_t filler, off_t offset,
                       struct fuse_file_info *fi){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res = 0;

	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(checkPath(fpath));
	if (dp == NULL) return -errno;

	int mode_fg = encdFolder(fpath);
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		char nama[1000000];
		strcpy(nama, de->d_name);
		if(mode_fg == 1){
			if(de->d_type == DT_REG) decode(nama, 1);
			else if(de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) decode(nama, 0);
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
		else{
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
	}
	closedir(dp);
    makeLogInfo(1, "READDIR", fpath, "");
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size){
	int res;
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	res = readlink(checkPath(fpath), buf, size - 1);
	makeLogInfo(1, "READLINK", fpath, "");
	if (res == -1) return -errno;
	buf[res] = '\0';
	return 0;
}


static int xmp_unlink(const char *path){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = unlink(checkPath(fpath));
    makeLogWarning("UNLINK", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
	char fpath[1000];
	mergePath(fpath, dirpath, path);

	int res;

	res = mkdir(checkPath(fpath), mode);
	if (res == -1) return -errno;

    char check_substr[1024];
    char check_substr2[1024];
    if(lastName(fpath) == 0) return 0;
    char filePath[1000000];
    strcpy(filePath, lastName(fpath));
    sub_str(filePath, check_substr, 0, 5);
    sub_str(filePath, check_substr2, 0, 3);
	if(strcmp(check_substr, "AtoZ_") == 0){
		encode1(fpath, 1);
	}
	else if(strcmp(check_substr2, "RX_") == 0){
		encode2(fpath, 1);
	}
	makeLog(2, "", fpath);
	makeLogInfo(1, "MKDIR", fpath, "");
	return 0;
}

static int xmp_rmdir(const char *path){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = rmdir(checkPath(fpath));
    makeLogWarning("RMDIR", fpath);
	if (res == -1) return -errno;
	return 0;
}


static int xmp_rename(const char *from, const char *to){
    char ffrom[1000];
	mergePath(ffrom, dirpath, from);

    char fto[1000];
	mergePath(fto, dirpath, to);

	int res;
	res = rename(checkPath(ffrom), checkPath(fto));
	if (res == -1) return -errno;

	int fromm = 0, too = 0;
	char check_substr[1024], check_substr2[1024], check2[1024];
    if(lastName(ffrom) == 0) return 0;
    char filePath[1000000];
    strcpy(filePath, lastName(ffrom));
    sub_str(filePath, check_substr, 0, 5);
    sub_str(filePath, check_substr2, 0, 3);
	if(strcmp(check_substr, "AtoZ_") == 0){
		fromm = 1;
	}
	else if(strcmp(check_substr2, "RX_") == 0){
		fromm = 2;
	}

    if(lastName(fto) == 0) return 0;
    strcpy(filePath, lastName(fto));
    sub_str(filePath, check_substr, 0, 5);
	if(strcmp(check2, "AtoZ_") == 0){
		too = 1;
	}
	else if(strcmp(check2, "RX_") == 0){
		too = 2;
	}

	if(fromm == 0 && too == 1) encode1(fto, 1);
	else if(fromm == 2 && too != 1) encode1(fto, -1);
	else if(fromm == 1 && too != 1) encode1(fto, -1);
	else if(fromm == 2 && too == 2) encode2(fto, 1);
	else if(fromm == 1 && too == 2) encode2(fto, 1);
	else if(fromm == 0 && too == 2) encode2(fto, 1);

	makeLog(1, ffrom, fto);
	makeLogInfo(2, "RENAME", ffrom, fto);

	return 0;
}

static int xmp_symlink(const char *from, const char *to){
    char ffrom[1000];
	mergePath(ffrom, dirpath, from);
	int res;

	res = symlink(from, to);
	makeLogInfo(1, "SYMLINK", ffrom, "");
	if (res == -1)return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = open(checkPath(fpath), fi->flags);
    makeLogInfo(1, "OPEN", fpath, "");
	if (res == -1) return -errno;
	close(res);
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = chmod(checkPath(fpath), mode);
	makeLogInfo(1, "CHMOD", fpath, "");
	if (res == -1) return -errno;
	return 0;
}

static int xmp_fsync(const char *path,
                     int isdatasync,struct fuse_file_info *fi){
    char fpath[1000];
	mergePath(fpath, dirpath, path);
	(void) path;
	(void) isdatasync;
	(void) fi;
	makeLogInfo(1, "FSYNC", fpath, "");
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec tmsc[2]){
	int res;
	char fpath[1000];
	mergePath(fpath, dirpath, path);

	struct timeval tmvl[2];
	tmvl[0].tv_usec = tmsc[0].tv_nsec / 1000;
	tmvl[0].tv_sec = tmsc[0].tv_sec;
	tmvl[1].tv_usec = tmsc[1].tv_nsec / 1000;
	tmvl[1].tv_sec = tmsc[1].tv_sec;

	res = utimes(checkPath(fpath), tmvl);
    makeLogInfo(1, "UTIMENS", fpath, "");
	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = lchown(checkPath(fpath), uid, gid);
	makeLogInfo(1, "CHOWN", fpath, "");
	if (res == -1) return -errno;
	return 0;
}

static int xmp_truncate(const char *path, off_t size){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = truncate(checkPath(fpath), size);
    makeLogInfo(1, "TRUNCATE", fpath, "");
	if (res == -1) return -errno;
	return 0;
}

static int xmp_link(const char *from, const char *to){
    char ffrom[1000];
	mergePath(ffrom, dirpath, from);
	char fto[1000];
	mergePath(fto, dirpath, to);
	int res;

	res = link(from, to);
	makeLogInfo(2, "LINK", ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;

	res = statvfs(checkPath(fpath), stbuf);
	makeLogInfo(1, "STATFS", fpath, "");
	if (res == -1) return -errno;
	return 0;
}

static int xmp_write(const char *path,
                     const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;
	int fd;

	(void) fi;
	fd = open(checkPath(fpath), O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

    makeLogInfo(1, "WRITE", fpath, "");
	close(fd);
	return res;
}

static int xmp_release(const char *path, struct fuse_file_info *fi){
    char fpath[1000];
	mergePath(fpath, dirpath, path);
	(void) path;
	(void) fi;
	makeLogInfo(1, "RELEASE", fpath, "");
	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
    (void) fi;

    int res;
    res = creat(checkPath(fpath), mode);
    if(res == -1) return -errno;

    makeLogInfo(1, "CREATE", fpath, "");
    close(res);
    return 0;
}


static int xmp_getattr(const char *path, struct stat *stbuf){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	int res;
	res = lstat(checkPath(fpath), stbuf);
	makeLogInfo(1, "GETATTR", fpath, "");
	if (res == -1) return -errno;
	return 0;
}

static int xmp_read(const char *path, char *buf,
                    size_t size, off_t offset,
                    struct fuse_file_info *fi){
	char fpath[1000];
	int res = 0;
	int fd = 0;
	mergePath(fpath, dirpath, path);

	(void) fi;
	fd = open(checkPath(fpath), O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;
	close(fd);

    makeLogInfo(1, "READ", fpath, "");
	return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	checkPath(fpath);
	int res;

	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	}
	else if (S_ISFIFO(mode)){
        res = mkfifo(fpath, mode);
	}
	else {
        res = mknod(fpath, mode, rdev);
	}
	if (res == -1) return -errno;

    makeLogInfo(1, "MKNOD", fpath, "");
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
