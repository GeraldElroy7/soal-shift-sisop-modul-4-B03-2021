# soal-shift-sisop-modul-4-B03-2021

### Anggota Kelompok
1. Nur Putra Khanafi     05111940000020
2. Gerald Elroy Van Ryan 05111940000187
3. Husnan                05111940007002

## Soal 1

[SourceCodeFS](https://github.com/GeraldElroy7/soal-shift-sisop-modul-4-B03-2021/blob/main/SinSeiFS_B03.c)

Pada soal, *user* diminta untuk membantu Sin dan Sei membuat filesystem dengan metode encode yang mutakhir.

### Cara Pengerjaan 1A

*User* diminta untuk me-encode direktori yang dibuat dengan awalan “AtoZ_” dengan Atbash cipher(mirror).

```c
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
```

Pada proses pembuatan(`mkdir`) direktori, sistem akan mengecek awalan nama dari folder untuk dilakukan perubahan jika memenuhi syarat.

```c
void encode1(char *str, int mode_fg){
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	EncodeAtoZRecur(str, mode_fg);
}
```
Ketika masuk ke proses encode, direktori akan dicek apakah termasuk jenis direktori pada umumnya untuk melanjutkan proses.

```c
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
```
Direktori akan di encode mirror dengan bantuan variabel key yaitu abjad uppercase dan lowercase.

#### Output
![1af](https://user-images.githubusercontent.com/65794806/121807048-1631a100-cc85-11eb-973f-774e566d15d0.png)

### Cara Pengerjaan 1B

Kali ini, *user* diminta untuk me-encode direktori yang direname dengan awalan “AtoZ_” dengan Atbash cipher(mirror).

```c
static int xmp_rename(const char *from, const char *to){
    char ffrom[1000];
	mergePath(ffrom, dirpath, from);

    char fto[1000];
	mergePath(fto, dirpath, to);

	int res;
	res = rename(checkPath(ffrom), checkPath(fto));
	if (res == -1) return -errno;
...
}
```
Pada proses rename, path awal dan path baru akan dicek, jika memenuhi syarat maka, proses akan dilanjutnya sebagai folder dengan awalan "AtoZ_".
Proses selanjutnya akan sama dengan proses 1A.

#### Output
![1bf](https://user-images.githubusercontent.com/65794806/121807057-1e89dc00-cc85-11eb-8e19-5314e2869e27.png)

### Cara Pengerjaan 1C

Pada soal 1C, *user* diminta untuk me-decode direktori yang direname menjadi tidak berawalan "AtoZ_".

```c
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
...
}
```

Pada proses pembacaan direktori, direktori akan dicek dengan ` encdFolder()` untuk mendapatkan status dari direktori.

```C
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
```

Selanjutnya akan dilakukan proses decode yang algoritmanya sama dengan encode karena mirror.

#### Output
![1cf](https://user-images.githubusercontent.com/65794806/121807066-277aad80-cc85-11eb-949e-966b5ccdb2ab.png)

### Cara Pengerjaan 1D

Pada *problem* 1D, *user* juga diminta untuk mencatat setiap pemanggilan system rename dan mkdir pada log.

```c
void makeLog(int opsi, char* path1, char* path2){
    if(opsi == 1){
            char logdata[1000];
            sprintf(logdata, "RENAME: %s --> %s", path1, path2);
            FILE *result = fopen("/home/nurputra/AtoZ_log", "a");
            fprintf(result, "%s\n", logdata);
            fclose(result);
    }else if(opsi == 2){
            char logdata[1000];
            sprintf(logdata, "MKDIR : %s", path2);
            FILE *result = fopen("/home/nurputra/log", "a");
            fprintf(result, "%s\n", logdata);
            fclose(result);
    }

}
```

```c
static int xmp_rename(const char *from, const char *to){
  ...

	makeLog(1, ffrom, fto);
  ...

	return 0;
}
```

```c
static int xmp_mkdir(const char *path, mode_t mode){
	...
  
	makeLog(2, "", fpath);
	...
	return 0;
}
```

Pada fungsi, jika ada pemanggilan rename, maka akan masuk ke opsi 1 dengan mengambil 2 path yaitu path awal dan path akhir hasil rename. Untuk mkdir, akan masuk ke opsi 2.


#### Output
![1df](https://user-images.githubusercontent.com/65794806/121807072-2d708e80-cc85-11eb-97de-cbe473e86aa3.png)

### Cara Pengerjaan 1E

Pada soal 3E, *user* diminta untuk me-encode dan decode terhadap direktori yang ada di dalamnya dengan cara rekursi.

```c
void EncodeAtoZRecur(char *str, int mode_fg){
	struct dirent *dp;
	DIR *dir = opendir(str);

	if(!dir) return;

	while((dp = readdir(dir)) != NULL){
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
      ...
      
			else if(dp->d_type == DT_DIR){
				rename(path, newname);
				EncodeAtoZRecur(newname, mode_fg);
			}
    }
	}
}
```

Pada proses ini, dalam dari direktori akan di cek dan dibuat penamaan baru yang nanti akan masuk kembali ke dalam direktorinya lagi sesuai dengan status `mode_fg`.


#### Output
![1ef](https://user-images.githubusercontent.com/65794806/121807079-35303300-cc85-11eb-8305-4a5287c97fe7.png)

### Kendala Selama Pengerjaan

1. Sering terdapat dump error sehingga sangat sulit untuk mendeteksi bagian yang erro pada source code.


## Soal 2
Belum dapat diselesaikan.

### Kendala Selama Pengerjaan

1. Terdapat dump error sehingga sangat sulit untuk mendeteksi bagian yang erro pada source code.
2. Proses delay antara pengecekan log dan perubahan nama direktori yang tidak sesuai.

## Soal 3
Belum dapat diselesaikan.

### Kendala Selama Pengerjaan

1. Soal 3 dapat dikerjakan tuntas jika soal 2 telah selesai.

## Soal 4

Pada soal, *user* diminta untuk mencatat semua pemanggilan system kedalam log SinSeiFS.

### Cara Pengerjaan 4A

*User* diminta untuk menyimpan log di direktori home pengguna.

```c
...
    FILE *result = fopen("/home/[nama user]/SinSeiFS.log", "a");
...
```

Log dibuat dan disimpan dengan `fopen` pada path home.

#### Output
![4a](https://user-images.githubusercontent.com/65794806/121807106-614bb400-cc85-11eb-89ef-e60e55ce3b90.png)

### Cara Pengerjaan 4B

*user* diminta untuk membuat log yang isinya dua level, yaitu INFO dan WARNING.
```c
void makeLogWarning(char *txt, char* path){
    ...
    char* inlev = "WARNING";
    char log[1000];
    ...
    
	
    sprintf(log, "%s...", inlev, ...);
	  FILE *result = fopen("/home/nurputra/SinSeiFS.log", "a");
    fprintf(result, "%s\n", log);
    fclose(result);
}
```

```c
void makeLogInfo(int opsi, char *txt, char* path, char* path2){
        ...
        char* inlev = "INFO";
        ...
        
        sprintf(logdata, "%s...", inlev,...);
        FILE *result = fopen("/home/nurputra/SinSeiFS.log", "a");
        fprintf(result, "%s\n", logdata);
        fclose(result);
}
```

Akan dibuat variabel untuk menyimpan kata "WARNING" dan "INFO" yang nantinya akan dimasukkan di log sebagai awalan tiap baris.

#### Output
![4b](https://user-images.githubusercontent.com/65794806/121807113-690b5880-cc85-11eb-9c0c-10a44a37fa41.png)


### Cara Pengerjaan 4C

Pada 4C, *user* diminta untuk mencatat syscall rmdir dan unlink sebagai log level WARNING. Sehingga pada proses system rmdir dan unlink, akan disisipkan fungsi pembuatan log Warning.

```c
static int xmp_rmdir(const char *path){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	...
  
  makeLogWarning("RMDIR", fpath);
  
	...
	return 0;
}
```

```c
static int xmp_unlink(const char *path){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	...

   makeLogWarning("UNLINK", fpath);
	
  ...
	return 0;
}
```

#### Output
![4c](https://user-images.githubusercontent.com/65794806/121807121-6f99d000-cc85-11eb-9523-28d277dc41b7.png)

### Cara Pengerjaan 4D

Pada soal 4D, log level INFO adalah sisa system selain unlink dan rmdir. Sehingga pada proses system yang lainnya, akan disisipkan fungsi pembuatan log Info. Contohnya pada dua System ini.

```c
static int xmp_mkdir(const char *path, mode_t mode){
	char fpath[1000];
	mergePath(fpath, dirpath, path);
	...
  
	makeLogInfo(1, "MKDIR", fpath, "");
	return 0;
}
```

```c
static int xmp_rename(const char *from, const char *to){
  char ffrom[1000];
	mergePath(ffrom, dirpath, from);

  char fto[1000];
	mergePath(fto, dirpath, to);
	...
  
	makeLogInfo(2, "RENAME", ffrom, fto);

	return 0;
}
```

#### Output
![4d](https://user-images.githubusercontent.com/65794806/121807127-76284780-cc85-11eb-9567-781519a572cd.png)

### Cara Pengerjaan 4E

Pada soal 4E, *user* diminta untuk melakukan pencatatan dengan format,
[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

```c
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
```

Contohnya pada log Warning, dilakukan pengambilan waktu saat itu dengan struct tm untuk localtimenya, lalu menggunakan `strftime(...,"%d%m%Y-%H:%M:%S", ...)`. Kemudian varibel yang menyimpan kata "WARNING" ditempatkan di paling depan, setelah itu format waktu, kemudian nama System dan terakhir path yang sedang diproses.

```c
void makeLogInfo(int opsi, char *txt, char* path, char* path2){
    if(opsi == 1){
        ...
        
        sprintf(logdata, "%s::%s::%s::%s", inlev, this_tm, txt, path);
        
        ...
        fclose(result);
    }
    else if(opsi == 2){
        ...
        
        sprintf(logdata, "%s::%s::%s::%s::%s", inlev, this_tm, txt, path, path2);
        
        ...
        fclose(result);
    }
}
```

Pada log Info, ada dua opsi yaitu 1 untuk system yang memproses satu path saja seperti create dan 2 untuk sytem yang memproses dua path seperti rename.

#### Output
![4e](https://user-images.githubusercontent.com/65794806/121807133-7c1e2880-cc85-11eb-9657-e222faa7d50d.png)

### Kendala Selama Pengerjaan

Tidak ada.
